/**
 * @file EUSCI_B1_I2C.c
 * @brief Source code for the EUSCI_B1_I2C driver.
 *
 * This file contains the function definitions for the EUSCI_B1_I2C driver.
 * The EUSCI_B1_I2C driver uses busy-wait implementation.
 *
 * @note This function assumes that the necessary pin configurations for I2C communication have been performed
 *       on the corresponding pins. The output from the pins will be observed using an oscilloscope.
 *       - P6.4 (SDA)
 *       - P6.5 (SCL)
 *
 * For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @author Aaron Nanas
 *
 */

#include "../inc/EUSCI_B1_I2C.h"

void EUSCI_B1_I2C_Init()
{
    // Hold the EUSCI_B1 module in reset mode
    EUSCI_B1->CTLW0 |= 0x0001;

    //     CTWL0 Register Configuration
    //
    //     Bit(s)      Field       Value       Description
    //     -----       -----       -----       -----------
    //      15         UCA10        0x0        Own address is a 7-bit address
    //      14         UCSLA10      0x0        Address slave has a 7-bit address
    //      13         UCMM         0x0        Single master
    //      12         Reserved     0x0        Reserved
    //      11         UCMST        0x1        Master mode is selected
    //      10-9       UCMODEx      0x3        I2C mode is selected
    //      8          UCSYNC       0x1        Synchronous mode
    //      7-6        UCSSELx      0x3        eUSCI clock source is SMCLK
    //      5          UCTXACK      0x0        Transmit ACK condition in slave mode
    //      4          UCTR         0x0        Transmitter / Receiver
    //      3          UCTXNACK     0x0        Transmit NACK condition in slave mode
    //      2          UCTXSTP      0x0        Transmit STOP condition in master mode
    //      1          UCTXSTT      0x1        Transmit START condition in master mode
    //      0          UCSWRST      0x1        eUSCI logic held in reset state
    EUSCI_B1->CTLW0 |= 0x0FC1;

    //     CTWL1 Register Configuration
    //
    //     Bit(s)      Field       Value       Description
    //     -----       -----       -----       -----------
    //      15-9       Reserved     0x0        Reserved
    //      8          UCETXINT     0x0        Early UCTXIFG0 flag in slave mode
    //      7-6        UCCLTO       0x0        Disable clock low timeout
    //      5          UCSTPNACK    0x0        Send NACK before STOP condition in master receiver mode
    //      4          UCSWACK      0x0        Address acknowledge of slave is controlled by eUSCI module
    //      3-2        UCASTPx      0x0        No automatic STOP generation in slave mode when UCBCNTIFG is available
    //      1-0        UCGLITx      0x0        Deglitch time of 50 ns
    EUSCI_B1->CTLW1 = 0;

    // Set the SCL frequency. Since SMCLK is selected as the clock source,
    // the frequency used is 12 MHz
    // Choose 400 kHz for the SCL frequency:
    // N = (Clock Frequency) / (SCL Frequency) = (12,000,000 / 400,000) = 30
    // N = 30
    EUSCI_B1->BRW = 30;

    // Configure pins P6.4 (SDA) and P6.5 (SCL) as primary module function
    P6->SEL0 |= 0x30;
    P6->SEL1 &= ~0x30;

    // Clear the software reset bit to enable the EUSCI_B1 module
    EUSCI_B1->CTLW0 &= ~0x0001;

    // Ensure that all interrupts are disabled
    EUSCI_B1->IE = 0x0000;
}

void EUSCI_B1_I2C_Send_A_Byte(uint8_t slave_address, uint8_t data)
{
    // Wait until I2C is not busy
    while(EUSCI_B1->STATW & 0x0010);

    // Set the slave address
    EUSCI_B1->I2CSA = slave_address;

    // Configure I2C master transmit mode
    // Clear UCTXSTP (Bit 2): No STOP condition
    // Set UCTR (Bit 4): Transmitter mode
    // Set UCTXSTT (Bit 1): Generate START condition
    EUSCI_B1->CTLW0 = (EUSCI_B1->CTLW0 & ~0x0004) | 0x0012;

    // Wait for UCTXIFG0
    // Note: UCTXIFG0 is the TX interrupt flag
    while((EUSCI_B1->IFG & 0x0002) == 0);

    // Write data to TX buffer
    EUSCI_B1->TXBUF = data;

    // Wait for UCTXIFG0
    while((EUSCI_B1->IFG & 0x0002) == 0);

    // Set UCTXSTP (Bit 2): Generate STOP condition
    EUSCI_B1->CTLW0 |= 0x0004;

    // Clear UCTXIFG0
    EUSCI_B1->IFG &= ~0x0002;
}

