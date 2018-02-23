/* Name: gpio-usb
 * HomePage: http://homes-smart.ru/index.php/oborudovanie/uroki-i-primery/41-usb-kontroller-umnogo-doma
 * Last canges 12.12.14
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>        /* this is libusb */
#include "opendevice.h" /* common code moved to separate module */
#include "requests.h"   /* custom request numbers */
#include "usbconfig.h"


usb_dev_handle      *handle = NULL;
char                buffer[8];
int                 cnt;

// 1,2,3,21,24,25

int pinmode(int pin,int mode) // управление режимом без записи в EEROM.
{
cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 2,mode*256+pin, 0, buffer, sizeof(buffer), 5000);
        if(cnt < 0){
            fprintf(stderr, "USB error: %s\n", usb_strerror());
	    return -2;
        } else return 1;
}
int pinmodeEEROM(int pin,int mode) // управление режимом с записью в EEROM.
{
cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 3,mode*256+pin, 0, buffer, sizeof(buffer), 5000);
        if(cnt < 0){
            fprintf(stderr, "USB error: %s\n", usb_strerror());
	    return -2;
        } else return 1;
}

int digitalread(int pin) // чтение статуса порта.
{
              cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 24, pin, 0, buffer, sizeof(buffer), 5000);
        if(cnt < 1){
            if(cnt < 0){
                fprintf(stderr, "USB error: %s\n", usb_strerror());
		return -1;
            }else{
                fprintf(stderr, "only %d bytes received.\n", cnt);
		return -1;
            }
        }else return (buffer[0]);

}

int digitalwrite(int pin,int data) // запись порта.
{
      if (pin>0 && pin<9) {

        cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, 1, data*256+pin, 0, buffer, sizeof(buffer), 5000); // CUSTOM_RQ_SET_STATUS
        if(cnt < 0){
            fprintf(stderr, "USB error: %s\n", usb_strerror());
	    return -2;
        } else return 1;
      } else return -1;
}

static void usage(char *name)
{
    fprintf(stderr, "usbgpio commandline client\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s on <num gpio>.....Включить порт\n", name);
    fprintf(stderr, "  %s off <num gpio>....Выключить порт\n", name);
    fprintf(stderr, "  %s mode .............Управление режимом порта\n", name);
    fprintf(stderr, "  %s status .......... Просмотр статуса GPIO на вывод\n", name);
    fprintf(stderr, "  %s statusin ... .....Просмотр статуса GPIO на ввод\n", name);
    fprintf(stderr, "  %s pwm3 <level>......Управление ШИМ на 3 GPIO\n", name);
    fprintf(stderr, "  %s pwm4 <level>......Управление ШИМ на 4 GPIO\n", name);
    fprintf(stderr, "  %s rcsend <key>......Отправить ключ RCremote\n", name);
    fprintf(stderr, "  %s dhtread...........Чтение датчика DHT11 или DHT22\n", name);
    fprintf(stderr, "  %s dhtsetup..........Запрос статуса и вкл/выкл датчиков DHT\n", name);

#if ENABLE_TEST
    fprintf(stderr, "  %s test ..... run driver reliability test\n", name);
#endif /* ENABLE_TEST */
}

