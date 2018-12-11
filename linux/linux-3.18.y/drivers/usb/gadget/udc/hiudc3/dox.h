#ifndef _DWC_DOX_H_
#define _DWC_DOX_H_

/**
 * @file
 *
 * image html synopsys.png "Synopsys Logo"
 * image latex synopsys.eps "Synopsys Logo"
 *
 * This file contains the Doxygen comments that create the Functional
 * Specification document for the driver.
 *
 * @page intro_chapter Introduction
 *
 * The DWC_usb3 Subsystem Version 2.90b is a host and peripheral controller
 * that is compliant with the USB 3.0 specification. The peripheral controller
 * has a Synopsys-proprietary programming interface, which bears some
 * resemblance to the Intel xHCI host interface. This is because the controller
 * supports xHCI when acting as a host, and some of the logic is shared between
 * the host and peripheral modes.
 *
 * The 2.90b version supports SuperSpeed USB 3.0, and HS and FS USB 2.0.
 *
 * Source level driver software will be shipped with the controller. Customers
 * can use this software as a reference for developing drivers for other
 * platforms or operating environments.
 *
 * @section scope_sec Scope
 *
 * This document defines the functionality of the driver software that will
 * be shipped with the DWC_usb3 Version 2.90b controller. It specifies what
 * the software will do, not how it will accomplish these tasks.
 *
 * The driver software for DWC_usb3 Version 2.90b will support only Linux as
 * its operating environment. Therefore, this document currently defines
 * behavior under Linux. However, it is desired that many aspects of the driver
 * software can be easily ported to other operating systems or environments,
 * and towards that end, it has been written in a way to ease the porting
 * effort (see section 1.2.3 @ref env_and_hw_deps_subsec).
 *
 * For subsequent releases of the DWC_usb3 controller, this document will be
 * modified to include updates required in the driver software. These updates
 * may be made to support new features in the controller or to support new
 * operating environments.
 *
 * @section overview_sec Overview
 *
 * @subsection sw_arch_subsec Software Architecture
 *
 * The DWC_usb3 controller can act as either a host or a peripheral on the
 * USB. Figure 1.1 shows the software architecture for the DWC_usb3 controller.
 *
 * There are two stacks in the software architecture - the Host Stack and the
 * Peripheral Stack. Brief descriptions of each of these stacks are given
 * below to set the context for the driver software. Since the host part of
 * the DWC_usb3 controller conforms to the xHCI standard, the xHCI HCD
 * driver in the host OS will provide the host stack functionality. Therefore,
 * only the peripheral stack functionality will be implemented in the DWC_usb3
 * controller driver software. See the OTG 3.0 user manual for a description
 * of the DRD/OTG functionality.
 *
 * The Host Stack is used to request transfers to or from USB devices when
 * the DWC_usb3 component is acting in the role of a host. The top-level
 * component in this stack is a Host Application that acts as a producer or
 * consumer of data. The Class Drivers translate application requests into a
 * protocol specific to a certain type (or class) of devices. They use I/O
 * Request Packets (IRPs) to transfer data to or from USB devices. The USB
 * Driver (USBD) provides services to allow multiple Class Drivers to
 * configure, control, and exchange data with their associated devices. The
 * Hub Driver is also involved in the configuration process. The USBD handles
 * all communication with the Host Controller Driver (HCD), which must
 * understand the hardware architecture of the host controller. The HCD
 * interacts with the host controller hardware to execute the USB transfers
 * requested by the USBD.
 *
 * The Peripheral Stack is used to respond to requests received from a USB
 * host when the DWC_usb3 component is acting in the role of a peripheral.
 * The Peripheral Function is the sink or source of data requested by the host.
 * The Function Driver handles some USB requests directly. It also provides
 * endpoint read/write data interfaces and notification services to the
 * Peripheral Function. The Peripheral Controller Driver (PCD) understands
 * the hardware architecture of the peripheral controller. The PCD interacts
 * with the peripheral controller hardware to transfer data via the USB and
 * notifies the Function Driver of USB requests.
 *
 * @image html sw_stack.png "Figure 1.1: Generic USB Software Architecture"
 * @image latex sw_stack.eps "Generic USB Software Architecture" width=6.50in
 *
 * @subsection driver_sw_comp_subsec Driver Software Components
 *
 * There are two main driver software components - the Peripheral Controller
 * Driver and the Core Interface Layer (CIL). Basic functionality of the PCD
 * is described in section 1.2.1 @ref sw_arch_subsec above. A little more
 * elaboration of the PCD and a description of the CIL are given below.
 *
 * The PCD and CIL can be viewed as a hardware abstraction layer. In other
 * words, the Function Driver does not know or care about the underlying
 * hardware of the peripheral controller. It merely transfers data and
 * transmits commands via a software interface with the PCD. Changes made to
 * the peripheral controller do not require changes to this interface
 * (although the internal operation of the PCD and CIL would have to be
 * modified).
 *
 * The CIL provides common services for accessing and managing the DWC_usb3
 * hardware. These services include initialization, mapping of registers,
 * interrupt control, and low-level access to the CSRs and DMA descriptors
 * (TRBs).
 *
 * @subsection env_and_hw_deps_subsec Environment and Hardware Dependencies
 *
 * As noted above, changes to the underlying hardware will require changes to
 * the internal operation of the driver components. This is true for both of the
 * driver components (PCD and CIL). Both of these components are aware of the
 * internal architecture of the controller, so each of these components may need
 * to change to adapt to any hardware changes. However, for a given operating
 * system (such as Linux), the layers above the PCD would not require any
 * changes. They would continue to use the same API to communicate with the PCD.
 * That is why the PCD is considered a hardware abstraction layer.
 *
 * Most of the driver components are designed to be reasonably operating system
 * independent, by making the low-level routines OS-agnostic and placing them
 * into separate source files. The Linux-specific code is likewise contained in
 * a few separate source files. That should simplify porting the driver software
 * between operating environments.
 *
 * Starting with the 2.20a release, the software provides support for an
 * additional platform, called No-OS. No-OS means the code contains no
 * OS-specific dependencies at all. See the Porting Guide document for more
 * information about the No-OS platform, and for general information about
 * porting the driver to platforms other than Linux.
 *
 * Note, however, that the driver has only been completely tested under Linux
 * 3.6.3.
 *
 * @section deliverables_sec Deliverables
 *
 * This section describes the driver components, documentation, and demo
 * software included with the SuperSpeed USB3 Controller Linux Driver
 * Software.
 *
 * @subsection driver_sw_subsec Driver Software
 *
 * All driver components will be developed for the Linux operating
 * system on a PC + HAPS platform. The following components will be
 * packaged and released:
 *
 * - Peripheral Controller Driver source code
 * - Core Interface Layer source code
 * - USB Attached SCSI Protocol (UASP) Gadget Driver source code
 *
 * These components will support the following features:
 *
 * - Control and Bulk transfers in Peripheral Mode using the legacy Mass
 *   Storage Class protocol
 * - Control and Bulk transfers in Peripheral Mode using the new USB Attached
 *   SCSI protocol
 *
 * New for the 2.50b release:
 *
 * - "Bringup" driver source code. This is a stripped-down version of the PCD,
 *   with just enough logic for testing the registers, event interrupt, and DMA
 *   loopback features of the controller. It is intended to verify the basic
 *   functionality of the controller without needing any USB-specific support
 *   code in the OS.
 *
 *   The bringup driver source, along with a README file describing how to build
 *   and run the driver, can be found in the bringup/ directory.
 *
 * @subsection sw_doc_subsec Software Documentation
 *
 * Source documentation in HTML or PDF format will be delivered for the driver
 * components released with the DWC_usb3 controller. This will include
 * documentation of the API for each of the driver components.
 *
 * Instructions for acquiring and installing the Linux kernel sources and
 * applying any required patches will be included in the release.
 *
 * @subsection demo_sw_subsec Demo Software
 *
 * No need for special demo software is anticipated for the 2.90b release.
 * The demo will consist of using the controller as a mass-storage device,
 * and copying/streaming large files to and from the device from a host PC.
 *
 * @subsection bin_subsec Binaries
 *
 * No binaries will be distributed with the 2.90b release.
 *
 */

