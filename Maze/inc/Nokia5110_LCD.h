/**
 * @file Nokia5110_LCD.h
 * @brief Header file for the Nokia5110_LCD driver.
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

#ifndef INC_NOKIA5110_LCD_H_
#define INC_NOKIA5110_LCD_H_

#include <stdio.h>
#include <stdint.h>
#include "msp.h"
#include "Clock.h"

/**
 * @brief The SCREENW constant defines the width of the screen in pixels as 84.
 *
 * @note This constant represents the horizontal dimension of the screen.
 */
#define SCREENW     84

/**
 * @brief The SCREENH constant defines the height of the screen in pixels as 48.
 *
 * @note This constant represents the vertical dimension of the screen.
 */
#define SCREENH     48

/**
 * @brief The DC_BIT constant represents the bit position of the Data/Command (DC) pin.
 *
 * The DC_BIT constant defines the bit position of the Data/Command (DC) pin as 0x40.
 * This pin is used to distinguish between data and command transmissions in the SPI communication.
 *
 * @note This constant is used to set or clear the DC bit in the output register.
 */
#define DC_BIT      0x40

/**
 * @brief The RESET_BIT constant represents the bit position of the Reset (RST) pin.
 *
 * The RESET_BIT constant defines the bit position of the Reset (RST) pin as 0x08.
 * This pin is used to reset the Nokia 5110 LCD display.
 *
 * @note This constant is used to set or clear the Reset bit in the output register.
 */
#define RESET_BIT   0x08

/**
 * @brief Maximum columns of the LCD, although the pixels are
 * numbered from zero to (MAX-1).  Address may automatically
 * be incremented after each transmission.
 */
#define MAX_X       84

/**
 * @brief Maximum rows of the LCD, although the pixels are
 * numbered from zero to (MAX-1).  In the default horizontal
 * addressing mode (V = 0), rows are clustered into groups of
 * eight, which are all sent together with one 8-bit SSI data
 * transmission.
 */
#define MAX_Y       48

/**
 * @brief The CONTRAST constant represents the contrast value for the Nokia 5110 LCD display.
 *
 * The CONTRAST constant is set to 0xB1, which is the recommended value for the red SparkFun Nokia 5110 LCD display.
 * For the blue Nokia 5110 LCD display, the recommended contrast value is 0xB8.
 * You can adjust this value within the range of 0xA0 (lighter) to 0xCF (darker) to achieve the desired contrast for your specific display.
 *
 * @note This constant is used to set the contrast level of the LCD display.
 */
#define CONTRAST   0xBF //used to be 0xA5 - Zelgehai

/**
 * @brief The ASCII table contains the hexadecimal values that represent
 * pixels for a font that is 5 pixels wide and 8 pixels high
 */
//extern const uint8_t ASCII[][5];

/**
 * @brief Initializes the SPI module EUSCI_A3 for the Nokia 5110 LCD.
 *
 * This function configures the EUSCI_A3 module to enable SPI communication
 * for the Nokia 5110 LCD with the following configuration:
 *
 * - CTLW0 Register Configuration:
 *
 *  Bit(s)      Field       Value       Description
 *  -----       -----       -----       -----------
 *   15         UCCKPH       0x1        Data is captured on the first edge and changed on the following edge
 *   14         UCCKPL       0x0        Clock is low when inactive
 *   13         UCMSB        0x1        MSB first
 *   12         UC7BIT       0x0        8-bit data
 *   11         UCMST        0x1        Master mode is selected
 *   10-9       UCMODEx      0x2        4-pin SPI with active low UCSTE
 *   8          UCSYNC       0x1        Synchronous mode
 *   7-6        UCSSELx      0x2        eUSCI clock source is SMCLK
 *   5-2        Reserved     0x0        Reserved
 *   1          UCSTEM       0x1        UCSTE pin is used to generate signal for 4-wire slave
 *   0          UCSWRST      0x1        eUSCI logic held in reset state
 *
 * - Clock Frequency: 4 MHz (refer to Page 20 of Nokia 5110 LCD datasheet)
 * - Interrupts disabled
 *
 * For more information regarding the registers used, refer to the eUSCI_A SPI Registers section (25.4)
 * of the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @note This function assumes that the necessary pin configurations for SPI communication have been performed
 *       on the corresponding pins. The Nokia 5110 LCD does not have a Master In Slave Out (MISO) line.
 *
 * @return None
 */
void Nokia5110_SPI_Init();

