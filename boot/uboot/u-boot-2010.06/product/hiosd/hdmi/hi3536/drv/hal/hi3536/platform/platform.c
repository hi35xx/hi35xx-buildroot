#include "platform_api.h"
#include "sii_time.h"
#include "si_lib_log_api.h"
#include "si_drv_tx_regs.h"
#if 0
#define I2C_BIT_RATE 100
#define SPI_BIT_RATE 1000

//extern Aardvark handle;

uint16_t ports[16] = {0xFF};
uint16_t selectedPort;
//BB_HANDLE handle_under_use = 0; 

static const unsigned char cPinConversionHeader2BB[10] = {
    0, // Not connected
    0, // Not connected
    0, // Pin3
    7, // Pin4
    1, // Pin5
    6, // Pin6
    2, // Pin7
    5, // Pin8
    3, // Pin9
    4 // Pin10
};

int16_t getAvailableDevice(void)
{

    uint32_t unique_ids[16];
    int nelem = 16;

    // Find all the attached devices
    int count = aa_find_devices_ext(nelem, (u16 *)ports, nelem, (u32 *)unique_ids);
    u16 i;

    // Print the information on each device
    if (count > nelem)  count = nelem;
    for (i = 0; i < count; ++i) {
        // Determine if the device is in-use
        const char *status = "(avail) ";
        if (ports[i] & AA_PORT_NOT_FREE) {
            ports[i] &= ~AA_PORT_NOT_FREE;
            status = "(in-use)";
        }
        else
            return ports[i];
    }
    printf("No Free Aardvark Device\n");
    return -1;


}

void BB_Version( void )
{

    dword dwVersion;

    BlackBox_GetVersion( &dwVersion );
    printf( "BlackBox version %lx.%02lx.%02lx.%02lx\n", 
        dwVersion/0x1000000,
        dwVersion/0x10000 & 0xff,
        dwVersion/0x100 & 0xff,
        dwVersion & 0xff);

}

//void BB_Close( void )
//{
//    BlackBox_Close( handle_under_use );
//}

int BB_SetI2C100kHz( BB_HANDLE handle )
{

    I2C_OPTIONS i2cOptions;
        
    if( I2C_GetOptions( handle, &i2cOptions ) != BB_SUCCESS){
        printf( "I2C_GetOptions error\n" );
        return SII_PLATFORM_STATUS__FAILED;
    }
    else{
        i2cOptions.i2cBusMode = I2C_MODE_STD; //I2C_MODE_STD or I2C_MODE_FAST
		i2cOptions.i2cTimeout = 100;
		i2cOptions.readTimeout= 100;
		i2cOptions.writeTimeout=100;
        if( I2C_SetOptions( handle, i2cOptions ) != BB_SUCCESS){
            printf( "I2C_SetOptions error\n" );
            return SII_PLATFORM_STATUS__FAILED;
        }
    }
  
    return SII_PLATFORM_STATUS__SUCCESS;
}

