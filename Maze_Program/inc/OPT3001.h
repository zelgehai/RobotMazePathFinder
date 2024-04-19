/**
 * @file OPT3001.h
 * @brief Header file for the OPT3001 driver.
 *
 * This file contains the function definitions for the OPT3001 driver.
 *
 * The OPT3001 Ambient Light Sensor from the BP-BASSENSORSMKII Sensor BoosterPack is used.
 *
 * The OPT3001 driver uses the EUSCI_B1_I2C driver to access the registers
 * of the OPT3001 Ambient Light Sensor via I2C.
 *
 * @note This function assumes that the necessary pin configurations for the OPT3001 module have been performed
 *       on the corresponding pins.
 *
 * The OPT3001 Ambient Light Sensor uses the following pinout:
 *  - OPT3001 Pin 1 (VDD)       <-->  MSP432 LaunchPad Pin P4.5 (Device power)
 *  - OPT3001 Pin 2 (ADDR)      <-->  MSP432 LaunchPad GND (To set address 0x44 - Table 1 of datasheet)
 *  - OPT3001 Pin 3 (GND)       <-->  MSP432 LaunchPad GND
 *  - OPT3001 Pin 4 (SCL)       <-->  MSP432 LaunchPad Pin P6.5
 *  - OPT3001 Pin 5 (INT)       <-->  MSP432 LaunchPad Pin P4.2
 *  - OPT3001 Pin 6 (SDA)       <-->  MSP432 LaunchPad Pin P6.4
 *
 * @note Do not use with bumper sensors to avoid any pin conflicts.
 *
 * @note For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @note Refer to the OPT3001 Ambient Light Sensor datasheet for more information regarding the sensor.
 *
 * @author Aaron Nanas
 *
 */

#ifndef INC_OPT3001_H_
#define INC_OPT3001_H_

#include <stdint.h>
#include <msp.h>
#include "../inc/EUSCI_B1_I2C.h"
#include "../inc/Clock.h"

#define OPT3001_ADDRESS 0x44

/**
 * @brief The struct is used to define the individual bit fields within RawData
 * when it is being interpreted as a struct value
 *
 * RawData is a 16-bit unsigned integer that holds the raw data from the OPT3001 Ambient Light Sensor.
 *
 * Result is used to store the fractional result in straight binary coding (zero to full-scale) used for lux calculation
 *
 * Exponent contains the exponent portion used for lux calculation
 *
 * @note Details regarding the Result register can be found in section 7.6.1.1.1 of the OPT3001 datasheet
 */
typedef union
{
    uint16_t RawData;
    struct
    {
    uint16_t Result:    12;
    uint8_t Exponent:   4;
    };
} OPT3001_Result;

/**
 * @brief Initializes the OPT3001 light sensor module.
 *
 * This function configures the OPT3001 light sensor module by setting up the required
 * GPIO pins and initializing its operating parameters. It powers up the sensor, configures
 * its range, conversion time, and operating mode. The function also sets the latch style for
 * interrupt reporting. Ensure that the required hardware and peripheral configurations are
 * in place before calling this function.
 *
 * @note The OPT3001_Config structure defines the sensor's configuration parameters and should be
 *     correctly initialized before calling this function.
 *
 * @param None
 *
 * @return None
 */
void OPT3001_Init();

/**
 * @brief Reads the light intensity from the OPT3001 Ambient Light Sensor.
 *
 * This function reads the light intensity from the OPT3001 Ambient Light Sensor. by
 * accessing the Result register. The lux value is calculated using the
 * formula provided in the OPT3001 datasheet (Equation 3 on page 20):
 *
 * lux = 0.01 * (2^Exponent) * Result
 *
 * @note Ensure that the sensor is properly configured before calling this function.
 *
 * @note Details regarding the Result register can be found in section 7.6.1.1.1 of the OPT3001 datasheet
 *
 * @return An OPT3001_Result structure containing the raw light intensity data.
 */
OPT3001_Result OPT3001_Read_Light(void);

#endif /* INC_OPT3001_H_ */
