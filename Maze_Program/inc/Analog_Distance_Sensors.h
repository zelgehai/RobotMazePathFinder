/**
 * @file Analog_Distance_Sensor.h
 * @brief Header file for the Analog_Distance_Sensor driver.
 *
 * This file contains the function definitions for the Analog_Distance_Sensor driver.
 * It uses the ADC14 module to interface with three Sharp GP2Y0A21YK0F Analog Distance Sensors.
 *
 * The Sharp GP2Y0A21YK0F Analog Distance Sensors uses the following pinout:
 *  - Right GP2Y0A21YK0F VOUT   <-->  MSP432 LaunchPad Pin P9.0 (Analog Input, A17)
 *  - Center GP2Y0A21YK0F VOUT  <-->  MSP432 LaunchPad Pin P6.1 (Analog Input, A14)
 *  - Left GP2Y0A21YK0F VOUT    <-->  MSP432 LaunchPad Pin P9.1 (Analog Input, A16)
 *  - GP2Y0A21YK0F VCC          <-->  MSP432 LaunchPad 5V
 *  - GP2Y0A21YK0F GND          <-->  MSP432 LaunchPad GND
 *
 * @author Aaron Nanas
 *
 */

#ifndef INC_ANALOG_DISTANCE_SENSORS_H_
#define INC_ANALOG_DISTANCE_SENSORS_H_

#include <stdint.h>
#include "msp.h"

#define Ax 1195159
#define Bx -1058
#define Cx 40
#define ANALOG_DISTANCE_SENSOR_MAX 2552

/**
 * @brief Initialize the Sharp GP2Y0A21YK0F Analog Distance Sensors and configure ADC14 settings.
 *
 * This function initializes the Sharp GP2Y0A21YK0F Analog Distance Sensors and sets up the necessary configurations for the Analog-to-Digital Converter (ADC).
 * The ADC14 module will be used to measure analog voltage from the sensors and convert it into digital data.
 *
 * The function configures various registers of the ADC to control its behavior, such as the clock source, resolution, and input channels.
 * It also configures the corresponding GPIO pins for analog input.
 *
 * @return None
 */
void Analog_Distance_Sensor_Init();

/**
 * @brief Start ADC conversion and retrieve distance sensor data.
 *
 * This function initiates an ADC conversion to measure analog voltages from three different channels: A17, A14, and A16.
 * The function waits for the ADC to complete the conversion and retrieves the conversion results for each channel.
 *
 * @param Ch_17 Pointer to store the ADC result for channel A17 (P9.0).
 * @param Ch_14 Pointer to store the ADC result for channel A14 (P6.1).
 * @param Ch_16 Pointer to store the ADC result for channel A16 (P9.1).
 *
 * @return None
 */
void Analog_Distance_Sensor_Start_Conversion(uint32_t *Ch_17, uint32_t *Ch_14, uint32_t *Ch_16);

/**
 * @brief Calibrate the distance sensor reading based on a filtered distance value.
 *
 * This function calibrates the distance sensor reading based on a filtered distance value.
 * It applies a calibration formula to adjust the sensor data, providing a calibrated result.
 * The following calibration formula is used:
 *  Dx = (Ax / (filtered_distance + Bx) + Cx)
 *
 * @param filtered_distance The filtered distance value obtained from the sensor.
 *
 * @return Calibrated distance value, or 800 if the filtered distance is less than ANALOG_DISTANCE_SENSOR_MAX.
 */
int32_t Analog_Distance_Sensor_Calibrate(int filtered_distance);

#endif /* INC_ANALOG_DISTANCE_SENSORS_H_ */