bool_t SiiPlatformInit(SiiPlatformInterface_t *pInterfaceInfo)
{

	Sii_Init_ProcessTime();
   
    switch(pInterfaceInfo->interfaceType)
    {
   
    case SII_PLATFORM_TYPE__I2C:
    case SII_PLATFORM_TYPE__PRIF_OVER_I2C:
        switch(pInterfaceInfo->interfaceHardware)
        {
        case SII_PLATFORM_HARDWARE__BB:
			{
                BB_HANDLE bbHandle;
				int8_t num;
				BlackBox_Number( &num );
				if( num == 0 ){
					printf( "No BlackBox connected\n" );
					return SII_PLATFORM_STATUS__FAILED;
				}
				//atexit( BB_Close );  -> not possible to do as we are allowing multiple devices to be opened at time
				while(num)
				{
					if( BlackBox_Open( num - 1, &bbHandle ) == BB_SUCCESS ){
						break;
					}
					num--;
				}
				if(num <= 0)
				{
					printf("None of the Balckbox could be opened\n");
					return SII_PLATFORM_STATUS__FAILED;
				}
                pInterfaceInfo->handle = bbHandle;
				BB_Version();
				SetI2C16bitAddress( (BB_HANDLE)(pInterfaceInfo->handle), (pInterfaceInfo->interfaceType == SII_PLATFORM_TYPE__PRIF_OVER_I2C)?1:0 );
				return BB_SetI2C100kHz((BB_HANDLE)(pInterfaceInfo->handle));
			}
            break;
        case SII_PLATFORM_HARDWARE__AARDVARK:
            {
                AardvarkExt aaext;
                int bitrate;
                int16_t port  = getAvailableDevice();
                if (port >= 0)
                {
	                pInterfaceInfo->handle  = aa_open_ext(port, &aaext);

                    if (pInterfaceInfo->handle <= 0) {
		                printf("Unable to open Aardvark device on port %d\n",getAvailableDevice());
                        printf("Error code = %d\n", pInterfaceInfo->handle);
                        return SII_PLATFORM_STATUS__FAILED;
                    }
                    //printf("Opened Aardvark; features = 0x%02x\n", aaext.features);

                        // Ensure that the I2C subsystem is enabled
	                aa_configure(pInterfaceInfo->handle, AA_CONFIG_GPIO_I2C);

                    // Enable the I2C bus pullup resistors (2.2k resistors).
                    // This command is only effective on v2.0 hardware or greater.
                    // The pullup resistors on the v1.02 hardware are enabled by default.
                    aa_i2c_pullup(pInterfaceInfo->handle, AA_I2C_PULLUP_BOTH);

                    // Enable the Aardvark adapter's power pins.
                    // This command is only effective on v2.0 hardware or greater.
                    // The power pins on the v1.02 hardware are not enabled by default.
                    aa_target_power(pInterfaceInfo->handle, AA_TARGET_POWER_BOTH);
                    bitrate = aa_i2c_bitrate(pInterfaceInfo->handle, I2C_BIT_RATE);
                }
                else
                {
                    return SII_PLATFORM_STATUS__FAILED;
                }
            }
            return SII_PLATFORM_STATUS__SUCCESS;
        case SII_PLATFORM_HARDWARE__CHEETAH:
            return SII_PLATFORM_STATUS__FAILED;
        default:
            return SII_PLATFORM_STATUS__FAILED;
        }
        break;
    case SII_PLATFORM_TYPE__SPI:
        switch(pInterfaceInfo->interfaceHardware)
        {
        case SII_PLATFORM_HARDWARE__AARDVARK:
            {
                AardvarkExt aaext;
                int bitrate;
	            pInterfaceInfo->handle  = aa_open_ext(getAvailableDevice(), &aaext);

                if (pInterfaceInfo->handle <= 0) {
		            printk("Unable to open Aardvark device on port %d\n",getAvailableDevice());
                    printk("Error code = %d\n", pInterfaceInfo->handle);
                    return SII_PLATFORM_STATUS__FAILED;
                }

                printf("Opened Aardvark; features = 0x%02x\n", aaext.features);

                // Ensure that the SPI subsystem is enabled
	            aa_configure(pInterfaceInfo->handle, AA_CONFIG_SPI_GPIO);

                // Enable the Aardvark adapter's power pins.
                // This command is only effective on v2.0 hardware or greater.
                // The power pins on the v1.02 hardware are not enabled by default.
                aa_target_power(pInterfaceInfo->handle, AA_TARGET_POWER_BOTH);
                    // Setup the clock phase
                aa_spi_configure(pInterfaceInfo->handle, AA_SPI_POL_RISING_FALLING, AA_SPI_PHASE_SAMPLE_SETUP, AA_SPI_BITORDER_MSB);

                // Setup the bitrate
                bitrate = aa_spi_bitrate(pInterfaceInfo->handle, SPI_BIT_RATE);
            }
            return SII_PLATFORM_STATUS__SUCCESS;
        case SII_PLATFORM_HARDWARE__CHEETAH:
            return SII_PLATFORM_STATUS__FAILED;
        default:
            return SII_PLATFORM_STATUS__FAILED;
        }
        break;     
    default:
        return SII_PLATFORM_STATUS__FAILED;
        
    }
   
}
#endif 

