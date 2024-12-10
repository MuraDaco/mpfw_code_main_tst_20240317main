/*DS3231 low level API - Reza Ebrahimi v1.0*/
#include "ds3231.h"

uint8_t g_master_txBuff[I2C_DATA_LENGTH];
uint8_t g_master_rxBuff[I2C_DATA_LENGTH];

/*function to transmit one byte of data to register_address on ds3231 (device_address: 0X68)*/
status_t time_i2c_write_single(uint8_t device_address, uint8_t register_address, uint8_t *data_byte)
{
	return time_i2c_write_multi(device_address, register_address, data_byte, 1);
}

/*function to transmit an array of data to device_address, starting from start_register_address*/
status_t time_i2c_write_multi(uint8_t device_address, uint8_t start_register_address, uint8_t *data_array, uint8_t data_length)
{
	status_t retVal     = kStatus_Fail;
	uint8_t subAddress	= start_register_address;

	/* Send master blocking data to slave */
	retVal = I2C_MasterStart(EXAMPLE_I2C_MASTER, device_address, kI2C_Write);
    if (retVal == kStatus_Success)
    {
        /* subAddress = 0x01, data = g_master_txBuff - write to slave.
          start + slaveaddress(w) + subAddress + length of data buffer + data buffer + stop*/
        retVal = I2C_MasterWriteBlocking(EXAMPLE_I2C_MASTER, &subAddress, 1, kI2C_TransferNoStopFlag);
        if (retVal == kStatus_Success)
        {
            retVal = I2C_MasterWriteBlocking(EXAMPLE_I2C_MASTER, data_array, data_length, kI2C_TransferDefaultFlag);
            if (retVal == kStatus_Success)
            {
                retVal = I2C_MasterStop(EXAMPLE_I2C_MASTER);
            }
        }
    }

    return retVal;
}

/*function to read one byte of data from register_address on ds3231*/
status_t time_i2c_read_single(uint8_t device_address, uint8_t register_address, uint8_t *data_byte)
{
	return time_i2c_read_multi(device_address, register_address, data_byte, 1);
}

/*function to read an array of data from device_address*/
status_t time_i2c_read_multi(uint8_t device_address, uint8_t start_register_address, uint8_t *data_array, uint8_t data_length)
{
	status_t retVal     = kStatus_Fail;

	uint8_t subAddress	= start_register_address;

    /* Receive blocking data from slave */
    /* subAddress = 0x01, data = g_master_rxBuff - read from slave.
      start + slaveaddress(w) + subAddress + repeated start + slaveaddress(r) + rx data buffer + stop */
    retVal = I2C_MasterStart(EXAMPLE_I2C_MASTER, device_address, kI2C_Write);
    if (retVal == kStatus_Success)
    {
        retVal = I2C_MasterWriteBlocking(EXAMPLE_I2C_MASTER, &subAddress, 1, kI2C_TransferNoStopFlag);
        if (retVal == kStatus_Success)
        {
            retVal = I2C_MasterRepeatedStart(EXAMPLE_I2C_MASTER, device_address, kI2C_Read);
            if (retVal == kStatus_Success)
            {
                retVal = I2C_MasterReadBlocking(EXAMPLE_I2C_MASTER, data_array, data_length, kI2C_TransferDefaultFlag);
                if (retVal == kStatus_Success)
                {
                    retVal = I2C_MasterStop(EXAMPLE_I2C_MASTER);
                }
            }
        }
    }

    return retVal;
}

/*function to initialize I2C peripheral in 100khz or 400khz*/
void ds3231_I2C_init	(void)
{
	i2c_master_config_t masterConfig;

	/*
     * masterConfig.debugEnable = false;
     * masterConfig.ignoreAck = false;
     * masterConfig.pinConfig = kI2C_2PinOpenDrain;
     * masterConfig.baudRate_Bps = 100000U;
     * masterConfig.busIdleTimeout_ns = 0;
     * masterConfig.pinLowTimeout_ns = 0;
     * masterConfig.sdaGlitchFilterWidth_ns = 0;
     * masterConfig.sclGlitchFilterWidth_ns = 0;
     */
    I2C_MasterGetDefaultConfig(&masterConfig);

    /* Change the default baudrate configuration */
    masterConfig.baudRate_Bps = I2C_BAUDRATE;

    /* Initialize the I2C master peripheral */
    I2C_MasterInit(EXAMPLE_I2C_MASTER, &masterConfig, I2C_MASTER_CLOCK_FREQUENCY);


}
