/**
 * @file EUSCI_A3_UART.c
 * @brief Source code for the EUSCI_A3_UART driver.
 *
 * This file contains the function definitions for the EUSCI_A3_UART driver.
 *
 * @note Assumes that the necessary pin configurations for UART communication have been performed
 *       on the corresponding pins. P9.6 is used for UART RX while P9.7 is used for UART TX.
 *
 * @note For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @author Aaron Nanas
 *
 */

#include "../inc/EUSCI_A3_UART.h"

void EUSCI_A3_UART_Init()
{
    // Configure pins P9.6 (PM_UCA3RXD) and P9.7 (PM_UCA3TXD) to use the primary module function
    // by setting Bits 7 and 6 in the SEL0 register for P9
    // and clearing Bits 7 and 6 in the SEL1 register for P9
    P9->SEL0 |= 0xC0;
    P9->SEL1 &= ~0xC0;

    // Hold the EUSCI_A3 module in the reset state by setting the
    // UCSWRST bit (Bit 0) in the CTLW0 register
    EUSCI_A3->CTLW0 |= 0x01;

    // Clear all of the bits in the Modulation Control Word (MCTLW) register
    EUSCI_A3->MCTLW &= ~0xFF;

    // Disable the parity bit by clearing the UCPEN bit (Bit 15) in the CTLW0 register
    EUSCI_A3->CTLW0 &= ~0x8000;

    // Select odd parity for the parity bit by clearing the UCPAR bit (Bit 14) in the CTLW0 register
    // Note that the UCPAR bit is not used when parity is disabled
    EUSCI_A3->CTLW0 &= ~0x4000;

    // Set the bit order to Most Significant Bit (MSB) first by setting the UCMSB bit (Bit 13) in the CTLW0 register
    EUSCI_A3->CTLW0 |= 0x2000;

    // Select 8-bit character length by clearing the UC7BIT bit (Bit 12) in the CTLW0 register
    EUSCI_A3->CTLW0 &= ~0x1000;

    // Select one stop bit by clearing the UCSPB bit (Bit 11) in the CTLW0 register
    EUSCI_A3->CTLW0 &= ~0x0800;

    // Enable UART mode by writing 00b to the UCMODEx field (Bits 10-9) in the CTLW0 register
    EUSCI_A3->CTLW0 &= ~0x0600;

    // Disable synchronous mode by clearing the UCSYNC bit (Bit 8) in the CTLW0 register
    EUSCI_A3->CTLW0 &= ~0x0100;

    // Configure the EUSCI_A3 module to use SMCLK as the clock source by
    // writing a value of 10b to the UCSSELx field (Bits 7 to 6) in the CTLW0 register
    EUSCI_A3->CTLW0 |= 0x00C0;

    // Set the baud rate value by writing to the UCBRx field (Bits 15 to 0)
    // in the BRW register
    // N = (Clock Frequency) / (Baud Rate) = (12,000,000 / 9600) = 1250
    // Use only the integer part, so N = 1250
    EUSCI_A3->BRW = 1250;

    // Disable the following interrupts by clearing the
    // corresponding bits in the IE register:
    // - Transmit Complete Interrupt (UCTXCPTIE, Bit 3)
    // - Start Bit Interrupt (UCSTTIE, Bit 2)
    EUSCI_A3->IE &= ~0x0C;

    // Enable the following interrupts by setting the
    // corresponding bits in the IE register
    // - Transmit Interrupt (UCTXIE, Bit 1)
    // - Receive Interrupt (UCRXIE, Bit 0)
    EUSCI_A3->IE |= 0x03;

    // Release the EUSCI_A3 module from the reset state by clearing the
    // UCSWRST bit (Bit 0) in the CTLW0 register
    EUSCI_A3->CTLW0 &= ~0x01;
}

uint8_t EUSCI_A3_UART_InChar()
{
    // Check the Receive Interrupt flag (UCRXIFG, Bit 0)
    // in the IFG register and wait if the flag is not set
    // If the UCRXIFG is set, then the Receive Buffer (UCAxRXBUF) has
    // received a complete character
    while((EUSCI_A3->IFG & 0x01) == 0);

    // Return the data from the Receive Buffer (UCAxRXBUF)
    // Reading the UCAxRXBUF will reset the UCRXIFG flag
    return EUSCI_A3->RXBUF;
}

void EUSCI_A3_UART_OutChar(uint8_t data)
{
    // Check the Transmit Interrupt flag (UCTXIFG, Bit 1)
    // in the IFG register and wait if the flag is not set
    // If the UCTXIFG is set, then the Transmit Buffer (UCAxTXBUF) is empty
    while((EUSCI_A3->IFG & 0x02) == 0);

    // Write the data to the Transmit Buffer (UCAxTXBUF)
    // Writing to the UCAxTXBUF will clear the UCTXIFG flag
    EUSCI_A3->TXBUF = data;
}

uint8_t EUSCI_A3_UART_Transmit_Data()
{
    uint8_t button_status = Get_Buttons_Status();
    uint8_t tx_data = 0x00;

    switch(button_status)
    {
        // Button 1 and Button 2 are pressed
        case 0x00:
        {
            tx_data = 0x00;
            EUSCI_A3_UART_OutChar(tx_data);
            break;
        }

        // Button 1 is pressed
        // Button 2 is not pressed
        case 0x10:
        {
            tx_data = 0xAA;
            EUSCI_A3_UART_OutChar(tx_data);
            break;
        }

        // Button 1 is not pressed
        // Button 2 is pressed
        case 0x02:
        {
            tx_data = 0x46;
            EUSCI_A3_UART_OutChar(tx_data);
            break;
        }

        // Button 1 and Button 2 are not pressed
        case 0x12:
        {
            tx_data = 0xF0;
            EUSCI_A3_UART_OutChar(tx_data);
            break;
        }

        default:
        {
            tx_data = 0xF0;
            EUSCI_A3_UART_OutChar(tx_data);
            break;
        }
    }

    return tx_data;
}

void EUSCI_A3_UART_Ramp_Data(uint8_t TX_Buffer[], uint8_t RX_Buffer[])
{
    // Create a for-loop that starts from index 0 to index 255 (use BUFFER_LENGTH)
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        // Make a function call to EUSCI_A3_UART_OutChar and pass the index value


        // Assign the value of the index to TX_Buffer[i]


        // Assign the value returned by EUSCI_A3_UART_InChar to RX_Buffer[i]

    }
}

void EUSCI_A3_UART_Validate_Data(uint8_t TX_Buffer[], uint8_t RX_Buffer[])
{
    // Create a for-loop that starts from index 0 to index 255 (use BUFFER_LENGTH)
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        // Print the contents of TX_Buffer[i] and RX_Buffer[i] in one line. There should be
        // a newline (i.e. \n) for each iteration


        // Include a condition that checks if TX_Buffer[i] != RX_Buffer[i]. If there is
        // a data mismatch between TX_Buffer[i] and RX_Buffer[i], then indicate in a
        // printf message that there is a mismatch and specify which set of data is not the same




    }
}