#if defined(CONFIG_HDMI_STB_SDK)
#define HDMI_CTRL_BASE_ADDR 0xf8ce0000
#elif defined(CONFIG_HDMI_BVT_SDK)
#define HDMI_CTRL_BASE_ADDR 0x13010000
#endif
SiiPlatformStatus_t SiiPlatformRead(SiiPlatformInterface_t *pInterfaceInfo, uint8_t devId, uint16_t address, uint8_t *data_in, uint16_t length)
{
    uint16_t count = 0;
#if 1
    volatile uint32_t *addr = (uint32_t *)IO_ADDRESS(((address - BASE_ADDRESS)*4 | HDMI_CTRL_BASE_ADDR));
    for (; count < length; count++)  
    {
        *(data_in + count) = (uint8_t)(*(addr + count));
    }
#endif    
#if 0    
	switch(pInterfaceInfo->interfaceHardware)
	{

	case SII_PLATFORM_HARDWARE__BB:
		switch(pInterfaceInfo->interfaceType)
		{
		case SII_PLATFORM_TYPE__I2C:
		case SII_PLATFORM_TYPE__PRIF_OVER_I2C:
            SetI2C16bitAddress( (BB_HANDLE)(pInterfaceInfo->handle), (pInterfaceInfo->interfaceType == SII_PLATFORM_TYPE__PRIF_OVER_I2C)?1:0 );
			if(0 != I2C_ReadBlock((BB_HANDLE)pInterfaceInfo->handle, devId, (word)address, (byte *)data_in, (word)length))
            {
                printf("failed to read 0x%x - 0x%x\n", devId, address);
				return SII_PLATFORM_STATUS__FAILED;
            }
			else
				return SII_PLATFORM_STATUS__SUCCESS;
		default:
			return SII_PLATFORM_STATUS__FAILED;
		}
		break;
	case SII_PLATFORM_HARDWARE__AARDVARK:
		switch(pInterfaceInfo->interfaceType)
		{
		case SII_PLATFORM_TYPE__I2C:
			{
				u16 num_written;
				u16 num_read;
				if(AA_I2C_STATUS_OK != aa_i2c_write_read(pInterfaceInfo->handle, devId >> 1, AA_I2C_NO_FLAGS, 1, (u08*)&address, 
					&num_written, (u16)length, (u08*)data_in, &num_read))
                {
                    printf("failed to read 0x%x - 0x%x\n", devId, address);
					return SII_PLATFORM_STATUS__FAILED;
                }
			}
            return SII_PLATFORM_STATUS__SUCCESS;
		case SII_PLATFORM_TYPE__PRIF_OVER_I2C:
			{
				u16 num_written;
				u16 num_read;
                uint8_t prifAddr[2] = {0x00, 0x00};
                prifAddr[0] = (uint8_t)((address & 0xFF00) >> 8);
                prifAddr[1] = (uint8_t)(address & 0x00FF);
                if(AA_I2C_STATUS_OK != aa_i2c_write_read(pInterfaceInfo->handle, devId >> 1, AA_I2C_NO_FLAGS, 2, (u08*)prifAddr, 
					&num_written, (u16)length, (u08*)data_in, &num_read))
                {
                    printk("failed to read 0x%x - 0x%x\n", devId, address);
					return SII_PLATFORM_STATUS__FAILED;
                }
			}
            return SII_PLATFORM_STATUS__SUCCESS;
		case SII_PLATFORM_TYPE__SPI:
            {
                uint8_t num_bytes_written = 0;
                //uint8_t num_bytes_read = 0;
                uint8_t spiAddr[2] = {0x00, 0x00};
                spiAddr[0] = (uint8_t)((address & 0xFF00) >> 8);
                spiAddr[1] = (uint8_t)(address & 0x00FF);
                // first write the address. ignore the data received while writing address
                num_bytes_written = aa_spi_write(pInterfaceInfo->handle, (u16)2, spiAddr,
                                     (u16)2, data_in);

                if (num_bytes_written < 0) {
                    printf("error: %s\n", aa_status_string(num_bytes_written));
                    return SII_PLATFORM_STATUS__FAILED;
                } else if (num_bytes_written != 2) {
                    printf("error: only a partial number of bytes written\n");
                    printf("  (%d) instead of full (%d)\n", num_bytes_written, 2);
                    return SII_PLATFORM_STATUS__FAILED;
                }
                //in above see if a loop is required
                for (;;)
                {
                    uint16_t total_bytes_read = 0;
                    uint16_t num_bytes_read = 0;
        
                    // Read the SPI message.
                    // This function has an internal timeout (see datasheet).
                    // To use a variable timeout the function aa_async_poll could
                    // be used for subsequent messages.
                    num_bytes_read = aa_spi_slave_read(pInterfaceInfo->handle, length - num_bytes_read, data_in+num_bytes_read);

                    if (num_bytes_read < 0 && num_bytes_read != AA_SPI_SLAVE_TIMEOUT) {
                        printf("error: %s\n", aa_status_string(num_bytes_read));
                        return SII_PLATFORM_STATUS__FAILED;
                    }
                    else {
                        total_bytes_read = total_bytes_read + num_bytes_read;
                    }
                    if(total_bytes_read == length)
                    {
                        return SII_PLATFORM_STATUS__SUCCESS;
                    }
                }
            }
            break;
		default:
			return SII_PLATFORM_STATUS__FAILED;
		}
		break;
	case SII_PLATFORM_HARDWARE__CHEETAH:
		switch(pInterfaceInfo->interfaceType)
		{
		case SII_PLATFORM_TYPE__I2C:
			break;
		case SII_PLATFORM_TYPE__SPI:
			break;
		default:
			return SII_PLATFORM_STATUS__FAILED;
		}
		break;

	case SII_PLATFORM_HARDWARE__HISI:
		switch(pInterfaceInfo->interfaceType)
		{
    		case SII_PLATFORM_TYPE__APB:
                for (; count < length; count++)  
                {
                    *(data_in + count) = (uint8_t)(*(addr + count));
                }
    			break;
    		default:
    			return SII_PLATFORM_STATUS__FAILED;
		}
		break;
    default:
        return SII_PLATFORM_STATUS__FAILED;
	}
#endif
    return SII_PLATFORM_STATUS__SUCCESS;
}

