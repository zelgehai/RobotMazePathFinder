/**
 * @file PMOD_Color.h
 * @brief Header file for the PMOD_Color driver.
 *
 * This file contains the function definitions for the PMOD_Color driver.
 * It interfaces with the PMOD Color sensor module, which uses the I2C communication protocol.
 *  - Product Link: https://digilent.com/shop/pmod-color-color-sensor-module/
 *  - Reference Manual: https://digilent.com/reference/reference/pmod/pmodcolor/reference-manual
 *  - AMS TCS34725 Datasheet: https://ams.com/documents/20143/36005/TCS3472_DS000390_3-00.pdf
 *
 * The PMOD Color sensor module uses the following I2C configuration:
 *  - SCL Frequency: 400 kHz
 *  - MSB First
 *
 * The following connections must be made:
 *  - PMOD COLOR IO1 / ~INT     (Pin 1)     <-->  Not Connected
 *  - PMOD COLOR IO2 / LED_EN   (Pin 2)     <-->  MSP432 LaunchPad Pin P8.3
 *  - PMOD COLOR SCL            (Pin 3)     <-->  MSP432 LaunchPad Pin P6.5 (SCL)
 *  - PMOD COLOR SDA            (Pin 4)     <-->  MSP432 LaunchPad Pin P6.4 (SDA)
 *  - PMOD COLOR GND            (Pin 5)     <-->  MSP432 LaunchPad GND
 *  - PMOD COLOR VCC            (Pin 6)     <-->  MSP432 LaunchPad VCC (3.3V)
 *
 * For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @author Aaron Nanas
 *
 */

#ifndef INC_PMOD_COLOR_H_
#define INC_PMOD_COLOR_H_

#include <stdint.h>
#include "msp.h"
#include "EUSCI_B1_I2C.h"
#include "Clock.h"

typedef struct
{
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t clear;
} PMOD_Color_Data;

typedef struct
{
    PMOD_Color_Data min, max;
} PMOD_Calibration_Data;

// Default I2C address for the PMOD COLOR
#define PMOD_COLOR_ADDRESS                      0x29

// Register Bit Position Values
#define SET_BIT_0                               0x01
#define SET_BIT_1                               0x02
#define SET_BIT_2                               0x04
#define SET_BIT_3                               0x08
#define SET_BIT_4                               0x10
#define SET_BIT_5                               0x20
#define SET_BIT_6                               0x40
#define SET_BIT_7                               0x80

// Register Address Definitions
#define PMOD_COLOR_ENABLE_REG                   0x00
#define PMOD_COLOR_ATIME_REG                    0x01
#define PMOD_COLOR_WTIME_REG                    0x03
#define PMOD_COLOR_AILTL_REG                    0x04
#define PMOD_COLOR_AILTH_REG                    0x05
#define PMOD_COLOR_AIHTL_REG                    0x06
#define PMOD_COLOR_AIHTH_REG                    0x07
#define PMOD_COLOR_PERS_REG                     0x0C
#define PMOD_COLOR_CONFIG_REG                   0x0D
#define PMOD_COLOR_CONTROL_REG                  0x0F
#define PMOD_COLOR_DEVICE_ID_REG                0x12
#define PMOD_COLOR_STATUS_REG                   0x13
#define PMOD_COLOR_CDATA_L_REG                  0x14
#define PMOD_COLOR_CDATA_H_REG                  0x15
#define PMOD_COLOR_RDATA_L_REG                  0x16
#define PMOD_COLOR_RDATA_H_REG                  0x17
#define PMOD_COLOR_GDATA_L_REG                  0x18
#define PMOD_COLOR_GDATA_H_REG                  0x19
#define PMOD_COLOR_BDATA_L_REG                  0x1A
#define PMOD_COLOR_BDATA_H_REG                  0x1B

#define PMOD_COLOR_CMD_REPEAT                   0x08
#define PMOD_COLOR_AUTO_INC                     0xA0

#define PMOD_COLOR_ENABLE_POWER_ON              0x01
#define PMOD_COLOR_ENABLE_RGBC                  0x02

#define PMOD_COLOR_ENABLE_LED                   0x01
#define PMOD_COLOR_DISABLE_LED                  0x00

void PMOD_Color_Write_Register(uint8_t register_address, uint8_t register_data);

uint8_t PMOD_Color_Read_Register(uint8_t register_address);

void PMOD_Color_Init();

void PMOD_Color_LED_Init();

void PMOD_Color_LED_Control(uint8_t led_enable);

void PMOD_Color_Enable(uint8_t register_data);

uint8_t PMOD_Color_Get_Device_ID();

uint8_t PMOD_Color_Read_Raw_Color_Data(uint8_t register_address);

PMOD_Color_Data PMOD_Color_Get_RGBC();

PMOD_Calibration_Data PMOD_Color_Init_Calibration_Data(PMOD_Color_Data first_sample);

void PMOD_Color_Calibrate(PMOD_Color_Data new_sample, PMOD_Calibration_Data *calibration_data);

PMOD_Color_Data PMOD_Color_Normalize_Calibration(PMOD_Color_Data sample, PMOD_Calibration_Data calibration_data);

#endif /* INC_PMOD_COLOR_H_ */