/**
 * @page env_chapter Environment Specific Features
 *
 * This section defines functionality of the driver software that depends on
 * a particular operating system or operating environment. Since Linux is the
 * only environment currently supported, the Linux implementation is described
 * here. In the future, this section may include descriptions of wrappers to
 * glue generic driver components into various operating system environments.
 *
 * @section linux_arch_sec Linux Architecture
 *
 * The Linux architecture is very similar to the generic DWC_usb3 software
 * architecture shown in Figure 1.1. Figure 2.1 shows the Linux version of this
 * architecture.
 *
 * The USB Core component in Linux includes the USB Driver functionality plus a
 * framework to support Host Controller Drivers. The HCD framework is intended
 * to allow different HCDs to share code. This makes it easier to write new
 * HCDs and reduces the number of bugs. Only functionality that needs to be
 * aware of the underlying hardware is implemented in the HCD. This is
 * accomplished by defining the hc_driver interface. This interface consists of
 * the functions and data that must be supplied by an HCD in order to plug into
 * the Linux HCD framework. The OS-supplied xHCI HCD will implement the
 * hc_driver interface.
 *
 * In Linux, peripheral devices are called gadgets. So the Peripheral Function
 * is called a Gadget Application and the Function Driver is called a Gadget
 * Driver. Linux defines a Gadget API which is the interface between the Gadget
 * Driver and the Peripheral Controller Driver. Similar to the hc_driver
 * interface, the Gadget API is intended to isolate hardware-specific behavior
 * to the Peripheral Controller Driver. The Linux version of the DWC_usb3 PCD
 * will implement the Gadget API. See section 4.2.1 @ref gadget_api_subsec for
 * more information.
 *
 * The API of the Core Interface Layer is independent of the operating system.
 * Services provided by this component are the same regardless of the operating
 * system.
 *
 * @image html linux_stack.png "Figure 2.1: Linux USB Software Architecture with UASP Gadget"
 * @image latex linux_stack.eps "Linux USB Software Architecture with UASP Gadget" width=6.50in
 *
 * @section linux_driver_sec Linux Driver Module
 *
 * All of the driver components are contained in a single driver module. The
 * module wrapper code, including the module initialization and cleanup
 * functions, is Linux dependent. This section describes the Linux module
 * functionality.
 *
 * The dwc_usb3 module provides the initialization and cleanup entry points for
 * the DWC_usb3 driver. This module will be dynamically installed after Linux is
 * booted using the insmod command. When the module is installed, the
 * dwc_usb3_driver_init function is called. When the module is removed (using
 * rmmod), the dwc_usb3_driver_exit function is called.
 *
 * This module also defines a data structure for the dwc_usb3_device. This
 * structure allows the DWC_usb3 driver to comply with the standard Linux driver
 * model in which devices and drivers are registered with a bus driver. This has
 * the benefit that Linux can expose attributes of the driver and device in its
 * special sysfs file system. Users can then read or write files in this file
 * system to perform diagnostics on the driver components or the device.
 *
 * @subsection data_struct_subsec Data Structures
 *
 * These are the main data structures used by the DWC_usb3 driver module.
 *
 * - struct dwc_usb3_device
 *
 * @subsection init_and_clean_subsec Initialization and Cleanup Functions
 *
 * These are the initialization and cleanup functions of the DWC_usb3 driver
 * module.
 *
 * - dwc_usb3_driver_init()
 * - dwc_usb3_driver_probe()
 * - dwc_usb3_driver_remove()
 * - dwc_usb3_driver_exit()
 *
 */