SiiPlatformStatus_t SiiPlatformWrite(SiiPlatformInterface_t *pInterfaceInfo, uint8_t devId, uint16_t address, uint8_t *data_out, uint16_t length)
{
    uint16_t count = 0;
#if 1 
    volatile uint32_t *addr = (uint32_t *)IO_ADDRESS((((address - BASE_ADDRESS)*4)| HDMI_CTRL_BASE_ADDR));
    for (; count < length; count++)  
    {
        *(addr + count) = *(data_out + count);
    }  
#endif
#if 0    
 	switch(pInterfaceInfo->interfaceHardware)
	{

	case SII_PLATFORM_HARDWARE__BB:
		switch(pInterfaceInfo->interfaceType)
		{
		case SII_PLATFORM_TYPE__I2C:
		case SII_PLATFORM_TYPE__PRIF_OVER_I2C:
            SetI2C16bitAddress( (BB_HANDLE)(pInterfaceInfo->handle), (pInterfaceInfo->interfaceType == SII_PLATFORM_TYPE__PRIF_OVER_I2C)?1:0 );
			if(0 != I2C_WriteBlock((BB_HANDLE)pInterfaceInfo->handle, devId, (word)address, (byte *)data_out, (word)length))
            {
                printf("failed to write 0x%x - 0x%x\n", devId, address);
				return SII_PLATFORM_STATUS__FAILED;
            }
			else
				return SII_PLATFORM_STATUS__SUCCESS;
		default:
			return SII_PLATFORM_STATUS__FAILED;
		}
		break;
	case SII_PLATFORM_HARDWARE__AARDVARK:
		switch(pInterfaceInfo->interfaceType)
		{
		case SII_PLATFORM_TYPE__I2C:
			{
				u16 num_written;
				u08 data_final[256 + 1]; // +1 bytes for 1 byte of register offset
				data_final[0] = address;
				memcpy((uint8_t *)data_final+1, (uint8_t *)data_out, length);
				if(AA_I2C_STATUS_OK != aa_i2c_write_ext(pInterfaceInfo->handle, devId >> 1, AA_I2C_NO_FLAGS, (u16)length+1, data_final, &num_written))
                {
                    printf("failed to write 0x%x - 0x%x\n", devId, address);
					return SII_PLATFORM_STATUS__FAILED;
                }
				return SII_PLATFORM_STATUS__SUCCESS;
			}
		case SII_PLATFORM_TYPE__PRIF_OVER_I2C:
			{
				u16 num_written;
				u08 data_final[256 + 2]; // +2 bytes for 2 byte of register offset
				data_final[1] = address & 0xFF;
				data_final[0] = (address >> 8) & 0xFF;
				memcpy((uint8_t *)data_final+2, (uint8_t *)data_out, length);
				if(AA_I2C_STATUS_OK != aa_i2c_write_ext(pInterfaceInfo->handle, devId >> 1, AA_I2C_NO_FLAGS, (u16)length+2, data_final, &num_written))
                {
                    printf("failed to write 0x%x - 0x%x\n", devId, address);
					return SII_PLATFORM_STATUS__FAILED;
                }
				return SII_PLATFORM_STATUS__SUCCESS;
			}
		case SII_PLATFORM_TYPE__SPI:
            {
                uint16_t total_num_bytes = 0;
                uint8_t num_bytes_written = 0;
                uint8_t spiAddr[2+256] = {0x00, 0x00};
                uint8_t redData[2+256]; //junk data received while writing
                spiAddr[0] = (uint8_t)((address & 0xFF00) >> 8);
                spiAddr[1] = (uint8_t)(address & 0x00FF);
                memcpy(spiAddr+2, data_out, length);
                for(;;)
                {
                    // first write the address. ignore the data received while writing address
                    num_bytes_written = aa_spi_write(pInterfaceInfo->handle, (u16)2+length, spiAddr,
                                         (u16)2+length, redData);

                    if (num_bytes_written < 0) 
                    {
                        printf("error: %s\n", aa_status_string(num_bytes_written));
                        return SII_PLATFORM_STATUS__FAILED;
                    } else
                    {
                        total_num_bytes += num_bytes_written;
                    }
                    if(total_num_bytes == length)
                    {
                        return SII_PLATFORM_STATUS__SUCCESS;
                    }
                }   
            }
		    break;
 		default:
			return SII_PLATFORM_STATUS__FAILED;
		}
	case SII_PLATFORM_HARDWARE__CHEETAH:
		switch(pInterfaceInfo->interfaceType)
		{
		case SII_PLATFORM_TYPE__I2C:
			break;
		case SII_PLATFORM_TYPE__SPI:
			break;
		default:
			return SII_PLATFORM_STATUS__FAILED;
		}
		break;
        
	case SII_PLATFORM_HARDWARE__HISI:
		switch(pInterfaceInfo->interfaceType)
		{
    		case SII_PLATFORM_TYPE__APB:
                for (; count < length; count++)  
                {
                    *(addr + count) = *(data_out + count);
                }
    			break;
    		default:
    			return SII_PLATFORM_STATUS__FAILED;
		}
		break;
	default:
		return SII_PLATFORM_STATUS__FAILED;
	}
#endif    
    return SII_PLATFORM_STATUS__SUCCESS;
}
#if 0
SiiPlatformStatus_t SiiPlatformGPIOConfig(SiiPlatformInterface_t *pInterfaceInfo, SiiPlatformGPIO_t gpioPin, SiiPlatformGPIODirection_t direction)
{

	switch(pInterfaceInfo->interfaceHardware)
	{
	case SII_PLATFORM_HARDWARE__BB:
		if((gpioPin <= SII_PLATFORM_GPIO__7) && (gpioPin >= SII_PLATFORM_GPIO__0))
		{
			GPIO_OPTIONS gpioOptions;
			uint8_t gpioCfgVal;
			if( GPIO_GetOptions( (BB_HANDLE)pInterfaceInfo->handle, &gpioOptions ) )
			{
				return SII_PLATFORM_STATUS__FAILED; // failed
			}
			switch(direction)
			{
			case SII_PLATFORM_GPIO_DIRECTION__INPUT:
				gpioCfgVal = GPIO_INPUT;
				break;
			case SII_PLATFORM_GPIO_DIRECTION__OUTPUT:
				gpioCfgVal = GPIO_PUSH_PULL;
				break;
			case SII_PLATFORM_GPIO_DIRECTION__INOUT:
				gpioCfgVal = GPIO_BIDI;
				break;
			default:
				return SII_PLATFORM_STATUS__FAILED;
			}
			switch(gpioPin)
			{
			case 0:
				gpioOptions.gpioCfg0 = gpioCfgVal;
				break;
			case 1:
				gpioOptions.gpioCfg1 = gpioCfgVal;
				break;
			case 2:
				gpioOptions.gpioCfg2 = gpioCfgVal;
				break;
			case 3:
				gpioOptions.gpioCfg3 = gpioCfgVal;
				break;
			case 4:
				gpioOptions.gpioCfg4 = gpioCfgVal;
				break;
			case 5:
				gpioOptions.gpioCfg5 = gpioCfgVal;
				break;
			case 6:
				gpioOptions.gpioCfg6 = gpioCfgVal;
				break;
			case 7:
				gpioOptions.gpioCfg7 = gpioCfgVal;
				break;
			}
			if( GPIO_SetOptions( (BB_HANDLE)pInterfaceInfo->handle, gpioOptions ) )
			{
				return SII_PLATFORM_STATUS__FAILED; // failed
			}
			return SII_PLATFORM_STATUS__SUCCESS;
 		}
		return SII_PLATFORM_STATUS__FAILED;
	case SII_PLATFORM_HARDWARE__AARDVARK:
		{
			uint8_t gpioMask;
			switch(pInterfaceInfo->interfaceType)
			{
			case SII_PLATFORM_TYPE__I2C:
			case SII_PLATFORM_TYPE__PRIF_OVER_I2C:
				gpioMask = 0x01;
				if((gpioPin > SII_PLATFORM_GPIO__5) || (gpioPin < SII_PLATFORM_GPIO__2))
				{
					return SII_PLATFORM_STATUS__FAILED;
				}
				break;
			case SII_PLATFORM_TYPE__SPI:
				gpioMask = 0x01;
				if((gpioPin > SII_PLATFORM_GPIO__1) || (gpioPin < SII_PLATFORM_GPIO__0))
				{
					return SII_PLATFORM_STATUS__FAILED;
				}
				break;
			case SII_PLATFORM_TYPE__GPIO:
				gpioMask = 0x01;
				if((gpioPin > SII_PLATFORM_GPIO__5) || (gpioPin < SII_PLATFORM_GPIO__0))
				{
					return SII_PLATFORM_STATUS__FAILED;
				}
				break;
            case SII_PLATFORM_TYPE__APB: 
                return SII_PLATFORM_STATUS__SUCCESS;
                break;
			}
			{
				uint8_t gpioCfgVal;
				gpioCfgVal = aa_gpio_get(pInterfaceInfo->handle);
				switch(direction)
				{
				case SII_PLATFORM_GPIO_DIRECTION__INPUT:
					gpioCfgVal &= ~(gpioMask << (gpioPin));
					aa_gpio_direction(pInterfaceInfo->handle, gpioCfgVal);
					break;
				case SII_PLATFORM_GPIO_DIRECTION__OUTPUT:
					gpioCfgVal |= (gpioMask << (gpioPin));
					aa_gpio_direction(pInterfaceInfo->handle, gpioCfgVal);
					break;
				case SII_PLATFORM_GPIO_DIRECTION__INOUT:
					return SII_PLATFORM_STATUS__FAILED;
				default:
					return SII_PLATFORM_STATUS__FAILED;
				}
				return SII_PLATFORM_STATUS__SUCCESS;
			}
		}
        break;
	case SII_PLATFORM_HARDWARE__CHEETAH:
		break;
	case SII_PLATFORM_HARDWARE__HISI:
        return SII_PLATFORM_STATUS__SUCCESS;
		break;        
	default:
		break;
	}
	return SII_PLATFORM_STATUS__FAILED;
}

