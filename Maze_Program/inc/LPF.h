/**
 * @file      LPF.h
 * @brief     implements three FIR low-pass filters
 * @details   Finite length LPF<br>
 1) Size is the depth 2 to 512<br>
 2) y(n) = (sum(x(n)+x(n-1)+...+x(n-size-1))/size<br>
 3) To use a filter<br>
   a) initialize it once<br>
   b) call the filter at the sampling rate<br>
 * @version   TI-RSLK MAX v1.1
 * @author    Daniel Valvano and Jonathan Valvano
 * @copyright Copyright 2019 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      June 28, 2019


 ******************************************************************************/
/*!
 * @defgroup Math
 * @brief
 * @{*/
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


/**
 * Initialize first LPF<br>
 * Set all data to an initial value<br>
 * @param initial value to preload into MACQ
 * @param size depth of the filter, 2 to 512
 * @return none
 * @note  all three filters share the same size
 * @brief  Initialize first LPF
 */
void LPF_Init(uint32_t initial, uint32_t size);

/**
 * First LPF, calculate one filter output<br>
 * Called at sampling rate
 * @param newdata new ADC data
 * @return result filter output
 * @brief  FIR low pass filter
 */
uint32_t LPF_Calc(uint32_t newdata);

/**
 * First LPF, calculate noise as standard deviation<br>
 * Called every time the buffer refills
 * @param none
 * @return standard deviation
 * @brief  calculate amount of random noise
 */
int32_t Noise(void);

/**
 * Initialize second LPF<br>
 * Set all data to an initial value<br>
 * @param initial value to preload into MACQ
 * @param size depth of the filter, 2 to 512
 * @return none
 * @note  all three filters share the same size
 * @brief  Initialize second LPF
 */
void LPF_Init2(uint32_t initial, uint32_t size);

/**
 * Second LPF, calculate one filter output<br>
 * Called at sampling rate
 * @param newdata new ADC data
 * @return result filter output
 * @brief  FIR low pass filter
 */
uint32_t LPF_Calc2(uint32_t newdata);

/**
 * Second LPF, calculate noise as standard deviation<br>
 * Called every time the buffer refills
 * @param none
 * @return standard deviation
 * @brief  calculate amount of random noise
 */
int32_t Noise2(void);

/**
 * Initialize third LPF<br>
 * Set all data to an initial value<br>
 * @param initial value to preload into MACQ
 * @param size depth of the filter, 2 to 512
 * @return none
 * @note  all three filters share the same size
 * @brief  Initialize third LPF
 */
void LPF_Init3(uint32_t initial, uint32_t size);

/**
 * Third LPF, calculate one filter output<br>
 * Called at sampling rate
 * @param newdata new ADC data
 * @return result filter output
 * @brief  FIR low pass filter
 */
uint32_t LPF_Calc3(uint32_t newdata);

/**
 * Third LPF, calculate noise as standard deviation<br>
 * Called every time the buffer refills
 * @param none
 * @return standard deviation
 * @brief  calculate amount of random noise
 */
int32_t Noise3(void);

/**
 * 3-wide non recursive Median filter <br>
 * Called with new data at sampling rate
 * @param newdata new ADC data
 * @return result filter output
 * @brief  Median filter
 */
int32_t Median(int32_t newdata);

/**
 * Newton's method square root
 * @param s is an integer
 * @return is an integer
 * @brief  square root
 */
uint32_t isqrt(uint32_t s);
