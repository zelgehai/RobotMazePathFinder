/**
 * @file OPT3001.c
 * @brief Source code for the OPT3001 driver.
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

#include "../inc/OPT3001.h"

// An enumeration that defines constants for various commands that can be sent
// to the OPT3001 Ambient Light Sensor. Each constant represents a specific command
// such as reading the result or configuring the sensor
typedef enum
{
    RESULT            = 0x00,
    CONFIG            = 0x01,
    LOW_LIMIT         = 0x02,
    HIGH_LIMIT        = 0x03,
    MANUFACTURER_ID   = 0x7E,
    DEVICE_ID         = 0x7F,
} OPT3001_Commands;

// A union that combines a bitfield struct with a uint16_t raw_data field.
// This is used to interpret and modify the configuration data for the
// OPT3001 Ambient Light Sensor. Its configuration settings are stored in specific
// bitfields within a 16-bit register
typedef union{
  struct
  {
    uint8_t FaultCount:                 2;
    uint8_t MaskExponent:               1;
    uint8_t Polarity:                   1;
    uint8_t Latch:                      1;
    uint8_t FlagLow:                    1;
    uint8_t FlagHigh:                   1;
    uint8_t ConversionReady:            1;
    uint8_t OverflowFlag:               1;
    uint8_t ModeOfConversionOperation:  2;
    uint8_t ConversionTime:             1;
    uint8_t RangeNumber:                4;
  };
  uint16_t RawData;
} OPT3001_Config;

// Declare a config struct used when reading the configuration register
OPT3001_Config Read_Sensor_Configuration;

/**
 * @brief Writes a single command byte to the OPT3001 light sensor via I2C.
 *
 * @param command The command byte to send to the sensor.
 *
 * @return None
 */
static void OPT3001_Write_Data(OPT3001_Commands command)
{
    EUSCI_B1_I2C_Send_Multiple_Bytes(OPT3001_ADDRESS, &command, 1);
}

/**
 * @brief Reads data from the OPT3001 light sensor via I2C.
 *
 * @param data Pointer to a uint16_t variable where the sensor data will be stored.
 *
 * @return None
 */
static void OPT3001_Read_Data(uint16_t* data)
{
    uint8_t buffer[2];
    EUSCI_B1_I2C_Receive_Multiple_Bytes(OPT3001_ADDRESS, buffer, 2);
    *data = buffer[1] + ((uint16_t)buffer[0] << 8);
}

/**
 * @brief Reads a register from the OPT3001 sensor.
 *
 * @param command The command to read the desired register.
 *
 * @return An OPT3001_Result structure containing the data that was read.
 */
OPT3001_Result static OPT3001_Read_Register(OPT3001_Commands command)
{
    OPT3001_Result result;
    OPT3001_Write_Data(command);
    OPT3001_Read_Data(&result.RawData);
    return result;
}

/**
 * @brief This function writes a 16-bit value to a specific register on the OPT3001 sensor via I2C communication.
 *
 * @param register_address The register address to write the data to.
 * @param register_data The 16-bit data to be written to the register.
 *
 * @return None
 */
static void OPT3001_Write_Register(uint8_t register_address, uint16_t register_data)
{
    uint8_t buffer[] =
    {
        register_address,
        (register_data >> 8) & 0xFF,
        register_data & 0xFF,
    };

    EUSCI_B1_I2C_Send_Multiple_Bytes(OPT3001_ADDRESS, buffer, sizeof(buffer));
    Clock_Delay1us(10);
}

/**
 * @brief This function writes an OPT3001_Config struct to the OPT3001 sensor's configuration registers.
 *
 * @param config The OPT3001_Config struct containing configuration data to be written.
 *
 * @return None
 */
static void OPT3001_Write_Configuration(OPT3001_Config config)
{
    OPT3001_Write_Register(CONFIG, config.RawData);
}

/**
 * @brief This function reads the configuration from the OPT3001 sensor and returns it in an OPT3001_Config struct
 *
 * @return An OPT3001_Config structure containing the sensor's configuration data.
 */
OPT3001_Config static OPT3001_Read_Configuration()
{
    OPT3001_Config config;
    OPT3001_Write_Data(CONFIG);
    OPT3001_Read_Data(&config.RawData);
    return config;
}

void OPT3001_Init()
{
    // Configure pins P4.2 and P4.5 as GPIO pins
    P4->SEL0 &= ~0x24;
    P4->SEL1 &= ~0x24;

    // Set the direction of P4.2 as input
    P4->DIR &= ~0x04;

    // Enable the internal pull-up resistor
    P4->REN |= 0x04;

    // Set the initial state of P4.2 as high since open drain requires a pull-up
    P4->OUT |= 0x04;

    // Set the direction of P4.5 as output
    P4->DIR |= 0x20;

    // Set the initial state of P4.5 as high to power the OPT3001
    P4->OUT |= 0x20;

    // Provide a short 1 ms delay after configuring pins
    Clock_Delay1ms(1);

    // Instantiate new configuration struct
    OPT3001_Config New_Config;

    // Set RN (Bits 15-12) to 0x0C
    // This configures the device to operate in automatic
    // full-scale setting mode
    New_Config.RangeNumber = 0x0C; // full automatic scale

    // Set CT (Bit 11) of Configuration Register to 0
    // This sets the conversion time to 100 ms
    // If CT = 1, then the conversion time would be 800 ms,
    // allowing for lower noise measurement
    New_Config.ConversionTime = 0;

    // Set L (Bit 4) to 1
    // This configures the device to function in latched window-style
    // comparison operation, latching the interrupt reporting mechanisms
    // until a user-controlled clearing event
    New_Config.Latch = 1;

    // Set M (Bits 10-9) to 3
    // This configures the device to perform continuous conversions
    New_Config.ModeOfConversionOperation = 3;

    // Write the new configuration values to the OPT3001 configuration register
    OPT3001_Write_Configuration(New_Config);

    // Read the sensor's configuration values after writing to it
    Read_Sensor_Configuration = OPT3001_Read_Configuration();
}

OPT3001_Result OPT3001_Read_Light()
{
    // Read the Result Register (offset = 00h)
    // This register contains the most recent light to digital conversion
    return OPT3001_Read_Register(RESULT);
}
