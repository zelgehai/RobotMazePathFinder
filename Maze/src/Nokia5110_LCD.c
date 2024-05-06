/**
 * @file Nokia5110_LCD.c
 * @brief Source code for the Nokia5110_LCD driver.
 *
 * This file contains the function definitions for the Nokia5110_LCD driver.
 * It interfaces with the SparkFun Nokia 5110 LCD (LCD-10168).
 *  - Product Link: https://www.sparkfun.com/products/10168
 *  - Datasheet: http://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf
 *
 * The following connections must be made:
 *  - Nokia 5110 LCD VCC    <-->  MSP432 LaunchPad VCC (3.3V)
 *  - Nokia 5110 LCD GND    <-->  MSP432 LaunchPad GND
 *  - Nokia 5110 LCD SCE    <-->  MSP432 LaunchPad Pin P9.4 (SCE, Chip Enable)
 *  - Nokia 5110 LCD RST    <-->  MSP432 LaunchPad Pin P9.3 (Reset)
 *  - Nokia 5110 LCD D/C    <-->  MSP432 LaunchPad Pin P9.6 (D/C, Data/Command)
 *  - Nokia 5110 LCD MOSI   <-->  MSP432 LaunchPad Pin P9.7 (MOSI)
 *  - Nokia 5110 LCD SCLK   <-->  MSP432 LaunchPad Pin P9.5 (SCLK)
 *  - Nokia 5110 LCD LED    <-->  Unconnected
 *
 * @note This function assumes that the necessary pin configurations for SPI communication have been performed
 *       on the corresponding pins. The Nokia 5110 LCD does not have a Master In Slave Out (MISO) line.
 *       The output from the following pins will be observed using an oscilloscope:
 *       - P9.4 (SCE, Chip Enable)
 *       - P9.5 (SCLK)
 *       - P9.7 (MOSI, Master Out Slave In)
 *
 * For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @author Aaron Nanas
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "msp.h"
#include "../inc/Nokia5110_LCD.h"

const uint8_t ASCII[][5] = {
   {0x00, 0x00, 0x00, 0x00, 0x00} // 20
  ,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
  ,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
  ,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
  ,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
  ,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
  ,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
  ,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
  ,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
  ,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
  ,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
  ,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
  ,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
  ,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
  ,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
  ,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
  ,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
  ,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
  ,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
  ,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
  ,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
  ,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
  ,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
  ,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
  ,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
  ,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
  ,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
  ,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
  ,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
  ,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
  ,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
  ,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
  ,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
  ,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
  ,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
  ,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
  ,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
  ,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
  ,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
  ,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
  ,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
  ,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
  ,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
  ,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
  ,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
  ,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
  ,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
  ,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
  ,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
  ,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
  ,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
  ,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
  ,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
  ,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
  ,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
  ,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
  ,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
  ,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
  ,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
  ,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
  ,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c '\'
  ,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
  ,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
  ,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
  ,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
  ,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
  ,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
  ,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
  ,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
  ,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
  ,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
  ,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
  ,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
  ,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
  ,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
  ,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
  ,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
  ,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
  ,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
  ,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
  ,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
  ,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
  ,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
  ,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
  ,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
  ,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
  ,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
  ,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
  ,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
  ,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
  ,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
  ,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
  ,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
  ,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
  ,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ~
//  ,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f DEL
  ,{0x1f, 0x24, 0x7c, 0x24, 0x1f} // 7f UT sign
};

void Nokia5110_SPI_Init()
{
    // Hold the EUSCI_A3 module in reset mode
    EUSCI_A3->CTLW0 |= 0x01;

// CTWL0 Register Configuration
//
//  Bit(s)      Field       Value       Description
//  -----       -----       -----       -----------
//   15         UCCKPH       0x1        Data is captured on the first edge and changed on the following edge
//   14         UCCKPL       0x0        Clock is low when inactive
//   13         UCMSB        0x1        MSB first
//   12         UC7BIT       0x0        8-bit data
//   11         UCMST        0x1        Master mode is selected
//   10-9       UCMODEx      0x2        4-pin SPI with active low UCSTE
//   8          UCSYNC       0x1        Synchronous mode
//   7-6        UCSSELx      0x2        eUSCI clock source is SMCLK
//   5-2        Reserved     0x0        Reserved
//   1          UCSTEM       0x1        UCSTE pin is used to generate signal for 4-wire slave
//   0          UCSWRST      0x1        eUSCI logic held in reset state
    EUSCI_A3->CTLW0 |= 0xAD83;

    // Set the baud rate. The clock frequency used is 1 MHz.
    // N = (Clock Frequency) / (Baud Rate) = (12,000,000 / 1,000,000)
    // N = 3
    EUSCI_A3->BRW |= 12;

    // Configure P9.4, P9.5, and P9.7 pins as primary module function
    P9->SEL0 |= 0xB0;
    P9->SEL1 &= ~0xB0;

    // Configure P9.3 (Reset) and P9.6 (Data/Command) pins as GPIO pins
    P9->SEL0 &= ~(RESET_BIT | DC_BIT);
    P9->SEL1 &= ~(RESET_BIT | DC_BIT);

    // Set the direction of the P9.3 and P9.6 as output
    P9->DIR |= (RESET_BIT | DC_BIT);

    // Clear the software reset bit to enable the EUSCI_A3 module
    EUSCI_A3->CTLW0 &= ~0x01;

    // Ensure that the following interrupts are disabled:
    // - Receive Interrupt
    // - Transmit Interrupt
    EUSCI_A3->IE &= ~0x03;
}

void Nokia5110_SPI_Data_Command_Bit_Out(uint8_t data_command_select)
{
    if (data_command_select == 0)
    {
        P9->OUT &= ~DC_BIT;
    }
    else
    {
        P9->OUT |= DC_BIT;
    }
}

void Nokia5110_SPI_Reset_Bit_Out(uint8_t reset_value)
{
    if (reset_value == 0)
    {
        P9->OUT &= ~RESET_BIT;
    }
    else
    {
        P9->OUT |= RESET_BIT;
    }
}

// Active Low Reset
void Nokia5110_Reset()
{
    Nokia5110_SPI_Reset_Bit_Out(0);
    Clock_Delay1us(1);
    Nokia5110_SPI_Reset_Bit_Out(1);
}

void Nokia5110_Set_Contrast(uint8_t contrast_value)
{
    Nokia5110_Command_Write(0x21);
    Nokia5110_Command_Write(contrast_value);
    Nokia5110_Command_Write(0x20);
}

void Nokia5110_Config()
{
    // Chip active.
    // Set horizontal addressing mode (V = 0)
    // Use extended instruction set (H = 1)
    Nokia5110_Command_Write(0x21);

    // Set contrast to a default value of 0xB1
    Nokia5110_Command_Write(CONTRAST);

    // Set temperature coefficient
    Nokia5110_Command_Write(0x04);

    // LCD bias mode set to 0x14
    Nokia5110_Command_Write(0x14);

    // Software must send 0x20 before modifying the display control mode
    Nokia5110_Command_Write(0x20);

    // Set display control to normal mode. For inverse mode, send 0x0D
    Nokia5110_Command_Write(0x0C);
}

void Nokia5110_Init()
{
    Nokia5110_SPI_Init();
    Nokia5110_Reset();
    Nokia5110_Config();
}

void Nokia5110_Command_Write(uint8_t command)
{
    // UCBUSY - Wait until SPI is not busy
    while((EUSCI_A3->STATW & 0x0001) == 0x0001);

    // Set the Data/Command output pin to 0 to indicate that the transmitted byte is a command byte
    Nokia5110_SPI_Data_Command_Bit_Out(0x00);

    // Write the command byte to the transmit buffer
    EUSCI_A3->TXBUF = command;

    // UCBUSY - Wait until SPI is not busy
    while((EUSCI_A3->STATW & 0x0001) == 0x0001);
}

void Nokia5110_Data_Write(uint8_t data)
{
    // Wait until UCA3TXBUF is empty
    while((EUSCI_A3->IFG & 0x0002) == 0x0000);

    // Set the Data/Command output pin to 1 to indicate that the transmitted byte is a data byte
    Nokia5110_SPI_Data_Command_Bit_Out(0x01);

    // Write the data byte to the transmit buffer
    EUSCI_A3->TXBUF = data;
}

void Nokia5110_OutChar(char data)
{
    // Blank vertical line padding
    Nokia5110_Data_Write(0x00);
    for(int i = 0; i < 5; i = i + 1)
    {
        Nokia5110_Data_Write(ASCII[data - 0x20][i]);
    }
    // Blank vertical line padding
    Nokia5110_Data_Write(0x00);
}

void Nokia5110_OutString(char *ptr)
{
    while(*ptr)
    {
        Nokia5110_OutChar((unsigned char)*ptr);
        ptr = ptr + 1;
    }
}

void Nokia5110_OutUDec(uint16_t n)
{
    if (n < 10)
    {
        Nokia5110_OutString("    ");
        Nokia5110_OutChar(n + '0'); /* n is between 0 and 9 */
    }
    else if (n < 100)
    {
        Nokia5110_OutString("   ");
        Nokia5110_OutChar(n/10 + '0'); /* tens digit */
        Nokia5110_OutChar(n%10 + '0'); /* ones digit */
    }
    else if (n < 1000)
    {
        Nokia5110_OutString("  ");
        Nokia5110_OutChar(n/100 + '0'); /* hundreds digit */
        n = n%100;
        Nokia5110_OutChar(n/10 + '0'); /* tens digit */
        Nokia5110_OutChar(n%10 + '0'); /* ones digit */
    }
    else if (n < 10000)
    {
        Nokia5110_OutChar(' ');
        Nokia5110_OutChar(n/1000 + '0'); /* thousands digit */
        n = n%1000;
        Nokia5110_OutChar(n/100 + '0'); /* hundreds digit */
        n = n%100;
        Nokia5110_OutChar(n/10 + '0'); /* tens digit */
        Nokia5110_OutChar(n%10 + '0'); /* ones digit */
    }
    else
    {
        Nokia5110_OutChar(n/10000 + '0'); /* ten-thousands digit */
        n = n%10000;
        Nokia5110_OutChar(n/1000 + '0'); /* thousands digit */
        n = n%1000;
        Nokia5110_OutChar(n/100 + '0'); /* hundreds digit */
        n = n%100;
        Nokia5110_OutChar(n/10 + '0'); /* tens digit */
        Nokia5110_OutChar(n%10 + '0'); /* ones digit */
    }
}

