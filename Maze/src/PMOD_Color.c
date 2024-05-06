/**
 * @file PMOD_Color.c
 * @brief Source code for the PMOD_Color driver.
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

#include "../inc/PMOD_Color.h"

void PMOD_Color_Write_Register(uint8_t register_address, uint8_t register_data)
{
    uint8_t buffer[] =
    {
         register_address,
         register_data & 0xFF
    };

    EUSCI_B1_I2C_Send_Multiple_Bytes(PMOD_COLOR_ADDRESS, buffer, sizeof(buffer));
}

uint8_t PMOD_Color_Read_Register(uint8_t register_address)
{
    EUSCI_B1_I2C_Send_A_Byte(PMOD_COLOR_ADDRESS, register_address);
    uint8_t received_data = EUSCI_B1_I2C_Receive_A_Byte(PMOD_COLOR_ADDRESS);
    return received_data;
}

void PMOD_Color_Init()
{
    EUSCI_B1_I2C_Init();

    PMOD_Color_Enable(PMOD_COLOR_ENABLE_POWER_ON);

    Clock_Delay1us(2400);

    PMOD_Color_Enable(PMOD_COLOR_ENABLE_POWER_ON | PMOD_COLOR_ENABLE_RGBC);

    Clock_Delay1us(2400);

    PMOD_Color_LED_Init();
}

void PMOD_Color_LED_Init()
{
    P8->SEL0 &= ~0x08;
    P8->SEL1 &= ~0x08;
    P8->DIR |= 0x08;
    P8->OUT &= ~0x08;
}

void PMOD_Color_LED_Control(uint8_t led_enable)
{
    if (led_enable == 0x00)
    {
        P8->OUT &= ~0x08;
    }
    else
    {
        P8->OUT |= 0x08;
    }
}

void PMOD_Color_Enable(uint8_t register_data)
{
    PMOD_Color_Write_Register(PMOD_COLOR_CMD_REPEAT | PMOD_COLOR_ENABLE_REG, register_data);
}

uint8_t PMOD_Color_Get_Device_ID()
{
    uint8_t PMOD_Color_Device_ID = PMOD_Color_Read_Register(PMOD_COLOR_AUTO_INC | PMOD_COLOR_DEVICE_ID_REG);
    return PMOD_Color_Device_ID;
}

PMOD_Color_Data PMOD_Color_Get_RGBC()
{
    PMOD_Color_Data data;
    uint8_t color_buffer[8];

    EUSCI_B1_I2C_Send_A_Byte(PMOD_COLOR_ADDRESS, PMOD_COLOR_AUTO_INC | PMOD_COLOR_CDATA_L_REG);

    EUSCI_B1_I2C_Receive_Multiple_Bytes(PMOD_COLOR_ADDRESS, color_buffer, 8);

    data.clear = (color_buffer[1] << 8) | color_buffer[0];
    data.red = (color_buffer[3] << 8) | color_buffer[2];
    data.green = (color_buffer[5] << 8) | color_buffer[4];
    data.blue = (color_buffer[7] << 8) | color_buffer[6];

    return data;
}

uint8_t PMOD_Color_Read_Raw_Color_Data(uint8_t register_address)
{
    uint8_t PMOD_Color_Byte = PMOD_Color_Read_Register(PMOD_COLOR_AUTO_INC | register_address);
    return PMOD_Color_Byte;
}

PMOD_Calibration_Data PMOD_Color_Init_Calibration_Data(PMOD_Color_Data first_sample)
{
    PMOD_Calibration_Data calibration_data;

    calibration_data.min = first_sample;
    calibration_data.max = first_sample;

    return calibration_data;
}

void PMOD_Color_Calibrate(PMOD_Color_Data new_sample, PMOD_Calibration_Data *calibration_data)
{
    if (new_sample.clear < calibration_data->min.clear) calibration_data->min.clear = new_sample.clear;
    if (new_sample.red < calibration_data->min.red) calibration_data->min.red = new_sample.red;
    if (new_sample.green < calibration_data->min.green) calibration_data->min.green = new_sample.green;
    if (new_sample.blue < calibration_data->min.blue) calibration_data->min.blue = new_sample.blue;

    if (new_sample.clear > calibration_data->max.clear) calibration_data->max.clear = new_sample.clear;
    if (new_sample.red > calibration_data->max.red) calibration_data->max.red = new_sample.red;
    if (new_sample.green > calibration_data->max.green) calibration_data->max.green = new_sample.green;
    if (new_sample.blue > calibration_data->max.blue) calibration_data->max.blue = new_sample.blue;
}

PMOD_Color_Data PMOD_Color_Normalize_Calibration(PMOD_Color_Data sample, PMOD_Calibration_Data calibration_data)
{
    PMOD_Color_Data normalized_data;

    normalized_data.clear = (sample.clear - calibration_data.min.clear) * (0xFFFF / (calibration_data.max.clear - calibration_data.min.clear));
    normalized_data.red = (sample.red - calibration_data.min.red) * (0xFFFF / (calibration_data.max.red - calibration_data.min.red));
    normalized_data.green = (sample.green - calibration_data.min.green) * (0xFFFF / (calibration_data.max.green - calibration_data.min.green));
    normalized_data.blue = (sample.blue - calibration_data.min.blue) * (0xFFFF / (calibration_data.max.blue - calibration_data.min.blue));

    return normalized_data;
}