int main(int argc, char **argv)
{
//usb_dev_handle      *handle = NULL;
const unsigned char rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
char                vendor[] = {USB_CFG_VENDOR_NAME, 0}, product[] = {USB_CFG_DEVICE_NAME, 0};
//char                buffer[8];

int                  vid, pid, isOn;

    usb_init();
    if(argc < 2){   /* we need at least one argument */
        usage(argv[0]);
        exit(1);
    }
    /* compute VID/PID from usbconfig.h so that there is a central source of information */
    vid = rawVid[1] * 256 + rawVid[0];
    pid = rawPid[1] * 256 + rawPid[0];
    /* The following function is in opendevice.c: */
    if(usbOpenDevice(&handle, vid, vendor, pid, product, NULL, NULL, NULL) != 0){
        fprintf(stderr, "Could not find USB device \"%s\" with vid=0x%x pid=0x%x\n", product, vid, pid);
        exit(1);
    }
    /* Since we use only control endpoint 0, we don't need to choose a
     * configuration and interface. Reading device descriptor and setting a
     * configuration and interface is done through endpoint 0 after all.
     * However, newer versions of Linux require that we claim an interface
     * even for endpoint 0. Enable the following code if your operating system
     * needs it: */
#if 0
    int retries = 1, usbConfiguration = 1, usbInterface = 0;
    if(usb_set_configuration(handle, usbConfiguration) && showWarnings){
        fprintf(stderr, "Warning: could not set configuration: %s\n", usb_strerror());
    }
    /* now try to claim the interface and detach the kernel HID driver on
     * Linux and other operating systems which support the call. */
    while((len = usb_claim_interface(handle, usbInterface)) != 0 && retries-- > 0){
#ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
        if(usb_detach_kernel_driver_np(handle, 0) < 0 && showWarnings){
            fprintf(stderr, "Warning: could not detach kernel driver: %s\n", usb_strerror());
        }
#endif
    }
#endif
    if(strcasecmp(argv[1], "dhtread") == 0){
        cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 22, 0, 0, buffer, sizeof(buffer), 5000);
        if(cnt < 1){
            if(cnt < 0){
                fprintf(stderr, "USB error: %s\n", usb_strerror());
            }else{
                fprintf(stderr, "only %d bytes received.\n", cnt);
            }
        }else{
//	  printf("%d байт: %d %d %d %d %d\n",cnt,(unsigned char)(buffer[0]),(unsigned char)(buffer[1]),(unsigned char)(buffer[2]),(unsigned char)(buffer[3]),(unsigned char)(buffer[4]));

//	  printf("%d байт: %u %u %u %u %u\n",cnt,(unsigned char)(buffer[0]),(unsigned char)(buffer[1]),(unsigned char)(buffer[2]),(unsigned char)(buffer[3]),(unsigned char)(buffer[4]));


	  if (buffer[0]==0 && buffer[1]==0 && buffer[2]==0 && buffer[3]==0) {
	    printf("DHT датчик не найден или не включен командой dhtsetup.\n");
	  } else if ((((unsigned char)buffer[0] + (unsigned char)buffer[1] +(unsigned char) buffer[2] + (unsigned char)buffer[3] )& 0xFF) == (unsigned char)buffer[4]) { 

	  if (buffer[1]==0 && buffer[3]==0) printf("DHT11:%d %% %d C\n",(buffer[0]),(buffer[2])); // dht11
	 else  { // dht22
	     float f, h;
	         h = (unsigned char)buffer[0] * 256 + (unsigned char)buffer[1];
       h /= 10;

       f = ((unsigned char)buffer[2] & 0x7F)* 256 + (unsigned char)buffer[3];
       f /= 10.0;
       if ((unsigned char)buffer[2] & 0x80)  f *= -1;
       printf("DHT22: %.1f °C  %.1f %%\n", f, h);

	  }

	  } else printf("DHT Ошибка\n");
	}
    }else if(strcasecmp(argv[1], "status") == 0){
        cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, CUSTOM_RQ_GET_STATUS, 0, 0, buffer, sizeof(buffer), 5000);
        if(cnt < 1){
            if(cnt < 0){
                fprintf(stderr, "USB error: %s\n", usb_strerror());
            }else{
                fprintf(stderr, "only %d bytes received.\n", cnt);
            }
        }else{
           // printf("LED is %s\n", buffer[0] ? "on" : "off");
	  int f;
	  for(f = 0; f < 8; f++){
	  printf("LED %d is %s\n",f+1, buffer[f] ? "on" : "off");

	  }
        }

     }else if(strcasecmp(argv[1], "statusin") == 0){ // вывод статусов портов при режиме инпут

       if(argc > 2) {
	    int pin= atoi(argv[2]);

	    printf("MODE PIN %d is %d\n",pin, digitalread(pin));

       } else {
	 printf("all pins:\n");

               cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 25, 0, 0, buffer, sizeof(buffer), 5000);
        if(cnt < 1){
            if(cnt < 0){
                fprintf(stderr, "USB error: %s\n", usb_strerror());
            }else{
                fprintf(stderr, "only %d bytes received.\n", cnt);
            }
        }else{

	  int f;
	  for(f = 0; f < 8; f++){
	   printf("LED %d is %d\n",f+1, (int)(buffer[f]));
	  }
        }

      }
     }else if(strcasecmp(argv[1], "mode") == 0){ // режимы портов

       if(argc < 4) { // вывод режимов портов
               cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 21, 0, 0, buffer, sizeof(buffer), 5000);
        if(cnt < 1){
            if(cnt < 0){
                fprintf(stderr, "USB error: %s\n", usb_strerror());
            }else{
                fprintf(stderr, "only %d bytes received.\n", cnt);
            }
        }else{

	  int f;
	  for(f = 0; f < cnt; f++){
	    if ((unsigned char)(buffer[f])!=1) printf("LED %d mode output (%d)\n",f+1,(unsigned char)(buffer[f]));
	   else printf("LED %d mode input (%d)\n",f+1,(unsigned char)(buffer[f]));
	  }
	fprintf(stderr, "Установка порта на ввод или на вывод:  %s <Номер GPIO> <0 или 1>  \n", argv[0]);
        }
       } else { // установка режимов 1 порта
	  pinmodeEEROM (atoi(argv[2]),atoi(argv[3]));

       }
     }else if(strcasecmp(argv[1], "rcsend") == 0){
       if(argc > 2) {
	 unsigned long key= atoi(argv[2]);
	 if (key<531441) {
	 int per=3;
	if(argc > 3) per=atoi(argv[3]); // период вручную.


      unsigned int keyh = (unsigned long)key >> 16;
     // printf(" %d %d)\n",keyh,key);
      cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 4,key, keyh+256*per, buffer, sizeof(buffer), 5000);

       } else printf("Код не может быть больше 531440\n");
       } else printf("Не указан обязательный параметр:число от 0 до 531440\n");

     //-----------------------pwm start
     }else if(strcasecmp(argv[1], "pwm3") == 0){
       if(argc > 2) {
        int pin= atoi(argv[2]);
	// int val= atoi(argv[3]);

       cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 23,pin, 0, buffer, sizeof(buffer), 5000);
        if(cnt < 0){
            fprintf(stderr, "USB error: %s\n", usb_strerror());
        }
       } else printf("Не указан обязательный параметр:число от 0 до 255\n");

            }else if(strcasecmp(argv[1], "pwm4") == 0){
       if(argc > 2) {
        int pin= atoi(argv[2]);
	// int val= atoi(argv[3]);

       cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 24,pin, 0, buffer, sizeof(buffer), 5000);
        if(cnt < 0){
            fprintf(stderr, "USB error: %s\n", usb_strerror());
        } 
       } else printf("Не указан обязательный параметр:число от 0 до 255\n");
       //-----------------------pwm end

       }else if(strcasecmp(argv[1], "dhtsetup") == 0){ // вкл/выкл чтение датчиков dht
       if(argc > 2) {
	 int mode= atoi(argv[2]);
	  cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 6, mode, 0, buffer, sizeof(buffer), 5000);
	  if (mode==1) printf("Установка:Датчик DHT включен,обновление произойдет в течении минуты.\n");
	  else printf("Установка:Датчик DHT выключен\n");

       } else {

	 cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 5, 0, 0, buffer, sizeof(buffer), 5000);
	  if (buffer[0]==1) printf("Статус:Датчик DHT включен (%u)\n",(unsigned char)buffer[0]);
	  else printf("Статус:Датчик DHT выключен (%u)\n",(unsigned char)buffer[0]);
	  printf("Установка:%s dhtsetup 1 или 0 чтобы включить или выключить опрос датчиков\n",argv[0]);
       }

     }else if(strcasecmp(argv[1], "analog") == 0){ // аналог порт

     cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 17, 0, 0, buffer, sizeof(buffer), 5000);

           if(cnt < 1){
            if(cnt < 0){
                fprintf(stderr, "USB error: %s\n", usb_strerror());
            }else{
                fprintf(stderr, "only %d bytes received.\n", cnt);
            }
        }else{
     printf("analog:%u (%i %i)\n",((unsigned char)buffer[0]+(unsigned char)buffer[1]*256),(unsigned char)buffer[0],(unsigned char)buffer[1]);
        }

    }else if((isOn = (strcasecmp(argv[1], "on") == 0)) || strcasecmp(argv[1], "off") == 0){

      if(argc > 2) {
   if (atoi(argv[2])>0 && atoi(argv[2])<9 )digitalwrite (atoi(argv[2]),isOn);
else printf("Возможный диапазон портов от 1 до 8\n");
      } else printf("Установка:%s on <номер GPIO> или off для выключения\n",argv[0]);


#if ENABLE_TEST
    }else if(strcasecmp(argv[1], "test") == 0){
        int i;
        srandomdev();
        for(i = 0; i < 50000; i++){
            int value = random() & 0xffff, index = random() & 0xffff;
            int rxValue, rxIndex;
            if((i+1) % 100 == 0){
                fprintf(stderr, "\r%05d", i+1);
                fflush(stderr);
            }
            cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, CUSTOM_RQ_ECHO, value, index, buffer, sizeof(buffer), 5000);
            if(cnt < 0){
                fprintf(stderr, "\nUSB error in iteration %d: %s\n", i, usb_strerror());
                break;
            }else if(cnt != 4){
                fprintf(stderr, "\nerror in iteration %d: %d bytes received instead of 4\n", i, cnt);
                break;
            }
            rxValue = ((int)buffer[0] & 0xff) | (((int)buffer[1] & 0xff) << 8);
            rxIndex = ((int)buffer[2] & 0xff) | (((int)buffer[3] & 0xff) << 8);
            if(rxValue != value || rxIndex != index){
                fprintf(stderr, "\ndata error in iteration %d:\n", i);
                fprintf(stderr, "rxValue = 0x%04x value = 0x%04x\n", rxValue, value);
                fprintf(stderr, "rxIndex = 0x%04x index = 0x%04x\n", rxIndex, index);
            }
        }
        fprintf(stderr, "\nTest completed.\n");
#endif /* ENABLE_TEST */
    }else{
        usage(argv[0]);
        exit(1);
    }
    usb_close(handle);
    return 0;
}
