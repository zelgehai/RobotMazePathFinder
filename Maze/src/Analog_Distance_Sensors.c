/**
 * @file Analog_Distance_Sensor.c
 * @brief Source code for the Analog_Distance_Sensor driver.
 *
 * This file contains the function definitions for the Analog_Distance_Sensor driver.
 * It uses the ADC14 module to interface with three Sharp GP2Y0A21YK0F Analog Distance Sensors.
 *
 * The Sharp GP2Y0A21YK0F Analog Distance Sensors uses the following pinout:
 *  - Right GP2Y0A21YK0F VOUT   <-->  MSP432 LaunchPad Pin P9.0 (Analog Input, A17)
 *  - Center GP2Y0A21YK0F VOUT  <-->  MSP432 LaunchPad Pin P6.1 (Analog Input, A14)
 *  - Left GP2Y0A21YK0F VOUT    <-->  MSP432 LaunchPad Pin P9.1 (Analog Input, A16)
 *  - GP2Y0A21YK0F VCC          <-->  MSP432 LaunchPad 5V
 *  - GP2Y0A21YK0F GND          <-->  MSP432 LaunchPad GND
 *
 * @author Aaron Nanas
 *
 */

#include "../inc/Analog_Distance_Sensors.h"

void Analog_Distance_Sensor_Init()
{
    // Clear ADC14ENC (Bit 1) to 0 to disable conversion
    ADC14->CTL0 &= ~0x00000002;

    // Wait for ADC14BUSY (Bit 16) to be 0
    while(ADC14->CTL0 & 0x00010000);

    //     CTL0 Register Configuration
    //
    //     Bit(s)         Field             Value       Description
    //     -----        ----------          ------      -------------
    //     31-30        ADC14PDIV           00b         Predivide selected ADC14CLK: Predivide by 1
    //     29-27        ADC14SHSx           000b        Sample-and-hold source select: ADC14SC bit
    //      26          ADC14SHP            1b          SAMPCON signal is sourced from the sampling timer
    //      25          ADC14ISSH           0b          Sample-input signal is not inverted
    //     24-22        ADC14DIVx           000b        Divide ADC14CLK frequency by 1
    //     21-19        ADC14SSELx          100b        ADC14CLK clock source: SMCLK
    //     18-17        ADC14CONSEQx        01b         Conversion sequence mode: Sequence-of-channels
    //      16          ADC14BUSY           0b          ADC14 busy status. Read-only.
    //     15-12        ADC14SHT1x          0011b       Sample-and-hold time of 32 ADC14CLK clock cycles
    //     11-8         ADC14SHT0x          0011b       Sample-and-hold time of 32 ADC14CLK clock cycles
    //      7           ADC14MSC            1b          Further sample-and-conversions are performed automatically
    //     6-5          Reserved            00b         Reserved
    //      4           ADC14ON             0b          ADC14 off
    //     3-2          Reserved            00b         Reserved
    //      1           ADC14ENC            0b          Disable conversion
    //      0           ADC14SC             0b          No sample-and-conversion start
    ADC14->CTL0 = 0x04223390;

    //     CTL1 Register Configuration
    //
    //     Bit(s)         Field             Value       Description
    //     -----        ----------          ------      -------------
    //     20-16        ADC14STARTADDx      00010b      Select start address of ADC14MEM2
    //     15-6         Reserved            000b        Reserved
    //     5-4          ADC14RES            11b         14-bit resolution (16 clock cycle conversion time)
    //      3           ADC14DF             0b          Data read-back format: Unsigned binary
    //      2           ADC14REFBURST       0b          Reference buffer on continuously
    //     1-0          ADC14PWRMD          00b         Regular power mode
    ADC14->CTL1 = 0x00020030;

    //     MCTL2 Register Configuration
    //
    //     Bit(s)         Field             Value       Description
    //     -----        ----------          ------      -------------
    //      15          ADC14WINCTH         0b          Not Used
    //      14          ADC14WINC           0b          Comparator window disabled
    //      13          ADC14DIF            0b          Single-ended mode enabled
    //      12          Reserved            0b          Reserved
    //     11-8         ADC14VRSEL          0000b       V(R+) = AVCC, V(R-) = AVSS
    //      7           ADC14EOS            0b          Not end of sequence
    //     6-5          Reserved            00b         Reserved
    //     4-0          ADC14INCHx          10001b      Input channel select: A17, P9.0
    ADC14->MCTL[2] = 0x00000011;

    //     MCTL3 Register Configuration
    //
    //     Bit(s)         Field             Value       Description
    //     -----        ----------          ------      -------------
    //      15          ADC14WINCTH         0b          Not Used
    //      14          ADC14WINC           0b          Comparator window disabled
    //      13          ADC14DIF            0b          Single-ended mode enabled
    //      12          Reserved            0b          Reserved
    //     11-8         ADC14VRSEL          0000b       V(R+) = AVCC, V(R-) = AVSS
    //      7           ADC14EOS            0b          Not end of sequence
    //     6-5          Reserved            00b         Reserved
    //     4-0          ADC14INCHx          01110b      Input channel select: A14, P6.1
    ADC14->MCTL[3] = 0x0000000E;

    //     MCTL4 Register Configuration
    //
    //     Bit(s)         Field             Value       Description
    //     -----        ----------          ------      -------------
    //      15          ADC14WINCTH         0b          Not Used
    //      14          ADC14WINC           0b          Comparator window disabled
    //      13          ADC14DIF            0b          Single-ended mode enabled
    //      12          Reserved            0b          Reserved
    //     11-8         ADC14VRSEL          0000b       V(R+) = AVCC, V(R-) = AVSS
    //      7           ADC14EOS            1b          End of sequence (Last conversion)
    //     6-5          Reserved            00b         Reserved
    //     4-0          ADC14INCHx          10000b      Input channel select: A16, P9.1
    ADC14->MCTL[4] = 0x00000090;

    // Disable all interrupts
    ADC14->IER0 = 0;
    ADC14->IER1 = 0;

    // Configure P6.1 as A14 (Analog Input)
    P6->SEL1 |= 0x02;
    P6->SEL0 |= 0x02;

    // Configure P9.1 and P9.0 as A16 and A17 (Analog Input)
    P9->SEL1 |= 0x03;
    P9->SEL0 |= 0x03;

    // Set ADC14ENC (Bit 1) to 1 to enable conversion
    ADC14->CTL0 |= 0x00000002;
}

