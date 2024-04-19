/**
 * @file Barcode_Scanner.c
 * @brief Source code for the Barcode_Scanner driver.
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

#include "../inc/Barcode_Scanner.h"

void Barcode_Scanner_Init()
{
    // Configure pins P3.2 (PM_UCA2RXD) and P3.3 (PM_UCA2TXD) to use the primary module function
    // by setting Bits 3 and 2 in the SEL0 register for P3
    // and clearing Bits 3 and 2 in the SEL1 register for P3
    P3->SEL0 |= 0x0C;
    P3->SEL1 &= ~0x0C;

    // Hold the EUSCI_A2 module in the reset state by setting the
    // UCSWRST bit (Bit 0) in the CTLW0 register
    EUSCI_A2->CTLW0 |= 0x01;

    // Clear all of the bits in the Modulation Control Word (MCTLW) register
    EUSCI_A2->MCTLW &= ~0xFF;

    // Disable the parity bit by clearing the UCPEN bit (Bit 15) in the CTLW0 register
    EUSCI_A2->CTLW0 &= ~0x8000;

    // Select odd parity for the parity bit by clearing the UCPAR bit (Bit 14) in the CTLW0 register
    // Note that the UCPAR bit is not used when parity is disabled
    EUSCI_A2->CTLW0 &= ~0x4000;

    // Set the bit order to Least Significant Bit (LSB) first by clearing the UCMSB bit (Bit 13) in the CTLW0 register
    EUSCI_A2->CTLW0 &= ~0x2000;

    // Select 8-bit character length by clearing the UC7BIT bit (Bit 12) in the CTLW0 register
    EUSCI_A2->CTLW0 &= ~0x1000;

    // Select one stop bit by clearing the UCSPB bit (Bit 11) in the CTLW0 register
    EUSCI_A2->CTLW0 &= ~0x0800;

    // Enable UART mode by writing 00b to the UCMODEx field (Bits 10-9) in the CTLW0 register
    EUSCI_A2->CTLW0 &= ~0x0600;

    // Disable synchronous mode by clearing the UCSYNC bit (Bit 8) in the CTLW0 register
    EUSCI_A2->CTLW0 &= ~0x0100;

    // Configure the EUSCI_A2 module to use SMCLK as the clock source by
    // writing a value of 10b to the UCSSELx field (Bits 7 to 6) in the CTLW0 register
    EUSCI_A2->CTLW0 |= 0x00C0;

    // Set the baud rate value by writing to the UCBRx field (Bits 15 to 0)
    // in the BRW register
    // N = (Clock Frequency) / (Baud Rate) = (12,000,000 / 115200) = 104.16666
    // Use only the integer part, so N = 104
    EUSCI_A2->BRW = 104;

    // Disable the following interrupts by clearing the
    // corresponding bits in the IE register:
    // - Transmit Complete Interrupt (UCTXCPTIE, Bit 3)
    // - Start Bit Interrupt (UCSTTIE, Bit 2)
    EUSCI_A2->IE &= ~0x0C;

    // Enable the following interrupts by setting the
    // corresponding bits in the IE register
    // - Transmit Interrupt (UCTXIE, Bit 1)
    // - Receive Interrupt (UCRXIE, Bit 0)
    EUSCI_A2->IE |= 0x03;

    // Release the EUSCI_A2 module from the reset state by clearing the
    // UCSWRST bit (Bit 0) in the CTLW0 register
    EUSCI_A2->CTLW0 &= ~0x01;
}

uint8_t Barcode_Scanner_InChar()
{
    // Check the Receive Interrupt flag (UCRXIFG, Bit 0)
    // in the IFG register and wait if the flag is not set
    // If the UCRXIFG is set, then the Receive Buffer (UCAxRXBUF) has
    // received a complete character
    while((EUSCI_A2->IFG & 0x01) == 0);

    // Return the data from the Receive Buffer (UCAxRXBUF)
    // Reading the UCAxRXBUF will reset the UCRXIFG flag
    return EUSCI_A2->RXBUF;
}

void Barcode_Scanner_OutChar(uint8_t data)
{
    // Check the Transmit Interrupt flag (UCTXIFG, Bit 1)
    // in the IFG register and wait if the flag is not set
    // If the UCTXIFG is set, then the Transmit Buffer (UCAxTXBUF) is empty
    while((EUSCI_A2->IFG & 0x02) == 0);

    // Write the data to the Transmit Buffer (UCAxTXBUF)
    // Writing to the UCAxTXBUF will clear the UCTXIFG flag
    EUSCI_A2->TXBUF = data;
}

int Barcode_Scanner_Read(char *buffer_pointer, uint16_t buffer_size)
{
    int length = 0;
    int string_size = 0;

    // Read the last received data from the UART Receive Buffer
    char character = Barcode_Scanner_InChar();

    // Check if the received character is a carriage return. Otherwise,
    // if it is a valid character, then store it in Barcode_Scanner_Buffer.
    // For each valid character, increment the string_size variable which will
    // indicate how many characters have been detected from the Barcode Scanner module
    while(character != CR)
    {
        // Remove the character from the buffer is the received character is a backspace character
        if (character == BS)
        {
            if (length)
            {
                buffer_pointer--;
                length--;
                Barcode_Scanner_OutChar(BS);
            }
        }

        // Otherwise, if there are more characters to be read, store them in the buffer
        else if (length < buffer_size)
        {
            *buffer_pointer = character;
            buffer_pointer++;
            length++;
            string_size++;
        }
        character = Barcode_Scanner_InChar();
    }

    *buffer_pointer = 0;

    return string_size;
}

uint8_t Check_Barcode_Scanner_Command(char Barcode_Scanner_Buffer[], char *command_string)
{
    if (strstr(Barcode_Scanner_Buffer, command_string) != NULL)
    {
        return 0x01;
    }
    else
    {
        return 0x00;
    }
}