void EUSCI_B1_I2C_Send_Multiple_Bytes(uint8_t slave_address, uint8_t *data_buffer, uint32_t packet_length)
{
    // Wait until I2C is not busy
    while(EUSCI_B1->STATW & 0x0010);

    // Set the slave address
    EUSCI_B1->I2CSA = slave_address;

    // Configure I2C master transmit mode
    // Clear UCTXSTP (Bit 2): No STOP condition
    // Set UCTR (Bit 4): Transmitter mode
    // Set UCTXSTT (Bit 1): Generate START condition
    EUSCI_B1->CTLW0 = (EUSCI_B1->CTLW0 & ~0x0004) | 0x0012;

    // Loop to write the data array to the TX buffer
    for(int i = 0; i < packet_length; i++)
    {
        // Wait for UCTXIFG0
        // Note: UCTXIFG0 is the TX interrupt flag
        while((EUSCI_B1->IFG & 0x0002) == 0);

        // Write data to TX buffer
        EUSCI_B1->TXBUF = data_buffer[i];
    }

    // Wait for UCTXIFG0
    while((EUSCI_B1->IFG & 0x0002) == 0);

    // Set UCTXSTP (Bit 2): Generate STOP condition
    EUSCI_B1->CTLW0 |= 0x0004;

    // Clear UCTXIFG0
    EUSCI_B1->IFG &= ~0x0002;
}

uint8_t EUSCI_B1_I2C_Receive_A_Byte(uint8_t slave_address)
{
    // Wait until I2C is not busy
    while(EUSCI_B1->STATW & 0x0010);

    // Hold the EUSCI_B1 module in reset mode
    EUSCI_B1->CTLW0 |= 0x0001;

    // Set the byte counter threshold to 1
    // This register can only be modified when UCSWRST is set to 1
    EUSCI_B1->TBCNT |= 0x0001;

    // Clear the software reset bit to enable the EUSCI_B1 module
    EUSCI_B1->CTLW0 &= ~0x0001;

    // Set the slave address
    EUSCI_B1->I2CSA = slave_address;

    // Configure I2C master receive mode
    // Clear UCTR (Bit 4): Receive mode
    // Set UCTXSTP (Bit 2): Generate STOP condition
    // Set UCTXSTT (Bit 1): Generate START condition
    EUSCI_B1->CTLW0 = ((EUSCI_B1->CTLW0 & ~0x0010) | 0x0006);

    // Wait for UCRXIFG0
    // Note: UCRXIFG0 is the RX interrupt flag
    while((EUSCI_B1->IFG & 0x0001) == 0);

    // Retrieve received data from the RX buffer and write to a local variable
    uint8_t data = EUSCI_B1->RXBUF;

    // Return received data
    return data;
}

void EUSCI_B1_I2C_Receive_Multiple_Bytes(uint8_t slave_address, uint8_t *data_buffer, uint16_t packet_length)
{
    // Set the slave address
    EUSCI_B1->I2CSA = slave_address;

    // Configure I2C master receive mode
    // Clear UCTR (Bit 4): Receive mode
    EUSCI_B1->CTLW0 &= ~0x0010;

    // Set UCTXSTT (Bit 1): Generate START condition
    EUSCI_B1->CTLW0 |= 0x0002;

    // Loop to receive data and write to data buffer
    for(int i = 0; i < packet_length; i++)
    {
        // Check if this is the last byte
        if (i == packet_length-1)
        {
            // Set UCTXSTP (Bit 2): Generate STOP condition
            // In master receiver mode, the STOP condition is preceded by a NACK
            EUSCI_B1->CTLW0 |= 0x0004;
        }

        // Wait for UCRXIFG0
        // Note: UCRXIFG0 is the RX interrupt flag
        while((EUSCI_B1->IFG & 0x0001) == 0);

        // Retrieve received data from the RX buffer and write to data_buffer
        data_buffer[i] = EUSCI_B1->RXBUF;
    }

    // Wait while UCTXSTP is 1
    // Note: UCTXSTP is automatically cleared after the STOP condition is generated
    while(EUSCI_B1->CTLW0 & 0x0004);
}
