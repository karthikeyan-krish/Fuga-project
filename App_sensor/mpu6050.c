
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrfx_twim.h"
#include "mpu6050.h"
#include "nrf_log.h"



//Initializing TWI0 instance
#define TWIM_INSTANCE_ID     1

// A flag to indicate the transfer state
static volatile bool m_xfer_done = false;


// Create a Handle for the twi communication
static const nrfx_twim_t m_twim = NRFX_TWIM_INSTANCE(TWIM_INSTANCE_ID);




//Event Handler
void twim_handler(nrfx_twim_evt_t const * p_event, void * p_context)
{
    //Check the event to see what type of event occurred
    switch (p_event->type)
    {
        //If data transmission or receiving is finished
	case NRFX_TWIM_EVT_DONE:
        m_xfer_done = true;//Set the flag
        break;
        
        default:
        // do nothing
          break;
    }
}



//Initialize the TWI as Master device
void twim_init(void)
{
    ret_code_t err_code;

    // Configure the settings for twi communication
    const nrfx_twim_config_t twim_config = {
       .scl                = TWI_SCL_M,  //SCL Pin
       .sda                = TWI_SDA_M,  //SDA Pin
       .frequency          = NRF_TWIM_FREQ_100K, //Communication Speed
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH, //Interrupt Priority(Note: if using Bluetooth then select priority carefully)
       .hold_bus_uninit     = false //automatically clear bus
    };



    //A function to initialize the twi communication
    err_code = nrfx_twim_init(&m_twim, &twim_config, twim_handler, NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("twim init success");
    //Enable the TWI Communication
    nrfx_twim_enable(&m_twim);
}



/*
   A function to write a Single Byte to MPU6050's internal Register
*/ 
bool mpu6050_register_write(uint8_t register_address, uint8_t value)
{
    ret_code_t err_code;
    uint8_t tx_buf[MPU6050_ADDRESS_LEN+1];
	
    //Write the register address and data into transmit buffer
    tx_buf[0] = register_address;
    tx_buf[1] = value;

    //Set the flag to false to show the transmission is not yet completed
    m_xfer_done = false;
    
    //Transmit the data over TWI Bus
   // err_code = nrf_drv_twi_tx(&m_twi, MPU6050_ADDRESS, tx_buf, MPU6050_ADDRESS_LEN+1, false);

    nrfx_twim_xfer_desc_t normal_mode_config = NRFX_TWIM_XFER_DESC_TX(MPU6050_ADDRESS, tx_buf, MPU6050_ADDRESS_LEN+1);
    
    err_code = nrfx_twim_xfer(&m_twim, &normal_mode_config, NULL);

    while (m_xfer_done == false);

    
    

    // if there is no error then return true else return false
    if (NRF_SUCCESS != err_code)
    {
        return false;
    }
    
    return true;	
}




/*
  A Function to read data from the MPU6050
*/ 
bool mpu6050_register_read(uint8_t register_address, uint8_t * destination, uint8_t number_of_bytes)
{
    ret_code_t err_code;

    //Set the flag to false to show the receiving is not yet completed
    m_xfer_done = false;
    
    // Send the Register address where we want to write the data
    //err_code = nrf_drv_twi_tx(&m_twi, MPU6050_ADDRESS, &register_address, 1, true);

    nrfx_twim_xfer_desc_t normal_mode_config = NRFX_TWIM_XFER_DESC_TX(MPU6050_ADDRESS, &register_address, 1);
    
    err_code = nrfx_twim_xfer(&m_twim, &normal_mode_config, NULL);


	  
    //Wait for the transmission to get completed
    while (m_xfer_done == false){}
    
    // If transmission was not successful, exit the function with false as return value
    if (NRF_SUCCESS != err_code)
    {
        return false;
    }

    //set the flag again so that we can read data from the MPU6050's internal register
    m_xfer_done = false;
	  
    // Receive the data from the MPU6050
    //err_code = nrf_drv_twi_rx(&m_twi, MPU6050_ADDRESS, destination, number_of_bytes);

    nrfx_twim_xfer_desc_t lis3d_reading = NRFX_TWIM_XFER_DESC_RX(MPU6050_ADDRESS, destination, number_of_bytes);

    /* Read 1 byte from the specified address - skip 3 bits dedicated for fractional part of temperature. */
    
    err_code = nrfx_twim_xfer(&m_twim, &lis3d_reading, NULL);
		
    //wait until the transmission is completed
    while (m_xfer_done == false){}
	
    // if data was successfully read, return true else return false
    if (NRF_SUCCESS != err_code)
    {
        return false;
    }
    
    return true;
}



/*
  A Function to verify the product id
  (its a basic test to check if we are communicating with the right slave, every type of I2C Device has 
  a special WHO_AM_I register which holds a specific value, we can read it from the MPU6050 or any device
  to confirm we are communicating with the right device)
*/ 
bool mpu6050_verify_product_id(void)
{
    uint8_t who_am_i; // create a variable to hold the who am i value


    // Note: All the register addresses including WHO_AM_I are declared in 
    // MPU6050.h file, you can check these addresses and values from the
    // datasheet of your slave device.
    if (mpu6050_register_read(ADDRESS_WHO_AM_I, &who_am_i, 1))
    {
        if (who_am_i != MPU6050_WHO_AM_I)
        {
            return false;
             NRF_LOG_INFO("WHO nam I fail");
        }
        else
        {
            return true;
            NRF_LOG_INFO("WHO nam I success");
        }
    }
    else
    {
        return false;
    }
}


/*
  Function to initialize the mpu6050
*/ 
bool mpu6050_init(void)
{   
  bool transfer_succeeded = true;
	
  //Check the id to confirm that we are communicating with the right device
  
  transfer_succeeded &= mpu6050_verify_product_id();
	
  if(mpu6050_verify_product_id() == false)
    {
	return false;
      }

  // Set the registers with the required values, see the datasheet to get a good idea of these values
  (void)mpu6050_register_write(0x20 , 0x37); 
  (void)mpu6050_register_write(0x24 , 0x3C); 

  return transfer_succeeded;
}







/*
  A Function to read accelerometer's values from the internal registers of MPU6050
*/ 
bool MPU6050_ReadAcc( int16_t *pACC )
{
  uint8_t buf[6];
  bool ret = false;		
  
  if(mpu6050_register_read(MPU6050_ACC_OUT | 0x80, buf, 6) == true)
  {
    mpu6050_register_read(MPU6050_ACC_OUT | 0x80, buf, 6);
    
    *pACC = (buf[1] << 8) | buf[0];
    pACC++;

    *pACC= (buf[3] << 8) | buf[2];
    pACC++;

    *pACC = (buf[5] << 8) | buf[4];
    pACC++;
		
    ret = true;
    }
  
  return ret;
}





