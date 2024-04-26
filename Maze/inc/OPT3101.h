/**
 * @file      opt3101.h
 * @brief     Mid-level software driver for the OPT3101 time of flight sensor
 * @details   MSP432 is master. EUSCI_B1 I2C is connected to OPT3101<br>
 * This file originally comes from the TIDA-010021 Firmware (tidcf48.zip) and<br>
 * was modified by Pololu to support the MSP432P401R.
 * @version   TI-RSLK MAX v1.1
 * @author    Daniel Valvano and Jonathan Valvano
 * @copyright Copyright 2019 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      December 22, 2019
<table>
<caption id="OPT3101">I2C/OPT3101 connected an external device</caption>
<tr><th>Pin   <th>signal
<tr><td>GND   <td>ground
<tr><td>VIN   <td>3.3V
<tr><td>P6.4  <td>I2C SDA data (bidirectional)
<tr><td>P6.5  <td>I2C SCL clock (MSP432 output)
<tr><td>GP1   <td>OPT3101 pin 11 -> P6.2/AUXR/GP1 input from the DATA_RDY
<tr><td>GP2   <td>OPT3101 pin 12 -> P10.1 Not used
<tr><td>RST_MS<td>OPT3101 pin 17 <- P6.3/AUXL/nRST_MS output low to reset the OPT3101
</table>
 ******************************************************************************/


/* This example accompanies the book
   "Embedded Systems: Introduction to Robotics,
   Jonathan W. Valvano, ISBN: 9781074544300, copyright (c) 2019
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2019, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <msp.h>
#include "../inc/Clock.h"
#include "../inc/EUSCI_B1_I2C.h"

/**
 * Resets the OPT3101 distance sensor using its reset line and then waits for
 * it to be done loading its initial settings from the on-board EEPROM memory.
 * @param  none
 * @return none
 * @brief  Initialize OPT3101.
 */
void OPT3101_Init(void);


/**
 * Writes some settings to the OPT3101 which are generally necessary to use
 * the other parts of this library.
 * @param  none
 * @return none
 * @brief  Sets up the OPT3101.
 */
void OPT3101_Setup(void);

/**
 * Tells the OPT3101 to do its internal crosstalk calibration.  This
 * takes about 1.3 seconds and is necessary every time you start using the
 * device.
 * @param  none
 * @return none
 * @brief  Calibrates for internal crosstalk
 */
void OPT3101_CalibrateInternalCrosstalk(void);

/**
 * Starts a new measurement using whatever the last channel was.
 * It takes about 33ms to complete the measurement.
 * @param  none
 * @return none
 * @brief  Start measurement
 */
void OPT3101_StartMeasurement(void);

/**
 * Starts a new measurement using specified channel.
 * It takes about 33ms to complete the measurement.
 * @param  ch is 0,1,2 for channel
 * @return none
 * @brief  Start measurement on a specific channel
 */
void OPT3101_StartMeasurementChannel(uint32_t ch);

/**
 * Reads measurement data from the OPT3101.
 * This data is stored by the library, and you can use the following functions
 * to access it:<br>
 *  - OPT3101_MeasurementError()        <br>
 *  - OPT3101_GetDistanceMillimeters()  <br>
 *  - OPT3101_GetAmplitude()            <br>
 *  - OPT3101_GetTxChannel()
 * @param  none
 * @return none
 * @brief  Reads measurement
 */
void OPT3101_ReadMeasurement(void);

/**
 * Returns true if the OPT3101 has reported an error associated with the
 * measurement.
 * This function does not perform any I/O; it just retrieves information about
 *  the measurement data previously read by OPT3101_ReadMeasurement().
 * @param  none
 * @return true if error, false if no error
 * @brief  check for error
 */
bool OPT3101_MeasurementError(void);

/**
 * Calculates the distance to the target object in millimeters and returns
 * it.
 * This is simply the value of the PHASE_OUT output multiplied by a scaling
 * factor.
 * This function does not perform any I/O; it just retrieves information about
 * the measurement data previously read by OPT3101_ReadMeasurement().
 * @param  none
 * @return distance in mm
 * @brief  last distance measurement in mm
 * @note values over 1000 are not valid
 */
uint32_t OPT3101_GetDistanceMillimeters(void);

/**
 * Returns the value of the PHASE_OUT output, which generally represents
 * a distance to a target object.
 * This function does not perform any I/O; it just retrieves information about
 * the measurement data previously read by OPT3101_ReadMeasurement().
 * @param  none
 * @return phase
 * @brief  last phase measurement
 */
uint32_t OPT3101_GetPhase(void);

/**
 * Returns the value of the AMP_OUT output, which generally represents
 * how much light was reflected from a target object to the photo diode.
 * This function does not perform any I/O; it just retrieves information about
 * the measurement data previously read by OPT3101_ReadMeasurement().
 * @param  none
 * @return amplitude
 * @brief  last amplitude measurement
 */
uint32_t OPT3101_GetAmplitude(void);

/**
 * Returns the transmitter channel number that the last reading came from.
 * This function should return 0, 1, or 2, if everything is working, but if
 * there are communication errors then it could return 3.
 * This function does not perform any I/O; it just retrieves information about
 * the measurement data previously read by OPT3101_ReadMeasurement().
 * @param  none
 * @return channel
 * @brief  channel of last measurement
 */
uint32_t OPT3101_GetTxChannel(void);

/**
 * Checks to see if measurement is complete. Busy wait utilization<br>
 * 1) Start using OPT3101_StartMeasurementChannel(channel);<br>
 * 2) Wait for complete using OPT3101_CheckDistanceSensor();<br>
 * 3) Get results using OPT3101_GetMeasurement();<br>
 * @param  none
 * @return true if we have a new distance measurement ready to be read, false otherwise.
 * @brief  new measurement ready
 */
bool OPT3101_CheckDistanceSensor(void);

/**
 * Reads results and returns values, assumes a new measurement is complete.
 * This function will set one of the three entries in the array, depending on which channel
 * was measured. distances will have values in mm, and amplitudes will have
 * amplitude values.
 * Busy wait utilization<br>
 * 1) Start using OPT3101_StartMeasurementChannel(channel);<br>
 * 2) Wait for complete using OPT3101_CheckDistanceSensor();<br>
 * 3) Get results using OPT3101_GetMeasurement();<br>
 * @param  distances array for each of the three channels
 * @param  amplitudes array for each of the three channels
 * @return channel number for the last measurement
 * @brief  get measurements from last measurement
 */
uint32_t OPT3101_GetMeasurement(uint32_t distances[3], uint32_t amplitudes[3]);

/**
 * Configure the OPT3101 for continuous, interrupt driven measurements.
 * It takes about 33ms to complete one measurement.
 * An interrupt occurs when a new measurement is complete.
 * The ISR will set one of the three entries in the array, depending on which channel
 * was measured. distances will have values in mm, and amplitudes will have
 * amplitude values. It will also update the channel parameter
 * Interrupt utilization<br>
 * 1) Start using OPT3101_StartMeasurementChannel(channel);<br>
 * 2) ISR updates values in the arrays<br>
 * @param  pTxChan to a variable for the channel value
 * @param  distances array for each of the three channels
 * @param  amplitudes array for each of the three channels
 * @return none
 * @brief  get measurements from last measurement
 */
void OPT3101_ArmInterrupts(uint32_t *pTxChan, uint32_t distances[3], uint32_t amplitudes[3]);
