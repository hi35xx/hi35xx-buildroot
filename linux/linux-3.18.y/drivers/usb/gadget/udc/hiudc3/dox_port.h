#ifndef _DWC_DOX_PORT_H_
#define _DWC_DOX_PORT_H_

/**
 * @file
 *
 * image html synopsys.png "Synopsys Logo"
 * image latex synopsys.eps "Synopsys Logo"
 *
 * This file contains the Doxygen comments that create the Porting Guide
 * document for the driver.
 *
 * @page intro_chap Introduction
 *
 * This document gives some tips on how to port the DWC_usb3 example drivers to
 * an operating system other than Linux. This document focuses on the Peripheral
 * Controller Driver (PCD) in the \a driver/ source directory. The OTG3 and UAS
 * Gadget drivers in the \a otg3/ and \a gadget/ directories are not covered.
 *
 * The PCD source code includes an example platform port in the \a driver/no_os/
 * directory. \a no_os stands for "No OS", which means the code contains no
 * OS-specific dependencies at all.
 *
 * The \a no_os/ code also contains an implementation of a simple loopback-class
 * Function Driver. This driver provides one Bulk OUT and one Bulk IN endpoint,
 * and can either loop all packets received on the OUT endpoint back to the IN
 * endpoint, or can sink any packets received on the OUT endpoint while
 * simultaneously sourcing 0-filled packets on the IN endpoint. One of these two
 * behaviors can be selected at compile time by changing a single line of source
 * code.
 *
 * @page pcd_chap The PCD
 *
 * @section pcd_rules_sec Rules When Calling Into the PCD
 *
 * - The @ref dwc_usb3_pcd_init API routine in pcd.c must be called from process
 * context or equivalent, because it uses the @ref dwc_msleep routine to delay
 * while waiting for the DWC_usb3 core to come out of reset. The same is true
 * for @ref dwc_usb3_pcd_device_init if its <strong><em>soft_reset</em></strong>
 * parameter is <strong>true</strong>.
 *
 * - To protect against the IRQ handler corrupting core registers or data
 * structures, interrupts should be disabled before calling any of the
 * non-initialization routines in the PCD API. In addition, on multiprocessor
 * platforms a spinlock should also be taken, to protect against two CPUs
 * running at the same time and causing corruption. The <em>lock</em> field in
 * struct dwc_usb3_pcd is provided for this purpose.
 *
 * - If the above spinlock is used, then it must be released in the @ref
 * dwc_usb3_gadget_complete API routine before calling any Function Driver
 * routines which may try to take the spinlock again to call back into the PCD.
 * The spinlock must be retaken before @ref dwc_usb3_gadget_complete returns.
 *
 * @section comp_pcd_sec Porting the PCD
 *
 * First, try porting just the PCD code, which is cil.c, cil_intr.c, pcd.c,
 * pcd_intr.c, pcd_hiber.c, and ep0.c. This code should be easy to port because
 * it uses very few OS services. The following sections will walk you through
 * how to do this.
 *
 * @subsection os_defs_and_dev_sec Create the os_defs.h and os_dev.h Files
 *
 * Create \a os_defs.h and \a os_dev.h files for your platform. You can
 * either create a subdirectory (e.g. \a driver/myplat/) to contain these
 * files, and replace the existing \a os_defs.h and \a os_dev.h symlinks with
 * links to these files, or you can remove the symlinks and create these files
 * directly in the \a driver/ directory.
 *
 * We recommend that you start with the no_os_defs.h and no_os_dev.h files and
 * modify them as needed for your platform.
 *
 * @subsubsection os_defs_sec The os_defs.h File
 *
 * The \a os_defs.h file should contain all the \a \#include directives
 * needed to compile the PCD code on your platform, as well as definitions
 * needed to provide any missing types etc. that the PCD code requires.
 *
 * Specific things that need to be defined here include:
 * <ul>
 *  <li> Definitions of the types \a u8, \a u16, \a u32, \a u64, \a u_int8_t,
 *  \a u_int16_t, \a u_int32_t, \a u_char, \a u_short, \a u_int, and \a
 *  u_long.
 *
 *  <li> Definition of the type \a dwc_dma_t to hold a DMA address for your
 *  platform. This must be an integral type, not a pointer.
 *
 *  <li> Definition of the \a UPACKED attribute for the compiler. If you are
 *  using GCC, or ARMCC with the \a --gnu flag, this should be:
 *  <code><p> &nbsp;&nbsp;&nbsp; \#define UPACKED __attribute__ ((__packed__))
 *  </p></code>
 *
 *  <p> If you are using a Microsoft compiler, you should define this to
 *  nothing: </p>
 *  <code><p> &nbsp;&nbsp;&nbsp; \#define UPACKED </p></code>
 *
 *  <p> The \a \#include&nbsp;<pshpack1.h> and \a \#include&nbsp;<poppack.h>
 *  directives, as shown in the usb.h file, handle structure packing for
 *  Microsoft compilers. </p>
 *
 *  <p> If you are using some other compiler, check the compiler manual to see
 *  how to accomplish this. </p>
 *
 *  <li> Definition of the \a __iomem data attribute. For any platform except
 *  Linux, this should be defined to nothing:
 *  <code><p> &nbsp;&nbsp;&nbsp; \#define __iomem </p></code>
 *
 *  <li> Definition of the @ref wmb macro (write memory barrier). For
 *  uniprocessor platforms, this can be defined to a null function. For
 *  multiprocessor platforms, the definition of this macro is highly
 *  platform-dependent.
 *
 *  <p> This macro is only used by the DWC_usb3 hibernation code, so
 *  if you are not using the hibernation feature you can safely
 *  define it to a null function: </p>
 *  <code><p> &nbsp;&nbsp;&nbsp; \#define wmb() do {} while (0) </p></code>
 *
 *  <li> Definitions of the usb_request and usb_ep data types. The content of
 *  these structures is completely user-defined, but they must be defined here
 *  because the PCD embeds them into its own request and endpoint structures.
 *  This allows them to be passed back to the platform code when the PCD
 *  completes a USB transfer.
 *
 *  <li> Definitions of all the \a DWC_E_* error codes that the PCD uses. If
 *  your platform uses POSIX error codes, they can be easily mapped to the
 *  \a DWC_E_* error codes.
 *
 *  <p> The only error codes with any operational significance are \a
 *  DWC_E_NOT_SUPPORTED, \a DWC_E_IN_PROGRESS, and \a DWC_E_SHUTDOWN, so these
 *  three error codes must be distinct, while the rest can be mapped to a
 *  single value if so desired. </p>
 *
 *  <p> See the no_os_defs.h file for a complete list of the DWC_E_* error
 *  codes. </p>
 *
 *  <li> Definition of the dwc_usb3_core_params struct. This contains an entry
 *  for each option that the PCD driver supports. It is defined here so that you
 *  can add additional options for your platform code if you wish. See the
 *  no_os_defs.h file for a minimal implementation of this structure.
 * </ul>
 *
 * @subsubsection os_dev_sec The os_dev.h File
 *
 * Because the \a os_defs.h file is included first by the PCD code, it does not
 * have access to any of the definitions in the PCD header files. Therefore, any
 * of your definitions which need such access should be placed in the \a
 * os_dev.h file instead, which is included last by the PCD code. Specific
 * things that need to be defined here include:
 * <ul>
 *  <li> Definitions of the @ref dwc_rd32 and @ref dwc_wr32 inline functions.
 *  Given an offset from the beginning of the DWC_usb3 core address space to a
 *  particular register, these functions read from or write to that register.
 *
 *  <li> Definitions of the @ref dwc_usb3_get_pcd_req and @ref dwc_usb3_get_pcd_ep
 *  macros. Given a pointer to a usb_request or usb_ep, these macros return a
 *  pointer to the enclosing dwc_usb3_pcd_req or dwc_usb3_pcd_ep struct. For
 *  any compiler that provides the \a offsetof macro, these should be:
 *  <code>
 *  <p> &nbsp;&nbsp;&nbsp; \#define dwc_usb3_get_pcd_req(usbreq)&nbsp;\\ \n
 *  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
 *  ((dwc_usb3_pcd_req_t *)((char *)(usbreq)&nbsp;-&nbsp;\\ \n
 *  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
 *  offsetof(struct dwc_usb3_pcd_req, usb_req))) </p>
 *  <p> &nbsp; </p>
 *  <p> &nbsp;&nbsp;&nbsp; \#define dwc_usb3_get_pcd_ep(usbep)&nbsp;\\ \n
 *  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
 *  ((dwc_usb3_pcd_ep_t *)((char *)(usbep)&nbsp;-&nbsp;\\ \n
 *  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
 *  offsetof(struct dwc_usb3_pcd_ep, usb_ep))) </p>
 *  </code>
 *
 *  <li> Definitions of the @ref dwc_udelay and @ref dwc_mdelay non-sleeping delay
 *  routines. Given a delay value in microseconds or milliseconds respectively,
 *  these routines must delay for at least as long as the delay value.
 *  Non-sleeping means these routines may be called from any context, including
 *  interrupt context, so they must not reschedule or require an interrupt to
 *  indicate the end of the delay. Typically these routines would be implemented
 *  using a calibrated busy loop or a dedicated hardware timer.
 *
 *  <li> Definition of the @ref dwc_msleep sleeping delay routine. Given a delay
 *  value in milliseconds, this routine must delay for at least as long as the
 *  delay value. Sleeping means this routine will only be called from process
 *  context, so it can reschedule or require an interrupt to indicate the end of
 *  the delay. Typically this routine would be implemented using a system timer.
 *
 *  <li> Definitions of the @ref dwc_print\<n\>, @ref dwc_error\<n\>, @ref
 *  dwc_warn\<n\>, @ref dwc_info\<n\>, @ref dwc_isocdbg\<n\>, and @ref dwc_debug\<n\>
 *  logging routines.
 *  Given a \a printf style format string and the number of arguments indicated
 *  by the last digit of the routine's name, they print a message to the system
 *  message log, or to the console.
 *
 *  <p> The different routine names indicate the importance of the message,
 *  where @ref dwc_print\<n\> is the highest importance and @ref dwc_debug\<n\>
 *  is the lowest. </p>
 *
 *  <p> Typically these routines would be implemented using a \a printf type
 *  function if one is available, but the explicit argument count means they
 *  can be implemented without needing a \a varargs facility, if necessary. </p>
 * </ul>
 *
 * @subsection modify_pcd_h_sec Modify the pcd.h File
 *
 * The pcd.h file contains a few platform-dependent definitions, and may need
 * some minor modifications to work on your platform:
 * <ul>
 *  <li> dwc_usb3_pcd_req contains an \a entry field, which is used to link a
 *  request into the queue of requests for an endpoint. If your platform will
 *  use request queuing, then this field may need to be modified.
 *
 *  <p> Since the \a entry field is not used by the PCD, it can be commented out
 *  for the purpose of test-compiling the PCD. </p>
 *
 *  <li> dwc_ep contains a \a queue field, which is the head of the request
 *  queue for an endpoint. Again, this field may need to be modified for your
 *  platform, or it can be commented out for the purpose of test-compiling the
 *  PCD.
 *
 *  <li> dwc_usb3_pcd contains a \a lock field, which is used for protecting
 *  the driver data from simultaneous access by multiple processors. If your
 *  platform is uniprocessor, then this field is not needed and you can comment
 *  it out, otherwise it may need to be modified for your platform.
 *
 *  <p> Since the \a lock field is not used by the PCD, it can be commented out
 *  for the purpose of test-compiling the PCD. </p>
 * </ul>
 *
 * @subsection makefile_or_project_sec Create the Makefile / Project File
 *
 * Create a makefile / project file for your platform, and add \a cil.o, \a
 * cil_intr.o, \a pcd.o, \a pcd_intr.o, \a pcd_hiber.o, and \a ep0.o as the
 * target object files. Because the result will not link successfully yet due
 * to missing Function Driver routines, you should omit the link step, if
 * possible.
 *
 * @subsection test_compile_sec Test Compile the PCD
 *
 * <p> Now try compiling your project. </p>
 *
 * <p> If there are errors during the compile phase, you must modify your \a
 * os_defs.h and \a os_dev.h files, and/or pcd.h, to attempt to eliminate them.
 * </p>
 *
 * <p> You may need to modify some of the other PCD source files to achieve a
 * successful compile. For example, your compiler may require a different method
 * of packing data structures, which may require you to modify usb.h. </p>
 *
 * <p> If there are warnings during the compile phase, you should investigate
 * each one carefully to determine whether it signifies a real problem or not.
 * </p>
 *
 * <p> If your makefile / project file contains a link step, you should expect
 * it to fail, since at this point all of the Function Driver routines are
 * missing. </p>
 *
 * <p> If there are no errors or warnings during the compile phase, then the
 * initial porting phase is complete. </p>
 *
 * @page func_drvr_chap The Function Driver
 *
 * Porting the Function Driver is not as straightforward as the PCD, because
 * there are more external factors to take into account.
 *
 * Because of this, we have provided the No-OS platform port. This port needs
 * minimal support from the operating environment, and is designed to run as
 * close to the hardware as possible. It is also self-contained, implementing
 * its own Function Driver, so no OS support for USB functions is required.
 *
 * The Function implements a simple loopback test mode, where any data received
 * from the host on the OUT endpoint is sent back to the host on the IN
 * endpoint. Or, by changing the second line in
 * no_os_src_sink_lpbk.c:dwc_usb3_function_init() to this:
 *
 * <code><p>&nbsp;&nbsp;&nbsp; loopbk.src_sink = 1; </p></code>
 *
 * the Function will implement a source/sink test mode, which sinks any packets
 * received on the OUT endpoint while simultaneously sourcing 0-filled packets
 * on the IN endpoint.
 *
 * The major limitation of the No-OS port is that all DMA data structures are
 * statically allocated in the C code. This will only work in very simple
 * operating environments where the system memory is mapped with virtual
 * address == physical address. If you have an environment where this is not the
 * case, then at a minimum you will need to replace these static allocations
 * with calls to OS routines to perform proper DMA allocations.
 *
 * To help while debugging the No-OS port, we have included Linux code that is
 * conditionally compiled if the LINUXTEST macro is defined. This allows the
 * No-OS code to compile and run on a Linux platform, as a completely
 * self-contained driver that does not use the Linux Gadget framework. The
 * No-OS port can be built for Linux in this way by invoking "make" as follows:
 *
 * <code><p>&nbsp;&nbsp;&nbsp; make NOOS=1 </p></code>
 *
 * The no_os/ platform code consists of the following files:
 *
 * - no_os_init.c: Initialization code and "bus glue".
 * - no_os_gadget.c: Implements a custom Function Driver API.
 * - no_os_ep0.c: Contains the USB descriptors for the Function.
 * - no_os_src_sink_lpbk.c: Contains an implementation of a simple
 *   source-sink/loopback Function.
 * - no_os_hiber.c: Implements the platform-specific code needed to support the
 *   DWC_usb3 hibernation feature. Note: This code is incomplete, so you should
 *   consider DWC_usb3 hibernation as unsupported by the No-OS Function Driver
 *   at this time.
 *
 * @section func_walk_sec Code Walkthrough of the No-OS Function Driver
 *
 * @subsection func_init_sec Initialization
 *
 * At initialization time, @ref dwc_usb3_driver_init in no_os_init.c is called,
 * which does the following:
 * - Allocates memory for the \a usb3_dev driver data struct.
 * - Maps the DWC_usb3 registers into the processor's memory space.
 * - Calls the @ref dwc_usb3_pcd_check_snpsid API routine to make sure this is a
 *   DWC_usb3 device, and to save the value from the SNPSID register in the
 *   \a usb3_dev struct.
 * - Allocates DMAable memory for the event buffer, and for the EP0 DMA
 *   descriptors and data buffers.
 * - Calls the @ref dwc_usb3_pcd_common_init API routine.
 * - Calls the @ref dwc_usb3_gadget_init API routine in no_os_gadget.c, which
 *   does the following:
 *   - Sets the dwc_ep.num field for each physical endpoint to the USB endpoint
 *     number associated with that endpoint.
 *   - Initializes the dwc_ep.queue field for each physical endpoint. This is
 *     the head of the queue where each transfer request for that endpoint will
 *     be queued.
 * - Calls @ref dwc_usb3_function_init in no_os_src_sink_lpbk.c, which allocates
 *   data buffers for each Bulk endpoint.
 * - Calls the @ref dwc_usb3_pcd_init API routine.
 * - Hooks the @ref dwc_usb3_common_irq top-level interrupt handler routine to
 *   the DWC_usb3 IRQ vector.
 *
 * At this point the DWC_usb3 core and the driver are initialized, and waiting
 * for the Device to be connected to a Host.
 *
 * @subsection func_conn_enum_sec Connection and Enumeration
 *
 * When a connection to a Host is established, a <strong>USBRst</strong> event
 * is generated, followed by a <strong>ConnectDone</strong> event.
 *
 * The <strong>USBRst</strong> event is handled internally by the PCD.
 *
 * The <strong>ConnectDone</strong> event results in the following:
 * - The PCD calls the @ref dwc_usb3_gadget_connect API routine in no_os_gadget.c.
 * - @ref dwc_usb3_gadget_connect in turn calls @ref dwc_usb3_function_connect
 *   in no_os_src_sink_lpbk.c.
 * - @ref dwc_usb3_function_connect sets the function's Bulk max packet size
 *   according to the connection speed.
 *
 * Once the connection sequence completes, the Host starts sending Control
 * requests to the Device to enumerate it. The general flow for Control requests
 * is as follows:
 * - The PCD code in ep0.c handles most Control requests internally.
 * - For those requests it cannot handle, the PCD calls the
 *   @ref dwc_usb3_gadget_setup API routine in no_os_gadget.c.
 * - @ref dwc_usb3_gadget_setup in turn calls @ref dwc_usb3_no_os_setup in
 *   no_os_ep0.c.
 * - @ref dwc_usb3_no_os_setup handles <strong>GET_DESCRIPTOR</strong> Control
 *   requests (except for the BOS descriptor, which is handled by ep0.c), using
 *   the information encoded in the USB descriptors in no_os_ep0.c. See @ref
 *   usb_desc_sec for details.
 * - For all remaining Control requests, the no_os_ep0.c code calls @ref
 *   dwc_usb3_function_setup in no_os_src_sink_lpbk.c.
 *
 * @subsection func_config_sec Configuration
 *
 * Once the Host has enumerated the Device, it will configure it as follows:
 * - The host sends a <strong>SET_CONFIGURATION</strong> Control request with a
 *   <strong>bConfigurationValue</strong> of <strong>1</strong> (since that is
 *   the value given in the Config descriptors in no_os_ep0.c), followed by a
 *   <strong>SET_INTERFACE</strong> Control request with a
 *   <strong>bInterfaceNumber</strong> of <strong>0</strong> and a
 *   <strong>bAlternateSetting</strong> of <strong>0</strong> (since those are
 *   the values given in the Interface descriptors in no_os_ep0.c).
 * - <strong>SET_CONFIGURATION</strong> and <strong>SET_INTERFACE</strong> are
 *   two of the Control requests that are handled by @ref dwc_usb3_function_setup
 *   in no_os_src_sink_lpbk.c, so that routine will be called.
 * - @ref dwc_usb3_function_setup in turn calls @ref set_interface (in the same file)
 *   with an <strong><em>alt</em></strong> parameter of <strong>0</strong>.
 * - @ref set_interface calls @ref enable_eps (in the same file).
 * - @ref enable_eps calls @ref dwc_usb3_ep_enable in no_os_gadget.c for each of
 *   the Bulk endpoints, with pointers to the USB descriptors from no_os_ep0.c
 *   corresponding to the speed of the connection.
 * - @ref dwc_usb3_ep_enable does the following:
 *   - Gets the endpoint info from the Endpoint descriptor for that EP.
 *   - Decides how many TRBs to allocate for the EP.
 *   - Calls the @ref dwc_usb3_pcd_trb_alloc API routine to allocate the TRBs.
 *     Note: @ref dwc_usb3_pcd_trb_alloc will call back to the @ref
 *     dwc_usb3_gadget_alloc_dma API routine in no_os_gadget.c to do the
 *     memory allocation.
 *   - Calls the @ref dwc_usb3_pcd_ep_enable API routine to enable the endpoint
 *     in the DWC_usb3 core.
 * - @ref set_interface then calls @ref dwc_usb3_alloc_request in no_os_gadget.c
 *   repeatedly, to allocate a number of transfer requests for each of the Bulk
 *   endpoints.
 * - @ref dwc_usb3_alloc_request does the following:
 *   - Allocates a PCD request from the gadget's pool of requests.
 *   - Returns a pointer to the usb_request member of the PCD request.
 * - @ref set_interface then fills in a number of fields in each usb_request,
 *   among them the \a complete function pointer, then calls @ref dwc_usb3_ep_queue
 *   in no_os_gadget.c to queue it for transfer.
 * - @ref dwc_usb3_ep_queue does the following:
 *   - Sets the usb_request \a status field to -DWC_E_IN_PROGRESS.
 *   - Fills in a number of required fields in the PCD request.
 *   - Decides whether to start the request immediately, or to queue it to be
 *     started later. If it decides to start the request immediately, it:
 *     - Calls the @ref dwc_usb3_pcd_fill_trbs API routine to fill in the TRB
 *       for the request.
 *     - Calls the @ref dwc_usb3_pcd_ep_submit_req API routine to start the
 *       transfer in the DWC_usb3 core.
 *   - Adds the request to the endpoint's request queue.
 *
 * At this point the Device is in the Configured state, ready to receive Bulk
 * transfers from the Host.
 *
 * @subsection func_oper_sec Operation
 *
 * When the Host requests a Bulk data transfer to or from the Device, the PCD
 * will receive a <strong>XferComplete</strong> event from the DWC_usb3 core
 * when the transfer completes. This will result in the following:
 * - The PCD calls the @ref dwc_usb3_gadget_get_request API routine in
 *   no_os_gadget.c.
 * - @ref dwc_usb3_gadget_get_request returns the request that is at the head of
 *   the endpoint's request queue. This will be the request whose transfer has
 *   just completed.
 * - The PCD then calls the @ref dwc_usb3_gadget_complete API routine in
 *   no_os_gadget.c.
 * - @ref dwc_usb3_gadget_complete removes the request from the endpoint's queue,
 *   fills in the \a status and \a actual fields in the usb_request, and calls
 *   the \a complete function pointer in the usb_request.
 * - The \a complete function pointer points to @ref loopbk_complete in
 *   no_os_src_sink_lpbk.c, which does the following:
 *   - Checks the \a status field in the usb_request. If it is not
 *     <strong>0</strong> then an error or a disconnect occurred, which are
 *     handled appropriately.
 *   - If the \a status field is <strong>0</strong>, then the endpoint is
 *     checked. If it is the OUT EP, @ref dwc_usb3_ep_queue in no_os_gadget.c is
 *     called to requeue the usb_request on the IN EP for sending the data back
 *     to the host. If it is the IN EP, @ref dwc_usb3_ep_queue is called to requeue
 *     the usb_request on the OUT EP for receiving more data from the host.
 * - The PCD then calls the @ref dwc_usb3_gadget_start_next_request API routine
 *   in no_os_gadget.c.
 * - If there are any more requests queued for the endpoint,
 *   @ref dwc_usb3_gadget_start_next_request takes the first one and does the
 *   following:
 *   - Calls the @ref dwc_usb3_pcd_fill_trbs API routine to fill in the TRB for
 *     the request.
 *   - Calls the @ref dwc_usb3_pcd_ep_submit_req API routine to start the
 *     transfer in the DWC_usb3 core.
 *
 * @section usb_desc_sec USB Descriptors
 *
 * no_os_ep0.c contains a minimal set of USB descriptors - a set of String
 * descriptors, a Device descriptor, a Device Qualifier descriptor, and three
 * Configuration descriptors, one for each of the speeds that the DWC_usb3 core
 * supports. Each of the Configuration descriptors contains two Endpoint
 * descriptors, one for a Bulk IN endpoint and one for a Bulk OUT endpoint. The
 * Super Speed Configuration descriptor also contains two Super Speed Endpoint
 * Companion descriptors, one for each of the Bulk endpoints. All of these
 * descriptors may need to be modified for your platform's particular Function
 * implementation.
 *
 * ep0.c contains the required BOS descriptor. Fields that must be modified here
 * are \a bU1DevExitLat and \a wU2DevExitLat in the Super Speed Device
 * Capability descriptor, and \a containerID in the optional Container ID
 * Capability descriptor (if used).
 *
 * The compiler must be instructed not to add padding between the fields in any
 * of these descriptors. This is the purpose of the \a UPACKED attribute macro
 * for the GCC compiler (or the ARMCC compiler with the \a --gnu flag). The
 * \a \#include&nbsp;<pshpack1.h> and \a \#include&nbsp;<poppack.h> directives,
 * as shown in usb.h and no_os_ep0.c, accomplish the same thing for the
 * Microsoft compilers.
 *
 * The exact format for all of these descriptors is described in detail in
 * Chapter 9 of the USB 3.0 specification.
 *
 * @section comp_func_sec Porting the No-OS Function Driver
 *
 * Copy all the .c files from the \a driver/no_os/ directory into the platform
 * directory that you created earlier to hold your \a os_defs.h and \a os_dev.h
 * files (e.g. \a driver/myplat/). Rename all the .c files accordingly (e.g.
 * \a no_os_init.c -> \a myplat_init.c, \a no_os_gadget.c -> \a myplat_gadget.c
 * ...).
 *
 * Modify the source code as necessary to work on your platform.
 *
 * Modify the makefile / project file for your platform and add e.g. \a
 * myplat_init.o, \a myplat_gadget.o ... to the target object files. You should
 * also add a link step if one does not exist already.
 *
 * Now try compiling your project. As with @ref test_compile_sec, you must
 * modify the source files as necessary until the compile phase completes
 * successfully.
 *
 * Once the compile phase is successful, try to resolve any errors that occur
 * in the link phase. Most likely these will be due to missing support routines,
 * such as \a memcpy or \a printf. You must either add libraries to the link step
 * to provide these routines, write your own routines and include them in the
 * build, or modify the driver source files to use alternate versions of these
 * routines.
 *
 * @page pcd_api_chap PCD to Function Driver API
 *
 * @section pcd_funcs_sec API Routines in the PCD
 *
 * These routines are provided by the PCD (and the CIL), and make up the PCD
 * side of the API.
 *
 * @subsection init_funcs_sec Initialization Routines
 *
 * These routines handle initialization of the CIL and PCD driver components
 * and the DWC_usb3 core.
 *
 * See @ref init_api_grp for a detailed description of these routines.
 *
 * - dwc_usb3_pcd_check_snpsid()
 * - dwc_usb3_pcd_common_init()
 * - dwc_usb3_pcd_common_remove()
 * - dwc_usb3_pcd_init()
 * - dwc_usb3_pcd_remove()
 * - dwc_usb3_pcd_device_init()
 * - dwc_usb3_pcd_device_remove()
 *
 * @subsection trb_funcs_sec TRB Routines
 *
 * These routines handle the allocation, deallocation, and setup of TRBs.
 *
 * See @ref trb_api_grp for a detailed description of these routines.
 *
 * - dwc_usb3_pcd_trb_alloc()
 * - dwc_usb3_pcd_trb_free()
 * - dwc_usb3_pcd_fill_trbs()
 *
 * @subsection ep_funcs_sec Endpoint Routines
 *
 * These routines handle all the functionality required for configuring,
 * enabling, controlling, and submitting transfers to an endpoint.
 *
 * Note: For Control endpoint 0, only the submit_req, cancel_req, request_done,
 * and set_halt routines are used; the remaining functionality is handled either
 * by the @ref ep0_funcs_sec below or internally by the PCD.
 *
 * See @ref ep_api_grp for a detailed description of these routines.
 *
 * - dwc_usb3_pcd_ep_enable()
 * - dwc_usb3_pcd_ep_disable()
 * - dwc_usb3_pcd_ep_submit_req()
 * - dwc_usb3_pcd_ep_cancel_req()
 * - dwc_usb3_pcd_request_done()
 * - dwc_usb3_pcd_ep_start_transfer()
 * - dwc_usb3_pcd_ep_set_stall()
 * - dwc_usb3_pcd_ep_clear_stall()
 * - dwc_usb3_pcd_ep_set_halt()
 *
 * @subsection ep0_funcs_sec Control Endpoint 0 Routines
 *
 * These routines are only used for Control endpoint 0.
 *
 * See @ref ep0_api_grp for a detailed description of these routines.
 *
 * - dwc_usb3_pcd_ep0_out_start()
 * - dwc_usb3_pcd_ep0_start_transfer()
 * - dwc_usb3_pcd_ep0_continue_transfer()
 * - dwc_usb3_pcd_ep0_data_stage()
 *
 * @subsection misc_funcs_sec Miscellaneous Routines
 *
 * These are miscellaneous routines that don't fit into any of the other
 * categories.
 *
 * See @ref misc_api_grp for a detailed description of these routines.
 *
 * - dwc_usb3_pcd_get_ep_by_addr()
 * - dwc_usb3_pcd_get_frame_number()
 * - dwc_usb3_pcd_isoc_ep_hiber_restart()
 * - dwc_usb3_pcd_stop()
 * - dwc_usb3_pcd_get_link_state()
 * - dwc_usb3_pcd_set_link_state()
 * - dwc_usb3_pcd_remote_wake()
 * - dwc_usb3_pcd_do_test_mode()
 *
 * @section gadget_funcs_sec API Routines in the Function Driver
 *
 * These routines need to be implemented by the platform code, and make up the
 * Function Driver side of the API.
 *
 * @subsection gadget_notif_sec Function Driver Notification Routines
 *
 * These routines receive notifications from the PCD when certain events occur
 * which the Function Driver may need to be aware of.
 *
 * See @ref gadget_notif_grp for a detailed description of these routines.
 *
 * - dwc_usb3_gadget_connect()
 * - dwc_usb3_gadget_disconnect()
 * - dwc_usb3_gadget_suspend()
 * - dwc_usb3_gadget_resume()
 * - dwc_usb3_gadget_setup()
 * - dwc_usb3_gadget_complete()
 *
 * @subsection gadget_callbk_sec Function Driver Callback Routines
 *
 * The PCD calls these routines when it needs something from the Function
 * Driver.
 *
 * See @ref gadget_callbk_grp for a detailed description of these routines.
 *
 * - dwc_usb3_gadget_alloc_dma()
 * - dwc_usb3_gadget_free_dma()
 * - dwc_usb3_gadget_get_request()
 * - dwc_usb3_gadget_start_next_request()
 * - dwc_usb3_gadget_isoc_ep_start()
 * - dwc_usb3_gadget_request_nuke()
 * - dwc_usb3_gadget_set_ep_not_started()
 *
 */

#endif /* _DWC_DOX_PORT_H_ */