void Analog_Distance_Sensor_Start_Conversion(uint32_t *Ch_17, uint32_t *Ch_14, uint32_t *Ch_16)
{
    // Wait for ADC14BUSY (Bit 16) to be 0
    while(ADC14->CTL0 & 0x00010000);

    // Start sample-and-conversion
    // Note: ADC14SC is reset automatically
    ADC14->CTL0 |= 0x00000001;

    // Wait for ADC14IFG4 to be set (when ADC14MEM4 is loaded with a conversion result)
    // Note: This bit is reset to 0 when the ADC14MEM4 register is read (i.e. when the last channel, A16, is read)
    while((ADC14->IFGR0 & 0x10) == 0);

    // Read the result of P9.0 (A17) from the MEM[2] register (0 to 16383)
    *Ch_17 = ADC14->MEM[2];

    // Read the result of P6.1 (A14) from the MEM[3] register (0 to 16383)
    *Ch_14 = ADC14->MEM[3];

    // Read the result of P9.1 (A16) from the MEM[4] register (0 to 16383)
    *Ch_16 = ADC14->MEM[4];
}

int32_t Analog_Distance_Sensor_Calibrate(int filtered_distance)
{
    // If the filtered distance (after LPF) is less than the max, return 800 mm
    if (filtered_distance < ANALOG_DISTANCE_SENSOR_MAX)
    {
        return 800;
    }

    // Otherwise, calculate the calibrated distance using the calibration formula
    return (Ax / (filtered_distance + Bx) + Cx);
}