SiiPlatformStatus_t SiiPlatformGPIOSet(SiiPlatformInterface_t *pInterfaceInfo, SiiPlatformGPIO_t gpioPin, SiiPlatformGPIOLevel_t level)
{

	switch(pInterfaceInfo->interfaceHardware)
	{
	case SII_PLATFORM_HARDWARE__BB:
		if((gpioPin <= SII_PLATFORM_GPIO__7) && (gpioPin >= SII_PLATFORM_GPIO__1))
		{
			if(level == SII_PLATFORM_GPIO_LEVEL__HIGH)
			{
				GPIO_SetPins( (BB_HANDLE)pInterfaceInfo->handle, 1 << gpioPin );
			}
			else
				GPIO_ClearPins( (BB_HANDLE)pInterfaceInfo->handle, 1 << gpioPin );

			return SII_PLATFORM_STATUS__SUCCESS;
		}
		break;
	case SII_PLATFORM_HARDWARE__AARDVARK:
		{
			uint8_t gpioMask = 0x01;
			uint8_t value = 0;
			int gpiostat = 0;
            gpiostat = aa_gpio_get(pInterfaceInfo->handle);
			switch(pInterfaceInfo->interfaceType)
			{
			case SII_PLATFORM_TYPE__I2C:
			case SII_PLATFORM_TYPE__PRIF_OVER_I2C:
				if((gpioPin > SII_PLATFORM_GPIO__5) || (gpioPin < SII_PLATFORM_GPIO__2))
				{
					return SII_PLATFORM_STATUS__FAILED;
				}
				break;
			case SII_PLATFORM_TYPE__SPI:
				if((gpioPin > SII_PLATFORM_GPIO__1) || (gpioPin < SII_PLATFORM_GPIO__0))
				{
					return SII_PLATFORM_STATUS__FAILED;
				}
				break;
			case SII_PLATFORM_TYPE__GPIO:
				if((gpioPin > SII_PLATFORM_GPIO__5) || (gpioPin < SII_PLATFORM_GPIO__0))
				{
					return SII_PLATFORM_STATUS__FAILED;
				}
				break;
			default:
				return SII_PLATFORM_STATUS__FAILED;
			}
			value = (level == SII_PLATFORM_GPIO_LEVEL__HIGH) ? (gpiostat | (gpioMask << gpioPin)) : (gpiostat & ~(gpioMask << gpioPin));
			if (AA_OK != aa_gpio_set(pInterfaceInfo->handle, value))
			{
				SII_LIB_LOG_PRINT2(("Aardvark : Failed to set gpio[%d]\n", gpioPin));
				return SII_PLATFORM_STATUS__FAILED;
			}
			return SII_PLATFORM_STATUS__SUCCESS;
		}
		break;
	case SII_PLATFORM_HARDWARE__CHEETAH:
		break;
 	case SII_PLATFORM_HARDWARE__HISI:
        return SII_PLATFORM_STATUS__SUCCESS;
		break;       
	default:
		break;
    }
	return SII_PLATFORM_STATUS__FAILED;

}