/**
 * @brief The Nokia5110_SPI_Data_Command_Bit_Out function sets the data/command select bit for the Nokia5110 LCD.
 *
 * This function sets the data/command select bit based on the provided value (data_command_select) for the Nokia5110 LCD.
 * If the value is 0, the data/command select bit is cleared (DC_BIT is set to low).
 * If the value is non-zero, the data/command select bit is set (DC_BIT is set to high).
 *
 * @param data_command_select The value indicating the data/command select bit: 0 for command, non-zero for data.
 *
 * @return None
 */
void Nokia5110_SPI_Data_Command_Bit_Out(uint8_t data_command_select);

/**
 * @brief The Nokia5110_SPI_Reset_Bit_Out function sets the reset bit for the Nokia5110 LCD.
 *
 * This function sets the reset bit based on the provided value (reset_value) for the Nokia5110 LCD.
 * If the value is 0, the reset bit is cleared (RESET_BIT is set to low).
 * If the value is non-zero, the reset bit is set (RESET_BIT is set to high).
 *
 * @param reset_value The value indicating the reset bit: 0 for reset, non-zero for normal operation.
 *
 * @return None
 */
void Nokia5110_SPI_Reset_Bit_Out(uint8_t reset_value);

/**
 * @brief The Nokia5110_Reset function performs an active low reset for the Nokia5110 LCD.
 *
 * This function initiates an active low reset sequence for the Nokia5110 LCD.
 * It sets the reset bit to low, delays for 1000 ns, and then sets the reset bit back to high.
 *
 * @param None
 *
 * @return None
 */
void Nokia5110_Reset();

/**
 * @brief The Nokia5110_Set_Contrast functions modifies the contrast of the Nokia5110 LCD.
 *
 * This function modifies the contrast of the Nokia5110 LCD by sending the following
 * command bytes: 0x21, contrast value, and 0x20.
 *
 * @param contrast_value
 *
 * @return None
 */
void Nokia5110_Set_Contrast(uint8_t contrast_value);

/**
 * @brief The Nokia5110_Config function configures the Nokia5110 LCD.
 *
 * This function configures the Nokia5110 LCD by sending a series of commands to initialize various settings.
 * It sets the addressing mode, instruction set, contrast, temperature coefficient, LCD bias mode, and display control mode.
 *
 * @param None
 *
 * @return None
 */
void Nokia5110_Config();

/**
 * @brief The Nokia5110_Init function initializes the Nokia5110 LCD.
 *
 * This function initializes the Nokia5110 LCD by performing the SPI initialization, reset, and configuration.
 * It calls the respective functions for SPI initialization, reset, and configuration.
 *
 * @param None
 *
 * @return None
 */
void Nokia5110_Init();

/**
 * @brief The Nokia5110_Command_Write function writes a command to the Nokia5110 LCD.
 *
 * This function writes a command byte to the Nokia5110 LCD by sending it via the SPI interface.
 * It waits for the SPI to become idle, sets the data/command select bit to indicate a command byte,
 * writes the command byte to the transmit buffer, and waits for the SPI to complete the transmission.
 *
 * @param command The command byte to be written.
 *
 * @return None
 */
void Nokia5110_Command_Write(uint8_t command);

/**
 * @brief The Nokia5110_Data_Write function writes data to the Nokia5110 LCD.
 *
 * This function writes a data byte to the Nokia5110 LCD by sending it via the SPI interface.
 * It waits for the transmit buffer to become empty, sets the data/command select bit to indicate a data byte,
 * writes the data byte to the transmit buffer, and waits for the SPI to complete the transmission.
 *
 * @param data The data byte to be written.
 *
 * @return None
 */
void Nokia5110_Data_Write(uint8_t data);

/**
 * @brief The Nokia5110_OutChar function prints a character to the Nokia 5110 48x84 LCD.
 *
 * This function prints a character (data) to the current cursor position on the Nokia 5110 LCD.
 * The cursor position is automatically updated, and the character will wrap to the next row or back to the top if necessary.
 * One blank column of pixels is printed on either side of the character for readability.
 * Since characters are 8 pixels tall and 5 pixels wide, 12 characters fit per row, and there are six rows.
 *
 * @param data The character to print.
 *
 * @return None
 *
 * @note It takes approximately 14 microseconds to output a character.
 * @note Assumes the LCD is in the default horizontal addressing mode (V = 0).
 */
void Nokia5110_OutChar(char data);

