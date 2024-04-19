/**
 * @file Barcode_Scanner.h
 * @brief Header file for the Barcode_Scanner driver.
 *
 * This file contains the function definitions for the Barcode_Scanner driver.
 * The EUSCI_A2 module is used to support UART communication with the Barcode Scanner module.
 *
 * @note Assumes that the necessary pin configurations for UART communication have been performed
 *       on the corresponding pins. P3.2 is used for UART RX while P3.3 is used for UART TX.
 *
 * @note For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 * 
 * @note For more information regarding the Barcode Scanner module, refer to Mouser's product page.
 * Link: https://www.mouser.com/new/sparkfun/sparkfun-2d-barcode-scanner-board/
 *
 * @note Derived from Tymofii and Eduardo's final project (Group 11) in ECE 595R/L during Fall 2023.
 * The original project used the following components:
 *  - Barcode Scanner (UART)
 *  - Piezo Buzzer (GPIO)
 *  - Nokia5110 LCD (SPI)
 *  - DC Motors (GPIO + PWM)
 *
 * @author Aaron Nanas
 *
 */

#ifndef INC_BARCODE_SCANNER_H_
#define INC_BARCODE_SCANNER_H_

#include <stdint.h>
#include <string.h>
#include "msp.h"

#define BARCODE_SCANNER_BUFFER_SIZE 64

/**
 * @brief Carriage return character
 */
#define CR   0x0D
/**
 * @brief Line feed character
 */
#define LF   0x0A
/**
 * @brief Back space character
 */
#define BS   0x08
/**
 * @brief escape character
 */
#define ESC  0x1B
/**
 * @brief space character
 */
#define SP   0x20
/**
 * @brief delete character
 */
#define DEL  0x7F

/**
 * @brief The Barcode_Scanner_Init function initializes the EUSCI_A2 module to use UART mode.
 *
 * This function configures the EUSCI_A2 module to enable UART mode
 * with the following configuration:
 *
 * - Parity: Disabled
 * - Bit Order: Least Significant Bit (LSB) first
 * - Character Length: 8 data bits
 * - Stop Bits: 1
 * - Mode: UART
 * - UART Clock Source: SMCLK
 * - Baud Rate: 115200
 *
 * For more information regarding the registers used, refer to the eUSCI_A UART Registers section (24.4)
 * of the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @note This function assumes that the necessary pin configurations for UART communication have been performed
 *       on the corresponding pins. P3.2 is used for UART RX while P3.3 is used for UART TX.
 *
 * @return None
 */
void Barcode_Scanner_Init();

/**
 * @brief Receives a single character over UART using the EUSCI_A2 module.
 *
 * This function receives a single character over UART using the EUSCI_A2 module.
 * It waits until a character is available in the UART receive buffer and then reads
 * the received data. A character consists of the following bits:
 *
 * - 1 Start Bit
 * - 8 Data Bits
 * - 1 Stop Bit
 *
 * @param None
 *
 * @return The received unsigned 8-bit data from UART.
 */
uint8_t Barcode_Scanner_InChar();

/**
 * @brief Transmits a single character over UART using the EUSCI_A2 module.
 *
 * This function transmits a single character over UART using the EUSCI_A2 module.
 * It waits until the UART transmit buffer is ready to accept new data and then writes the provided data
 * to the transmit buffer for transmission. A character consists of the following bits:
 *
 * - 1 Start Bit
 * - 8 Data Bits
 * - 1 Stop Bit
 *
 * @param data The unsigned 8-bit data to be transmitted over UART.
 *
 * @return None
 */
void Barcode_Scanner_OutChar(uint8_t data);

/**
 * @brief The Barcode_Scanner_Read function reads a set of characters from the Barcode Scanner module.
 *
 * This function reads a set of characters from the Barcode Scanner module by reading the
 * the UART Receive Buffer (UCAxRXBUF) until a carriage return (CR) character is encountered.
 * The characters are stored in the provided buffer (buffer_pointer) up to the specified maximum length (buffer_size).
 * The function supports backspace (BS) character for deleting characters from the buffer.
 *
 * @param buffer_pointer Pointer to the buffer where the received characters will be stored.
 * @param buffer_size Maximum length of the buffer.
 *
 * @return Returns the total number of characters received from the Barcode Scanner module.
 */
int Barcode_Scanner_Read(char *buffer_pointer, uint16_t buffer_size);

/**
 * @brief The Check_Barcode_Scanner_Command checks if the specified string can be found in the given buffer.
 *
 * This function reads the contents of Barcode_Scanner_Buffer and checks if the specified string
 * is in the buffer using the strstr function from the <string.h> library.
 *
 * @param char Barcode_Scanner_Buffer[] A char array that contains the received string from the Barcode Scanner module.
 * @param char *command_string A pointer to the string that will be searched in the Barcode_Scanner_Buffer array.
 *
 * @return Returns 0x01 if the string is in the buffer. Otherwise, returns 0x00.
 */
uint8_t Check_Barcode_Scanner_Command(char Barcode_Scanner_Buffer[], char *command_string);

#endif /* INC_BARCODE_SCANNER_H_ */