/**
 * @page cil_chapter Core Interface Layer
 *
 * The Core Interface Layer provides basic services for accessing and managing
 * the DWC_usb3 hardware. The CIL also manages the memory map for the core so
 * that the PCD doesn't have to do this separately.
 *
 * @section csr_sec Control and Status Register Structures
 *
 * The following structures define the size and relative field offsets for
 * each register in the DWC_usb3 controller. These structures are not created
 * in memory through normal memory allocation methods. After mapping memory
 * for the controller into the OS memory space, these structures are overlaid
 * on the mapped memory by setting the appropriate base address for each
 * structure. Each register can then be accessed via its address in one of
 * these structures.
 *
 * The precise method for accessing registers is OS-specific. It may be as
 * simple as directly reading or writing a register field in one of these
 * structures. Or it may require passing the mapped register address to a
 * read/write method defined by the OS.
 *
 * @subsection core_glbl_regs_subsec Core Global Registers Structure
 *
 * The following structure defines the size and relative field offsets for
 * the Core Global registers.
 *
 * - struct dwc_usb3_core_global_regs
 *
 * @subsection dev_glbl_regs_subsec Peripheral Mode Global Registers Structure
 *
 * The following structure defines the size and relative field offsets for
 * the Peripheral Mode Global Registers.
 *
 * - struct dwc_usb3_dev_global_regs
 *
 * @subsection dev_ep_regs_subsec Peripheral Mode Per-Endpoint Registers Structures
 *
 * The following structure defines the size and relative field offsets for
 * the Peripheral Mode Per-Endpoint Registers. There is one set of these
 * registers for each endpoint that is instantiated in the core.
 *
 * - struct dwc_usb3_dev_ep_regs
 *
 * @section cil_func_sec Functions
 *
 * These are the main functions provided by the CIL.
 *
 * - dwc_usb3_pcd_common_init()
 * - dwc_usb3_pcd_common_remove()
 * - dwc_usb3_pcd_device_init()
 * - dwc_usb3_enable_common_interrupts()
 * - dwc_usb3_enable_device_interrupts()
 * - dwc_usb3_mode()
 * - dwc_usb3_is_device_mode()
 * - dwc_usb3_is_host_mode()
 * - dwc_usb3_dump_global_registers()
 * - dwc_usb3_fill_desc()
 * - dwc_usb3_start_desc_chain()
 * - dwc_usb3_end_desc_chain()
 * - dwc_usb3_enable_desc()
 * - dwc_usb3_xmit_fn_remote_wake()
 * - dwc_usb3_dep_cfg()
 * - dwc_usb3_dep_xfercfg()
 * - dwc_usb3_dep_sstall()
 * - dwc_usb3_dep_cstall()
 * - dwc_usb3_dep_startxfer()
 * - dwc_usb3_dep_updatexfer()
 * - dwc_usb3_dep_endxfer()
 * - dwc_usb3_dep_startnewcfg()
 * - dwc_usb3_enable_ep()
 * - dwc_usb3_disable_ep()
 * - dwc_usb3_get_device_speed()
 * - dwc_usb3_get_frame()
 * - dwc_usb3_pcd_get_link_state()
 * - dwc_usb3_pcd_set_link_state()
 * - dwc_usb3_set_address()
 * - dwc_usb3_ena_usb2_suspend()
 * - dwc_usb3_dis_usb2_suspend()
 * - dwc_usb3_accept_u1()
 * - dwc_usb3_accept_u2()
 * - dwc_usb3_enable_u1()
 * - dwc_usb3_enable_u2()
 * - dwc_usb3_disable_u1()
 * - dwc_usb3_disable_u2()
 * - dwc_usb3_u1_enabled()
 * - dwc_usb3_u2_enabled()
 * - dwc_usb3_pcd_remote_wake()
 */

