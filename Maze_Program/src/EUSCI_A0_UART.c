/**
 * @file EUSCI_A0_UART.c
 * @brief Source code for the EUSCI_A0_UART driver.
 *
 * This file contains the function definitions for the EUSCI_A0_UART driver.
 *
 * For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @note The pins P1.2 and P1.3 are used for UART communication via USB.
 *
 * @author Aaron Nanas
 *
 */

#include "../inc/EUSCI_A0_UART.h"

void EUSCI_A0_UART_Init()
{
    // Configure pins P1.2 (PM_UCA0RXD) and P1.3 (PM_UCA0TXD) to use the primary module function
    // by setting Bits 3 and 2 in the SEL0 register for P3
    // and clearing Bits 3 and 2 in the SEL1 register for P3
    P1->SEL0 |= 0x0C;
    P1->SEL1 &= ~0x0C;

    // Hold the EUSCI_A0 module in the reset state by setting the
    // UCSWRST bit (Bit 0) in the CTLW0 register
    EUSCI_A0->CTLW0 |= 1;

    // Clear all of the bits in the Modulation Control Word (MCTLW) register
    EUSCI_A0->MCTLW &= ~0xFF;

    // Disable the parity bit by clearing the UCPEN bit (Bit 15) in the CTLW0 register
    EUSCI_A0->CTLW0 &= ~0x8000;

    // Select odd parity for the parity bit by clearing the UCPAR bit (Bit 14) in the CTLW0 register
    // Note that the UCPAR bit is not used when parity is disabled
    EUSCI_A0->CTLW0 &= ~0x4000;

    // Set the bit order to Least Significant Bit (LSB) first by clearing the UCMSB bit (Bit 13) in the CTLW0 register
    EUSCI_A0->CTLW0 &= ~0x2000;

    // Select 8-bit character length by clearing the UC7BIT bit (Bit 12) in the CTLW0 register
    EUSCI_A0->CTLW0 &= ~0x1000;

    // Select one stop bit by clearing the UCSPB bit (Bit 11) in the CTLW0 register
    EUSCI_A0->CTLW0 &= ~0x0800;

    // Enable UART mode by writing 00b to the UCMODEx field (Bits 10-9) in the CTLW0 register
    EUSCI_A0->CTLW0 &= ~0x0600;

    // Disable synchronous mode by clearing the UCSYNC bit (Bit 8) in the CTLW0 register
    EUSCI_A0->CTLW0 &= ~0x0100;

    // Configure the EUSCI_A0 module to use SMCLK as the clock source by
    // writing a value of 10b to the UCSSELx field (Bits 7 to 6) in the CTLW0 register
    EUSCI_A0->CTLW0 |= 0x00C0;

    // Set the baud rate value by writing to the UCBRx field (Bits 15 to 0)
    // in the BRW register
    // N = (Clock Frequency) / (Baud Rate) = (12,000,000 / 115200) = 104.16666
    // Use only the integer part, so N = 104
    EUSCI_A0->BRW = 104;

    // Disable the following interrupts by clearing the
    // corresponding bits in the IE register:
    // - Transmit Complete Interrupt (UCTXCPTIE, Bit 3)
    // - Start Bit Interrupt (UCSTTIE, Bit 2)
    EUSCI_A0->IE &= ~0x0C;

    // Enable the following interrupts by setting the
    // corresponding bits in the IE register
    // - Transmit Interrupt (UCTXIE, Bit 1)
    // - Receive Interrupt (UCRXIE, Bit 0)
    EUSCI_A0->IE |= 0x03;

    // Release the EUSCI_A0 module from the reset state by clearing the
    // UCSWRST bit (Bit 0) in the CTLW0 register
    EUSCI_A0->CTLW0 &= ~0x01;
}

char EUSCI_A0_UART_InChar()
{
    // Check the Receive Interrupt flag (UCRXIFG, Bit 0)
    // in the IFG register and wait if the flag is not set
    // If the UCRXIFG is set, then the Receive Buffer (UCAxRXBUF) has
    // received a complete character
    while((EUSCI_A0->IFG & 0x01) == 0);

    // Return the data from the Receive Buffer (UCAxRXBUF)
    // Reading the UCAxRXBUF will reset the UCRXIFG flag
    return((char)(EUSCI_A0->RXBUF));
}

void EUSCI_A0_UART_OutChar(char letter)
{
    // Check the Transmit Interrupt flag (UCTXIFG, Bit 1)
    // in the IFG register and wait if the flag is not set
    // If the UCTXIFG is set, then the Transmit Buffer (UCAxTXBUF) is empty
    while((EUSCI_A0->IFG & 0x02) == 0);

    // Write the data to the Transmit Buffer (UCAxTXBUF)
    // Writing to the UCAxTXBUF will clear the UCTXIFG flag
    EUSCI_A0->TXBUF = letter;
}

void EUSCI_A0_UART_InString(char *bufPt, uint16_t max)
{
    int length = 0;
    char character = EUSCI_A0_UART_InChar();

    while(character != CR)
    {
        if (character == BS)
        {
            if (length)
            {
                bufPt--;
                length--;
                EUSCI_A0_UART_OutChar(BS);
            }
        }
        else if (length < max)
        {
            *bufPt = character;
            bufPt++;
            length++;
            EUSCI_A0_UART_OutChar(character);
        }
        character = EUSCI_A0_UART_InChar();
    }
    *bufPt = 0;
}

