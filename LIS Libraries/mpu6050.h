#ifndef AT24C02_H__
#define AT24C02_H__
#include "nrf_delay.h"

//I2C Pins Settings, you change them to any other pins
#define TWI_SCL_M           22         //I2C SCL Pin
#define TWI_SDA_M           23         //I2C SDA Pin


#define MPU6050_ADDRESS_LEN  1         //MPU6050
#define MPU6050_ADDRESS     (0x18)  //MPU6050 Device Address
#define MPU6050_WHO_AM_I     0x33U     //MPU6050 ID


//#define MPU6050_GYRO_OUT        0x43
#define MPU6050_ACC_OUT         0x28

#define ADDRESS_WHO_AM_I          (0x0FU) //  WHO_AM_I register identifies the device. Expected value is 0x68.
#define ADDRESS_SIGNAL_PATH_RESET (0x33U) // 

//MPU6050 Registers addresses, see datasheet for more info and each register's function



void twim_init(void); // initialize the twi communication
bool mpu6050_init(void);    // initialize the mpu6050

/**
  @brief Function for writing a MPU6050 register contents over TWI.
  @param[in]  register_address Register address to start writing to
  @param[in] value Value to write to register
  @retval true Register write succeeded
  @retval false Register write failed
*/
bool mpu6050_register_write(uint8_t register_address, const uint8_t value);

/**
  @brief Function for reading MPU6050 register contents over TWI.
  Reads one or more consecutive registers.
  @param[in]  register_address Register address to start reading from
  @param[in]  number_of_bytes Number of bytes to read
  @param[out] destination Pointer to a data buffer where read data will be stored
  @retval true Register read succeeded
  @retval false Register read failed
*/
bool mpu6050_register_read(uint8_t register_address, uint8_t *destination, uint8_t number_of_bytes);

/**
  @brief Function for reading and verifying MPU6050 product ID.
  @retval true Product ID is what was expected
  @retval false Product ID was not what was expected
*/
bool mpu6050_verify_product_id(void);


bool MPU6050_ReadAcc( uint8_t *pACC);

#endif