/**
 * @page pcd_chapter Peripheral Controller Driver
 *
 * The Peripheral Controller Driver (PCD) is responsible for translating
 * requests from the Function Driver into the appropriate actions on the
 * DWC_usb3 controller. It isolates the Function Driver from the specifics of
 * the controller by providing an API to the Function Driver. This API may
 * vary between operating systems, but it remains constant within a given OS.
 * Section 4.2 @ref fun_drvr_ifc_sec describes this API for supported
 * operating systems.
 *
 * A USB device responds to commands issued from the host. Section 4.3
 * @ref std_usb_cmd_proc_sec describes handling of the standard USB
 * commands within the DWC_usb3 software environment.
 *
 * An important function of the PCD is managing interrupts generated by the
 * DWC_usb3 controller. The behaviors for each DWC_usb3 peripheral mode
 * interrupt are described in section 4.4 @ref dev_intr_svc_rtn_sec.
 *
 * @section pcd_data_st_sec Data Structures
 *
 * These are the main data structures used by the PCD.
 *
 * @subsection pcd_subsec PCD Data Structure
 *
 * The following structure encapsulates the data for the dwc_usb3 PCD.
 *
 * - struct dwc_usb3_pcd
 *
 * @subsection pcd_ep_subsec Endpoint Data Structure
 *
 * The following structure describes an endpoint. There is an array of these
 * in the PCD structure, one for each endpoint.
 *
 * - struct dwc_usb3_pcd_ep
 *
 * @subsection ep_subsec Endpoint State Structure
 *
 * The following structure represents the state of a single endpoint when
 * acting in peripheral mode. It contains the data items needed for an endpoint
 * to be activated and transfer packets. One of these structures is contained
 * in each dwc_usb3_pcd_ep structure.
 *
 * - struct dwc_ep
 *
 * @subsection pcd_req_subsec Transfer Request Data Structure
 *
 * The following structure describes a transfer request. It is used to form
 * a list of requests.
 *
 * - struct dwc_usb3_pcd_req
 *
 * @subsection req_subsec Transfer Request State Structure
 *
 * The following structure represents the state of a single transfer request
 * when acting in peripheral mode. It contains the data items needed for a
 * request to be started and completed.
 *
 * - struct dwc_req
 *
 * @subsection dev_dma_subsec DMA Descriptor Structure
 *
 * The following structure represents the DMA descriptors used by the dwc_usb3
 * controller in peripheral mode. These descriptors are also referred to as
 * Transfer Request Blocks or TRBs (as in the xHCI spec).
 *
 * - struct dwc_usb3_dma_desc
 *
 * @section fun_drvr_ifc_sec Function Driver Interface
 *
 * This section describes the API that the PCD presents to the Function Driver
 * for each supported operating system. Currently, Linux is the only supported
 * OS.
 *
 * @subsection gadget_api_subsec Linux Gadget API
 *
 * The Peripheral Controller Driver for Linux will implement the Gadget API,
 * so that the existing Gadget drivers can be used. (Gadget Driver is the
 * Linux terminology for a Function Driver.)
 *
 * The Linux Gadget API is defined in the header file <linux/usb_gadget.h>.
 * The following data structures define the functions implemented in the PCD
 * to provide the interface. The USB EP operations API is defined in the
 * structure usb_ep_ops and the USB Controller API is defined in the structure
 * struct usb_gadget_ops.
 *
 * @subsection usb_ep_ops_subsec USB Endpoint Operations
 *
 * The following sections briefly describe the behavior of the Gadget API
 * endpoint operations implemented in the DWC_usb3 driver software. Detailed
 * descriptions of the generic behavior of each of these functions can be
 * found in the Linux header file include/linux/usb_gadget.h.
 *
 * The Gadget API provides wrapper functions for each of the function pointers
 * defined in usb_ep_ops. The Gadget Driver calls the wrapper function, which
 * then calls the underlying PCD function. The following sections are named
 * according to the wrapper functions. Within each section, the corresponding
 * DWC_usb3 PCD function name is specified.
 *
 * Functions in the API that are not described below are not implemented.
 *
 * <strong><em>usb_ep_enable</em></strong>
 *
 * - ep_enable()
 *
 * <strong><em>usb_ep_disable</em></strong>
 *
 * - ep_disable()
 *
 * <strong><em>usb_ep_alloc_request</em></strong>
 *
 * - alloc_request()
 *
 * <strong><em>usb_ep_free_request</em></strong>
 *
 * - free_request()
 *
 * <strong><em>usb_ep_queue</em></strong>
 *
 * - ep_queue()
 *
 * <strong><em>usb_ep_dequeue</em></strong>
 *
 * - ep_dequeue()
 *
 * <strong><em>usb_ep_set_halt, usb_ep_clear_halt</em></strong>
 *
 * - ep_set_halt()
 *
 * @subsection gadget_ops_subsec Gadget Operations
 *
 * The following gadget operations will be implemented in the DWC_usb3 PCD.
 * Functions in the API that are not described below are not implemented.
 *
 * The Gadget API provides wrapper functions for each of the function pointers
 * defined in usb_gadget_ops. The Gadget Driver calls the wrapper function,
 * which then calls the underlying PCD function. The following sections are
 * named according to the wrapper functions. Within each section, the
 * corresponding DWC_usb3 PCD function name is specified.
 *
 * <strong><em>usb_gadget_get_frame</em></strong>
 *
 * - dwc_get_frame()
 *
 * <strong><em>usb_gadget_wakeup</em></strong>
 *
 * - dwc_usb3_wakeup()
 *
 * @subsection streams_ext_subsec USB 3.0 Bulk Streams Extension
 *
 * USB 3.0 introduces the concept of <em>Bulk Streams</em>. Bulk Streams provide
 * the ability to move multiple streams of data over a single Bulk endpoint.
 * This is used by a new version of the Mass Storage Class protocol, called USB
 * Attached SCSI Protocol (UASP). It allows multiple transfer requests to be
 * queued up by the Host, which can then be completed in whatever order the data
 * becomes available from the Peripheral.
 *
 * This requires some modifications to the Gadget API and the underlying USB
 * request functions, to allow the stream capabilities of an endpoint to be set,
 * and to indicate the stream ID associated with a transfer request. It also
 * requires a new File Storage gadget which has the needed Bulk Streams support.
 *
 * @section std_usb_cmd_proc_sec Standard USB Command Processing
 *
 * In Linux, the USB Command processing is done in two places - the first
 * being the PCD and the second being the Gadget Driver (for example, the
 * Mass Storage Gadget Driver). See pcd_setup() for a detailed explanation.
 *
 * @section dev_intr_svc_rtn_sec Peripheral Interrupt Service Routine
 *
 * The PCD handles the peripheral interrupts. Many conditions can cause a
 * peripheral interrupt. When an interrupt occurs, the peripheral interrupt
 * service routine determines the cause of the interrupt and dispatches
 * handling to the appropriate function. These interrupt handling functions
 * are described below.
 *
 * <strong><em>Global Peripheral Interrupts</em></strong>
 *
 * - dwc_usb3_handle_dev_intr()
 * - handle_disconnect_intr()
 * - handle_usb_reset_intr()
 * - dwc_usb3_handle_connect_done_intr()
 * - handle_link_status_change_intr()
 * - handle_wakeup_detected_intr()
 * - handle_sof_intr()
 * - handle_hiber_req_intr()
 *
 * <strong><em>Endpoint-Specific Peripheral Interrupts</em></strong>
 *
 * - dwc_usb3_handle_ep_intr()
 *
 * @section pcd_func_sec Other Functions
 *
 * These are the remaining functions provided by the PCD.
 *
 * - dwc_usb3_pcd_init()
 * - dwc_usb3_pcd_remove()
 * - dwc_usb3_pcd_ep_enable()
 * - dwc_usb3_pcd_ep_disable()
 * - dwc_usb3_pcd_ep_submit_req()
 * - dwc_usb3_pcd_ep_cancel_req()
 * - dwc_usb3_pcd_ep_set_halt()
 * - dwc_usb3_pcd_get_frame_number()
 * - dwc_usb3_ep0_activate()
 * - dwc_usb3_pcd_ep0_out_start()
 * - dwc_usb3_stop_all_xfers()
 * - dwc_usb3_get_out_ep()
 * - dwc_usb3_get_in_ep()
 * - dwc_usb3_handle_ep0()
 * - dwc_usb3_ep_complete_request()
 * - usb_gadget_register_driver() / usb_gadget_probe_driver()
 * - usb_gadget_unregister_driver()
 */