SiiPlatformGPIOLevel_t SiiPlatformGPIOStatusGet(SiiPlatformInterface_t *pInterfaceInfo, SiiPlatformGPIO_t gpioPin)
{

	switch(pInterfaceInfo->interfaceHardware)
	{
	case SII_PLATFORM_HARDWARE__BB:
		return  GPIO_GetPins((BB_HANDLE)pInterfaceInfo->handle, 1 << gpioPin) ? SII_PLATFORM_GPIO_LEVEL__HIGH : SII_PLATFORM_GPIO_LEVEL__LOW;
	case SII_PLATFORM_HARDWARE__AARDVARK:
		{
			int gpiostat = 0;
			gpiostat = aa_gpio_get(pInterfaceInfo->handle);
			return (gpiostat & (0x01 << gpioPin)) ? SII_PLATFORM_GPIO_LEVEL__HIGH : SII_PLATFORM_GPIO_LEVEL__LOW;
		}
		break;
	case SII_PLATFORM_HARDWARE__CHEETAH:
		return SII_PLATFORM_GPIO_LEVEL__LOW;
	case SII_PLATFORM_HARDWARE__HISI:
        return 0;
		break;        
	default:
		return SII_PLATFORM_GPIO_LEVEL__LOW;
	}

}

void SiiPlatformTimeMilliDelay( uint32_t millDelay )
{
	SiI_DelayMS((unsigned long)millDelay);
}

uint32_t SiiPlatformTimeMilliGet(void)
{
	return Sii_Get_Process_Time();
}
#endif 

#if (SII_ENV_BUILD_ASSERT != 0)
void SiiPlatformDebugAssert( const char *pFileName, uint32_t lineNumber,
        uint32_t expressionEvaluation, const char *pConditionText )
{
    // The following two parameters are currently not used
    expressionEvaluation = expressionEvaluation; //to avoid compiler warnings about unused variable
    pConditionText       = pConditionText; // to avoid compiler warnings about unused variable

#if 1
    pFileName = pFileName; // to avoid compiler warnings about unused variable
    lineNumber = lineNumber; // to avoid compiler warnings about unused variable
#else
    // Example how to print the place where error happened:
    // Print file name and line number caused an assertion
    printk("\n\nAssertion failure: File %s, Line %d\n\n", pFileName, (int) lineNumber);
#endif
        printk("Error In %s,Line %d\n",pFileName,lineNumber);       

    // Stop SW execution to make the issue noticeable
    // while(true) {}; // Note: just an example
}
#endif// SII_ENV_BUILD_ASSERT