void Nokia5110_OutSDec(int16_t n)
{
  char sign=' ';
  if(n<0){
    sign = '-';
    n =  -n; // positive now
  }
  if(n < 10){
    Nokia5110_OutString("    ");
    Nokia5110_OutChar(sign);
    Nokia5110_OutChar(n+'0'); /* n is between 0 and 9 */
  } else if(n<100){
    Nokia5110_OutString("   ");
    Nokia5110_OutChar(sign);
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  } else if(n<1000){
    Nokia5110_OutString("  ");
    Nokia5110_OutChar(sign);
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
  else if(n<10000){
    Nokia5110_OutChar(' ');
    Nokia5110_OutChar(sign);
    Nokia5110_OutChar(n/1000+'0'); /* thousands digit */
    n = n%1000;
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
  else {
    Nokia5110_OutChar(sign);
    Nokia5110_OutChar(n/10000+'0'); /* ten-thousands digit */
    n = n%10000;
    Nokia5110_OutChar(n/1000+'0'); /* thousands digit */
    n = n%1000;
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
}

void Nokia5110_OutUFix1(uint16_t n)
{
  char message[5];
  if(n>999)n=999;
  if(n>=100){  // 100 to 999
    message[0] = (n/100+'0'); /* tens digit */
    n = n%100; //the rest
  }else { // 0 to 99
    message[0] = ' '; /* n is between 0.0 and 9.9 */
  }
  message[1] = (n/10+'0'); /* ones digit */
  n = n%10; //the rest
  message[2] = '.';
  message[3] = (n+'0'); /* tenths digit */
  message[4] = 0;
  Nokia5110_OutString(message);
}

void Nokia5110_OutSFix1(int32_t n)
{
  char message[8];
  if(n<-9999) n=-9999;
  if(n>9999)  n=9999;
  if(n<0){
    message[0] = '-';
    n = -n; // now positive
  }else{
    message[0] = ' ';
  }
  if(n>=1000){  // 1000 to 9999
    message[1] = (n/1000+'0'); /* hundreds digit */
    n = n%1000; // the rest
    message[2] = (n/100+'0'); /* tens digit */
    n = n%100;  // the rest
  }else{
    if(n>=100){  // 100 to 999
      message[1] = ' '; /* n is between 0.0 and 9.9 */
      message[2] = (n/100+'0'); /* tens digit */
      n = n%100; //the rest
    }else { // 0 to 99
      message[1] = ' '; /* n is between 0.0 and 9.9 */
      message[2] = ' ';
    }
  }
  message[3] = (n/10+'0'); /* ones digit */
  n = n%10; //the rest
  message[4] = '.';
  message[5] = (n+'0'); /* tenths digit */
  message[6] = 0;
  Nokia5110_OutString(message);
}

void Nokia5110_OutHex7(uint8_t n)
{
    n = n&0x0F;
    if (n>9)
    {
        Nokia5110_OutChar('A'+n-10);
    }
    else
    {
        Nokia5110_OutChar(n+'0'); /* ones digit */
    }
}

void Nokia5110_OutUHex7(uint8_t n)
{
    Nokia5110_OutString(" 0x");
    Nokia5110_OutHex7(n/16); /* 16s digit */
    Nokia5110_OutHex7(n);    /* ones digit */
}

void Nokia5110_OutUDec16(uint32_t n)
{
  Nokia5110_OutChar(' ');
  if(n>=100){
    Nokia5110_OutChar(n/100+'0'); /* 100 digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* 10 digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }else{
    Nokia5110_OutChar(' ');
    if(n>=10){
      Nokia5110_OutChar(n/10+'0'); /* 10 digit */
      Nokia5110_OutChar(n%10+'0'); /* ones digit */
    }else{
      Nokia5110_OutChar(' ');
      Nokia5110_OutChar(n+'0'); /* ones digit */
    }
  }
}

void Nokia5110_OutUDec2(uint32_t n)
{
  if(n>=100){
    Nokia5110_OutChar(' ');  /* 100 digit */
    Nokia5110_OutChar('*'); /* illegal */
  }else{
    if(n>=10){
      Nokia5110_OutChar(n/10+'0'); /* 10 digit */
      Nokia5110_OutChar(n%10+'0'); /* ones digit */
    }else{
      Nokia5110_OutChar(' ');
      Nokia5110_OutChar(n+'0'); /* ones digit */
    }
  }
}

void Nokia5110_SetCursor(uint8_t newX, uint8_t newY)
{
    // Return if the input is bad
    if((newX > 11) || (newY > 5))
    {
        return;
    }
    // Multiply newX by 7 because each character is 7 columns wide

    // Setting bit 7 updates X-position
    Nokia5110_Command_Write(0x80 | (newX*7));

    // Setting bit 6 updates Y-position
    Nokia5110_Command_Write(0x40 | newY);
}

void Nokia5110_Clear()
{
    for (int i = 0; i < (MAX_X*MAX_Y/8); i = i + 1)
    {
        Nokia5110_Data_Write(0x00);
    }
    Nokia5110_SetCursor(0, 0);
}

void Nokia5110_DrawFullImage(const uint8_t *ptr)
{
    Nokia5110_SetCursor(0, 0);
    for (int i = 0; i < (MAX_X*MAX_Y/8); i = i + 1)
    {
        Nokia5110_Data_Write(ptr[i]);
    }
}
uint8_t Screen[SCREENW*SCREENH/8]; // buffer stores the next image to be printed on the screen

void Nokia5110_PrintBMP(uint8_t xpos, uint8_t ypos, const uint8_t *ptr, uint8_t threshold){
  int32_t width = ptr[18], height = ptr[22], i, j;
  uint16_t screenx, screeny;
  uint8_t mask;
  // check for clipping
  if((height <= 0) ||              // bitmap is unexpectedly encoded in top-to-bottom pixel order
     ((width%2) != 0) ||           // must be even number of columns
     ((xpos + width) > SCREENW) || // right side cut off
     (ypos < (height - 1)) ||      // top cut off
     (ypos > SCREENH))           { // bottom cut off
    return;
  }
  if(threshold > 14){
    threshold = 14;             // only full 'on' turns pixel on
  }
  // bitmaps are encoded backwards, so start at the bottom left corner of the image
  screeny = ypos/8;
  screenx = xpos + SCREENW*screeny;
  mask = ypos%8;                // row 0 to 7
  mask = 0x01<<mask;            // now stores a mask 0x01 to 0x80
  j = ptr[10];                  // byte 10 contains the offset where image data can be found
  for(i=1; i<=(width*height/2); i=i+1){
    // the left pixel is in the upper 4 bits
    if(((ptr[j]>>4)&0xF) > threshold){
      Screen[screenx] |= mask;
    } else{
      Screen[screenx] &= ~mask;
    }
    screenx = screenx + 1;
    // the right pixel is in the lower 4 bits
    if((ptr[j]&0xF) > threshold){
      Screen[screenx] |= mask;
    } else{
      Screen[screenx] &= ~mask;
    }
    screenx = screenx + 1;
    j = j + 1;
    if((i%(width/2)) == 0){     // at the end of a row
      if(mask > 0x01){
        mask = mask>>1;
      } else{
        mask = 0x80;
        screeny = screeny - 1;
      }
      screenx = xpos + SCREENW*screeny;
      // bitmaps are 32-bit word aligned
      switch((width/2)%4){      // skip any padding
        case 0: j = j + 0; break;
        case 1: j = j + 3; break;
        case 2: j = j + 2; break;
        case 3: j = j + 1; break;
      }
    }
  }
}

void Nokia5110_ClearBuffer()
{
    int i;
    for(i=0; i<SCREENW*SCREENH/8; i=i+1)
    {
        Screen[i] = 0;              // clear buffer
    }
}

void Nokia5110_DisplayBuffer()
{
  Nokia5110_DrawFullImage(Screen);
}

const unsigned char Masks[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

void Nokia5110_ClrPxl(uint32_t i, uint32_t j)
{
  Screen[84*(i>>3) + j] &= ~Masks[i&0x07];
}

void Nokia5110_SetPxl(uint32_t i, uint32_t j)
{
  Screen[84*(i>>3) + j] |= Masks[i&0x07];
}