/**
 * @page hiber_chapter Hibernation
 *
 * DWC_usb3 Subsystem Version 2.00a and above implements a new functionality,
 * hibernation. Hibernation allows power to be removed from most parts of the
 * controller during periods of inactivity. This is done through a combination
 * of hardware and software.
 *
 * The controller signals a device event when the host has requested a low-power
 * state suitable for the device to enter hibernation. When this event is
 * received, the PCD will stop the controller, command it to save critical state
 * information to main memory, save the content of the CSRs, and then shut off
 * Vcc power to the core.
 *
 * When a condition is detected that requires the core to resume operation, the
 * driver will receive an interrupt from the monitoring hardware, causing it to
 * switch Vcc back on, restore the content of the CSRs, and command the
 * controller to restore its critical state from main memory.
 *
 * The bulk of the hibernation code is contained in the files pcd_hiber.c and
 * linux_hiber.c. Note that this is not a complete implementation; the HAPS
 * platform on which it has been tested does not provide the normal PCIe PME
 * mechanism, instead there is special logic in the custom PCIe "gasket" to
 * handle power control and provide an interrupt to awaken the core from
 * hibernation. Therefore, the code in linux_hiber.c which handles power
 * control and wakeup will need to be rewritten for a "real" implementation.
 * Also, since the CPU is still running while the controller is in the
 * hibernated state, code has been added to linux_gadget.c to prevent any calls
 * that would try to touch the controller hardware from entering the driver
 * while hibernation is active. A "real" implementation should not require this.
 *
 * @section hiber_func_sec Hibernation Functions
 *
 * These are the functions provided by pcd_hiber.c and linux_hiber.c
 *
 * - dwc_enter_hibernation()
 * - dwc_exit_hibernation_after_connect()
 * - dwc_exit_hibernation()
 * - dwc_wait_pme_thread()
 * - dwc_usb3_handle_pme_intr()
 * - dwc_usb3_power_ctl()
 */
#endif /* _DWC_DOX_H_ */