/**
 * @brief The Nokia5110_OutString function prints a string of characters to the Nokia 5110 48x84 LCD.
 *
 * This function prints a string of characters (ptr) to the Nokia 5110 LCD.
 * The string will automatically wrap to the next row when reaching the right edge of the display,
 * so padding spaces may be needed to make the output look optimal.
 *
 * @param ptr Pointer to a NULL-terminated ASCII string.
 *
 * @return None
 *
 * @note Assumes the LCD is in the default horizontal addressing mode (V = 0).
 */
void Nokia5110_OutString(char *ptr);

/**
 * @brief The Nokia5110_OutUDec function outputs a 16-bit number in unsigned decimal format.
 *
 * This function outputs a 16-bit unsigned number (n) in decimal format with a fixed size of five right-justified digits of output.
 * The output is displayed on the Nokia 5110 LCD.
 *
 * @param n The 16-bit unsigned number to be displayed.
 *
 * @return None
 *
 * @note Assumes the LCD is in the default horizontal addressing mode (V = 0).
 */
void Nokia5110_OutUDec(uint16_t n);

/**
 * @brief The Nokia5110_OutSDec function outputs a 16-bit number in signed decimal format.
 *
 * This function outputs a 16-bit signed number (n) in decimal format with a fixed size of six right-justified digits of output.
 * The output is displayed on the Nokia 5110 LCD.
 *
 * @param n The 16-bit signed number to be displayed.
 *
 * @return None
 *
 * @note Assumes the LCD is in the default horizontal addressing mode (V = 0).
 */
void Nokia5110_OutSDec(int16_t n);

/**
 * @brief The Nokia5110_OutUFix1 function outputs a 16-bit number in unsigned 3-digit fixed point format with 0.1 resolution.
 *
 * This function outputs a 16-bit unsigned number (n) in fixed-point format with 3 digits and 0.1 resolution.
 * The numbers 0 to 999 are printed as " 0.0" to "99.9" on the Nokia 5110 LCD.
 *
 * @param n The 16-bit unsigned number to be displayed.
 *
 * @return None
 */
void Nokia5110_OutUFix1(uint16_t n);

/**
 * @brief The Nokia5110_OutSFix1 function outputs a 32-bit number in signed 4-digit fixed point format with 0.1 resolution.
 *
 * This function outputs a 32-bit signed number (n) in fixed-point format with 4 digits and 0.1 resolution.
 * The numbers -9999 to 9999 are printed as "-999.0" to " 999.9" on the Nokia 5110 LCD.
 *
 * @param n The 32-bit signed number to be displayed.
 *
 * @return None
 */
void Nokia5110_OutSFix1(int32_t n);

/**
 * @brief The Nokia5110_OutHex7 function outputs a 8-bit number in hexadecimal format with 7 bits.
 *
 * This function outputs a 8-bit number (n) in hexadecimal format with 7 bits.
 * The output is displayed on the Nokia 5110 LCD.
 *
 * @param n The 8-bit number to be displayed.
 *
 * @return None
 */
void Nokia5110_OutHex7(uint8_t n);

/**
 * @brief The Nokia5110_OutUHex7 function outputs a 8-bit number in unsigned hexadecimal format with 7 bits.
 *
 * This function outputs a 8-bit number (n) in unsigned hexadecimal format with 7 bits.
 * The output is displayed on the Nokia 5110 LCD.
 *
 * @param n The 8-bit number to be displayed.
 *
 * @return None
 */
void Nokia5110_OutUHex7(uint8_t n);

/**
 * @brief The Nokia5110_OutUDec16 function outputs a 32-bit number in unsigned decimal format with a fixed size of three digits.
 *
 * This function outputs a 32-bit unsigned number (n) in decimal format with a fixed size of three digits.
 * The output is displayed on the Nokia 5110 LCD.
 *
 * @param n The 32-bit unsigned number to be displayed.
 *
 * @return None
 */
void Nokia5110_OutUDec16(uint32_t n);

/**
 * @brief The Nokia5110_OutUDec2 function outputs a 32-bit number in unsigned decimal format with a fixed size of two digits.
 *
 * This function outputs a 32-bit unsigned number (n) in decimal format with a fixed size of two digits.
 * The output is displayed on the Nokia 5110 LCD.
 *
 * @param n The 32-bit unsigned number to be displayed.
 *
 * @return None
 */
void Nokia5110_OutUDec2(uint32_t n);