void EUSCI_A0_UART_OutString(char *pt)
{
    while(*pt)
    {
        EUSCI_A0_UART_OutChar(*pt);
        pt++;
    }
}

uint32_t EUSCI_A0_UART_InUDec()
{
    uint32_t number = 0;
    uint32_t length = 0;
    char character = EUSCI_A0_UART_InChar();

    // Accepts until <enter> is typed
    // The next line checks that the input is a digit, i.e. 0-9.
    // If the character is not 0-9, it is ignored and not echoed
    while(character != CR)
    {
        if ((character>='0') && (character<='9'))
        {
            // this line overflows if above 4294967295
            number = 10*number+(character-'0');
            length++;
            EUSCI_A0_UART_OutChar(character);
        }
        // If the input is a backspace, then the return number is
        // changed and a backspace is output to the screen
        else if ((character==BS) && length)
        {
            number /= 10;
            length--;
            EUSCI_A0_UART_OutChar(character);
        }
        character = EUSCI_A0_UART_InChar();
    }
  return number;
}

void EUSCI_A0_UART_OutUDec(uint32_t n)
{
    if (n >= 10)
    {
        EUSCI_A0_UART_OutUDec(n/10);
        n = n%10;
    }
    EUSCI_A0_UART_OutChar(n + '0'); /* n is between 0 and 9 */
}

void EUSCI_A0_UART_OutSDec(int32_t n)
{
    if (n < 0)
    {
        EUSCI_A0_UART_OutChar('-');
        EUSCI_A0_UART_OutUDec(-n);
    }
    else
    {
        EUSCI_A0_UART_OutUDec(n);
    }
}

void EUSCI_A0_UART_OutUFix(uint32_t n)
{
    EUSCI_A0_UART_OutUDec(n/10);
    EUSCI_A0_UART_OutChar('.');
    EUSCI_A0_UART_OutUDec(n%10);
}

uint32_t UART0_InUHex()
{
    uint32_t number = 0;
    uint32_t length = 0;
    uint32_t digit;

    char character = EUSCI_A0_UART_InChar();

    while(character != CR)
    {
        digit = 0x10; // assume bad
        if ((character>='0') && (character<='9'))
        {
            digit = character-'0';
        }
        else if ((character>='A') && (character<='F'))
        {
            digit = (character-'A')+0xA;
        }
        else if ((character>='a') && (character<='f'))
        {
            digit = (character-'a')+0xA;
        }

        // If the character is not 0-9 or A-F, it is ignored and not echoed
        if (digit <= 0xF)
        {
            number = number*0x10+digit;
            length++;
            EUSCI_A0_UART_OutChar(character);
        }
        // Backspace outputted and return value changed if a backspace is inputted
        else if ((character==BS) && length)
        {
            number /= 0x10;
            length--;
            EUSCI_A0_UART_OutChar(character);
        }
    character = EUSCI_A0_UART_InChar();
    }
    return number;
}

void EUSCI_A0_UART_OutUHex(uint32_t number)
{
    // Use recursion to convert the number of
    // unspecified length as an ASCII string
    if (number >= 0x10)
    {
        EUSCI_A0_UART_OutUHex(number/0x10);
        EUSCI_A0_UART_OutUHex(number%0x10);
    }
    else
    {
        if (number < 0xA)
        {
            EUSCI_A0_UART_OutChar(number+'0');
        }
        else
        {
            EUSCI_A0_UART_OutChar((number-0x0A)+'A');
        }
    }
}

int EUSCI_A0_UART_Open(const char *path, unsigned flags, int llv_fd)
{
    EUSCI_A0_UART_Init();
    return 0;
}

int EUSCI_A0_UART_Close(int dev_fd)
{
    return 0;
}
int EUSCI_A0_UART_Read(int dev_fd, char *buf, unsigned count)
{
    char ch;
    // Receive char from the serial terminal
    ch = EUSCI_A0_UART_InChar();
    // Return by reference
    ch = *buf;
    // Output the received char from the serial terminal
    EUSCI_A0_UART_OutChar(ch);
    return 1;
}

int EUSCI_A0_UART_Write(int dev_fd, const char *buf, unsigned count)
{
    unsigned int num = count;

    while(num)
    {
        if(*buf == 10)
        {
            EUSCI_A0_UART_OutChar(13);
        }
        EUSCI_A0_UART_OutChar(*buf);
        buf++;
        num--;
    }
    return count;
}

off_t EUSCI_A0_UART_LSeek(int dev_fd, off_t ioffset, int origin)
{
    return 0;
}

int EUSCI_A0_UART_Unlink(const char * path)
{
    return 0;
}

int EUSCI_A0_UART_Rename(const char *old_name, const char *new_name)
{
    return 0;
}

void EUSCI_A0_UART_Init_Printf()
{
    int ret_val;
    FILE *fptr;

    EUSCI_A0_UART_Init();

    ret_val = add_device("uart", _SSA,
                         EUSCI_A0_UART_Open,
                         EUSCI_A0_UART_Close,
                         EUSCI_A0_UART_Read,
                         EUSCI_A0_UART_Write, EUSCI_A0_UART_LSeek,
                         EUSCI_A0_UART_Unlink,
                         EUSCI_A0_UART_Rename);

    // Return if there is an error
    if (ret_val) return;

    fptr = fopen("uart","w");

    // Return if there is an error
    if (fptr == 0) return;

    // Redirect stdout to UART
    freopen("uart:", "w", stdout);

    // Turn off buffering for stdout
    setvbuf(stdout, NULL, _IONBF, 0);
}
