// LPF.c
// Runs on MSP432
// implements three FIR low-pass filters

// Jonathan Valvano
// September 12, 2017

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

#include <stdint.h>
#include "msp.h"
#include "../inc/LPF.h"

// Newton's method
// s is an integer
// sqrt(s) is an integer
uint32_t isqrt(uint32_t s){
uint32_t t;         // t*t will become s
int n;                   // loop counter to make sure it stops running
  t = s/10+1;            // initial guess
  for(n = 16; n; --n){   // guaranteed to finish
    t = ((t*t+s)/t)/2;
  }
  return t;
}

//**************Low pass Digital filter**************
uint32_t Size;      // Size-point average, Size=1 to 512
uint32_t x[1024];   // two copies of MACQ
uint32_t *Pt;       // pointer to current
uint32_t I1;        // index to oldest
uint32_t LPFSum;    // sum of the last Size samples
void LPF_Init_old(uint32_t initial, uint32_t size){ int i;
  if(size>512) size=512; // max
  Size = size;
  Pt = &x[0];
  LPFSum = Size*initial; // prime MACQ with initial data
  for(i=0; i<2*Size; i++){
    x[i] = initial;
  }
}
// calculate one filter output, called at sampling rate
// Input: new ADC data   Output: filter output
// y(n) = (x(n)+x(n-1)+...+x(n-Size-1)/Size
uint32_t LPF_Calc_old(uint32_t newdata){
  if(Pt == &x[0]){
    Pt = &x[Size-1];              // wrap
  } else{
    Pt--;                         // make room for data
  }
  LPFSum = LPFSum+newdata -*Pt;   // subtract oldest, add newest
  *Pt = *(Pt+Size) = newdata;     // two copies of the new data
  return LPFSum/Size;
}

void LPF_Init(uint32_t initial, uint32_t size){ int i;
  if(size>1024) size=1024; // max
  Size = size;
  I1 = Size-1;
  LPFSum = Size*initial; // prime MACQ with initial data
  for(i=0; i<Size; i++){
    x[i] = initial;
  }
}
// calculate one filter output, called at sampling rate
// Input: new ADC data   Output: filter output
// y(n) = (x(n)+x(n-1)+...+x(n-Size-1)/Size
uint32_t LPF_Calc(uint32_t newdata){
  if(I1 == 0){
    I1 = Size-1;              // wrap
  } else{
    I1--;                     // make room for data
  }
  LPFSum = LPFSum+newdata-x[I1];   // subtract oldest, add newest
  x[I1] = newdata;     // save new data
  return LPFSum/Size;
}
// calculate noise as standard deviation, called every time buffer refills
// Input: none   Output: standard deviation
int32_t Noise(void){ int32_t sum,mean,sigma;
  if(Size<2) return 0;
  sum = 0;
  for(int i=0;i<Size;i++){
    sum = sum+x[i];
  }
  mean = sum/Size; // DC component
  sum = 0;
  for(int i=0;i<Size;i++){
    sum = sum+(x[i]-mean)*(x[i]-mean); // total energy in AC part
  }
  sigma = isqrt(sum/(Size-1));
//  snr = mean/sigma;
  return sigma;
}

uint32_t x2[1024];   // two copies of MACQ
uint32_t *Pt2;       // pointer to current
uint32_t LPFSum2;    // sum of the last Size samples
void LPF_Init2(uint32_t initial, uint32_t size){ int i;
  if(size>512) size=512; // max
  Size = size;
  Pt2 = &x2[0];
  LPFSum2 = Size*initial; // prime MACQ with initial data
  for(i=0; i<2*Size; i++){
    x2[i] = initial;
  }
}
// calculate one filter output, called at sampling rate
// Input: new ADC data   Output: filter output, DAC data
// y(n) = (x(n)+x(n-1)+...+x(n-Size-1)/Size
uint32_t LPF_Calc2(uint32_t newdata){
  if(Pt2 == &x2[0]){
    Pt2 = &x2[Size-1];              // wrap
  } else{
    Pt2--;                         // make room for data
  }
  LPFSum2 = LPFSum2+newdata -*Pt2;   // subtract oldest, add newest
  *Pt2 = *(Pt2+Size) = newdata;     // two copies of the new data
  return LPFSum2/Size;
}
// calculate noise as standard deviation, called every time buffer refills
// Input: none   Output: standard deviation
int32_t Noise2(void){ int32_t sum,mean,sigma2;
  if(Size<2) return 0;
  sum = 0;
  for(int i=0;i<Size;i++){
    sum = sum+x2[i];
  }
  mean = sum/Size; // DC component
  sum = 0;
  for(int i=0;i<Size;i++){
    sum = sum+(x2[i]-mean)*(x2[i]-mean); // total energy in AC part
  }
  sigma2 = isqrt(sum/(Size-1));
//  snr2 = mean/sigma2;
  return sigma2;
}

uint32_t x3[1024];   // two copies of MACQ
uint32_t *Pt3;       // pointer to current
uint32_t LPFSum3;    // sum of the last Size samples
void LPF_Init3(uint32_t initial, uint32_t size){ int i;
  if(size>512) size=512; // max
  Size = size;
  Pt3 = &x3[0];
  LPFSum3 = Size*initial; // prime MACQ with initial data
  for(i=0; i<2*Size; i++){
    x3[i] = initial;
  }
}
// calculate one filter output, called at sampling rate
// Input: new ADC data   Output: filter output, DAC data
// y(n) = (x(n)+x(n-1)+...+x(n-Size-1)/Size
uint32_t LPF_Calc3(uint32_t newdata){
  if(Pt3 == &x3[0]){
    Pt3 = &x3[Size-1];              // wrap
  } else{
    Pt3--;                         // make room for data
  }
  LPFSum3 = LPFSum3+newdata -*Pt3;   // subtract oldest, add newest
  *Pt3 = *(Pt3+Size) = newdata;     // two copies of the new data
  return LPFSum3/Size;
}
// calculate noise as standard deviation, called every time buffer refills
// Input: none   Output: standard deviation
int32_t Noise3(void){ int32_t sum,mean,sigma3;
  if(Size<2) return 0;
  sum = 0;
  for(int i=0;i<Size;i++){
    sum = sum+x3[i];
  }
  mean = sum/Size; // DC component
  sum = 0;
  for(int i=0;i<Size;i++){
    sum = sum+(x3[i]-mean)*(x3[i]-mean); // total energy in AC part
  }
  sigma3 = isqrt(sum/(Size-1));
//  snr3 = mean/sigma3;
  return sigma3;
}

int32_t u1,u2,u3;   // last three points
int32_t Median(int32_t newdata){
  u3 = u2;
  u2 = u1;
  u1 = newdata;
  int32_t result;
  if(u1>u2)
    if(u2>u3)     result = u2;   // u1>u2,u2>u3       u1>u2>u3
      else
        if(u1>u3) result = u3;   // u1>u2,u3>u2,u1>u3 u1>u3>u2
        else      result = u1;   // u1>u2,u3>u2,u3>u1 u3>u1>u2
  else
    if(u3>u2)     result = u2;   // u2>u1,u3>u2       u3>u2>u1
      else
        if(u1>u3) result = u1;   // u2>u1,u2>u3,u1>u3 u2>u1>u3
        else      result = u3;   // u2>u1,u2>u3,u3>u1 u2>u3>u1
  return(result);
}