/**
 * @brief The Nokia5110_SetCursor function moves the cursor to the desired X and Y positions.
 *
 * This function moves the cursor to the desired X and Y positions on the Nokia 5110 LCD.
 * The next character will be printed at the specified position.
 * X=0 represents the leftmost column, and Y=0 represents the top row.
 *
 * @param newX The new X-position of the cursor (0 <= newX <= 11).
 * @param newY The new Y-position of the cursor (0 <= newY <= 5).
 *
 * @return None
 */
void Nokia5110_SetCursor(uint8_t newX, uint8_t newY);

/**
 * @brief The Nokia5110_Clear function clears the LCD by writing zeros to the entire screen.
 *
 * This function clears the LCD by writing zeros to all pixels on the screen.
 * It also resets the cursor position to (0,0), which is the top-left corner of the screen.
 *
 * @param None
 *
 * @return None
 */
void Nokia5110_Clear();

/**
 * @brief The Nokia5110_DrawFullImage function fills the whole screen by drawing a 48x84 bitmap image.
 *
 * This function fills the entire screen of the Nokia 5110 LCD by drawing a bitmap image.
 * The image data is specified by the pointer to a 504-byte bitmap (ptr).
 * The LCD is assumed to be in the default horizontal addressing mode (V = 0).
 *
 * @param ptr Pointer to a 504-byte bitmap image.
 *
 * @return None
 *
 * @note Assumes the LCD is in the default horizontal addressing mode (V = 0).
 */
void Nokia5110_DrawFullImage(const uint8_t *ptr);

/**
 * @brief The Nokia5110_PrintBMP function takes a bitmap image and puts its image data in the buffer for display.
 *
 * Bitmaps defined for the LM3S1968 or LM3S8962's 4-bit grayscale OLED display may contain header data and padding.
 * This function extracts the image data from the bitmap and places it in the buffer so that it appears on the screen
 * after the next call to Nokia5110_DisplayBuffer().
 *
 * The interface and operation of this process are modeled after RIT128x96x4_BMP(x, y, image).
 *
 * @param xpos      The horizontal position of the bottom-left corner of the image, columns from the left edge.
 *                  Must be less than 84. 0 is on the left; 82 is near the right.
 * @param ypos      The vertical position of the bottom-left corner of the image, rows from the top edge.
 *                  Must be less than 48. 2 is near the top; 47 is at the bottom.
 * @param ptr       Pointer to a 16-color BMP image.
 * @param threshold Grayscale colors above this number make the corresponding pixel 'on'.
 *                  Valid range: 0 to 14. 0 is fine for ships, explosions, projectiles, and bunkers.
 *
 * @return None
 */
void Nokia5110_PrintBMP(uint8_t xpos, uint8_t ypos, const uint8_t *ptr, uint8_t threshold);

/**
 * @brief The Nokia5110_ClearBuffer function clears the buffer in RAM that holds one screen.
 *
 * This function clears the buffer in RAM that holds the screen data.
 *
 * @param None
 *
 * @return None
 */
void Nokia5110_ClearBuffer();

/**
 * @brief The Nokia5110_DisplayBuffer function fills the whole screen by drawing a 48x84 screen image from the RAM buffer.
 *
 * This function fills the entire screen of the Nokia 5110 LCD by drawing a 48x84 screen image from the RAM buffer.
 * The LCD is assumed to be in the default horizontal addressing mode (V = 0).
 *
 * @param None
 *
 * @return None
 *
 * @note Assumes the LCD is in the default horizontal addressing mode (V = 0).
 */
void Nokia5110_DisplayBuffer();

/**
 * @brief The Nokia5110_ClrPxl function clears the internal screen buffer pixel at position (i, j), turning it off.
 *
 * This function clears the pixel at the specified position (i, j) in the internal screen buffer.
 * The pixel is turned off.
 *
 * @param i The row index of the pixel (0 to 47).
 * @param j The column index of the pixel (0 to 83).
 *
 * @return None
 */
void Nokia5110_ClrPxl(uint32_t i, uint32_t j);

/**
 * @brief The Nokia5110_SetPxl function sets the internal screen buffer pixel at position (i, j), turning it on.
 *
 * This function sets the pixel at the specified position (i, j) in the internal screen buffer.
 * The pixel is turned on.
 *
 * @param i The row index of the pixel (0 to 47).
 * @param j The column index of the pixel (0 to 83).
 *
 * @return None
 */
void Nokia5110_SetPxl(uint32_t i, uint32_t j);

#endif /* INC_NOKIA5110_LCD_H_ */
