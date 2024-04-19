/**
 * @file main.c
 * @brief Main source code for the ADC program.
 *
 * This file contains the main entry point for the ADC program.
 * The main controller (Controller_1) demonstrates a Wall Follower robot that explores a simple maze.
 *
 * It interfaces the following peripherals using the ADC14 module to demonstrate object detection:
 *  - Three Sharp GP2Y0A21YK0F Analog Distance Sensor (10-80cm)
 *
 * Timers are used in this lab:
 *  - SysTick:  Used to generate periodic interrupts at a specified rate (100 Hz)
 *  - Timer A0: Used to generate PWM signals that will be used to drive the DC motors
 *  - Timer A1: Used to generate periodic interrupts at a specified rate (2 kHz)
 *
 * @author Aaron Nanas
 *
 */

#include <stdint.h>
#include <math.h>
#include "msp.h"
#include "inc/Clock.h"
#include "inc/CortexM.h"
#include "inc/GPIO.h"
#include "inc/EUSCI_A0_UART.h"
#include "inc/Motor.h"
#include "inc/SysTick_Interrupt.h"
#include "inc/Timer_A1_Interrupt.h"
#include "inc/LPF.h"
#include "inc/Analog_Distance_Sensors.h"

//#define CONTROLLER_1    1
//#define CONTROLLER_2    1
#define CONTROLLER_3    1

//#define DEBUG_ACTIVE    1

// Initialize constant distance values (in mm)
#define TOO_CLOSE_DISTANCE  200
#define TOO_FAR_DISTANCE    400
#define DESIRED_DISTANCE    250

// Initialize constant PWM duty cycle values for the motors
#define PWM_NOMINAL         2500
#define PWM_SWING           1000
#define PWM_MIN             (PWM_NOMINAL - PWM_SWING)
#define PWM_MAX             (PWM_NOMINAL + PWM_SWING)

// Declare global variables used to store filtered distance values from the Analog Distance Sensor
uint32_t Filtered_Distance_Left;
uint32_t Filtered_Distance_Center;
uint32_t Filtered_Distance_Right;

// Declare global variables used to store converted distance values from the Analog Distance Sensor
int32_t Converted_Distance_Left;
int32_t Converted_Distance_Center;
int32_t Converted_Distance_Right;

// Declare global variable used to store the amount of error
int32_t Error;

// Proportional Controller Gain
int32_t Kp = 4;

// Initialize set point to 250 mm
int32_t Set_Point = 250;

// Declare global variables used to update PWM duty cycle values for the motors
uint16_t Duty_Cycle_Left;
uint16_t Duty_Cycle_Right;

/**
 * @brief This function enables ADC14 and samples the three Analog Distance Sensors.
 *
 * This function enables the ADC14 module and samples data from three Analog Distance Sensors (A17, A14, and A16).
 * It applies a low-pass filter to the raw sensor values and then calibrates them to obtain converted distance values.
 * The converted distance values are used for distance control and may be optionally printed if debug mode is active.
 *
 * @param None
 *
 * @return None
 */
void Sample_Analog_Distance_Sensor()
{
    // Declare local variables for the Sharp GP2Y0A21YK0F Analog Distance Sensors
    // before passing their addresses
    uint32_t Raw_A17;
    uint32_t Raw_A14;
    uint32_t Raw_A16;

    // Start conversion of Analog Distance Sensor raw values
    Analog_Distance_Sensor_Start_Conversion(&Raw_A17, &Raw_A14, &Raw_A16);

    // Apply low-pass filter to raw values
    Filtered_Distance_Right = LPF_Calc(Raw_A17);
    Filtered_Distance_Center = LPF_Calc2(Raw_A14);
    Filtered_Distance_Left = LPF_Calc3(Raw_A16);

    // Convert filtered distance values using the calibration formula
    Converted_Distance_Left = Analog_Distance_Sensor_Calibrate(Filtered_Distance_Left);
    Converted_Distance_Center = Analog_Distance_Sensor_Calibrate(Filtered_Distance_Center);
    Converted_Distance_Right = Analog_Distance_Sensor_Calibrate(Filtered_Distance_Right);
}

/**
 * @brief This function updates the PWM duty cycle values for the motors based on the measured distance values.
 *
 * This function calculates the error between the measured distances of the left and right sensors
 * and adjusts the motor duty cycles to maintain a desired distance. It also ensures that the
 * duty cycles remain within the defined range. It is used to drive along walls in a track.
 *
 * @param None
 *
 * @return None
 */
void Controller_1()
{
    // Check if both the left and right distance sensor readings are greater than the desired distance
    if ((Converted_Distance_Left > DESIRED_DISTANCE) && (Converted_Distance_Right > DESIRED_DISTANCE))
    {
        // Calculate the set point as the average of the left and right sensor distance readings
        Set_Point = (Converted_Distance_Left + Converted_Distance_Right) / 2;
    }
    else
    {
        // If at least one distance sensor reading is below the desired distance, assign the set point to the desired distance
        Set_Point = DESIRED_DISTANCE;
    }

    // Calculate the error based on the sensor readings
    if (Converted_Distance_Left < Converted_Distance_Right)
    {
        Error = Converted_Distance_Left - Set_Point;
    }
    else
    {
        Error = Set_Point - Converted_Distance_Right;
    }

    // Calculate the new duty cycle for the right motor based on the error and proportional constant (Kp)
    Duty_Cycle_Right = PWM_NOMINAL + (Kp * Error);

    // Calculate the new duty cycle for the left motor based on the error and proportional constant (Kp)
    Duty_Cycle_Left  = PWM_NOMINAL - (Kp * Error);

    // Ensure that the duty cycle for the right motor does not go below the minimum PWM value
    if (Duty_Cycle_Right < PWM_MIN) Duty_Cycle_Right = PWM_MIN;

    // Ensure that the duty cycle for the right motor does not exceed the maximum PWM value
    if (Duty_Cycle_Right > PWM_MAX) Duty_Cycle_Right = PWM_MAX;

    // Ensure that the duty cycle for the left motor does not go below the minimum PWM value
    if (Duty_Cycle_Left  < PWM_MIN) Duty_Cycle_Left  = PWM_MIN;

    // Ensure that the duty cycle for the left motor does not exceed the maximum PWM value
    if (Duty_Cycle_Left  > PWM_MAX) Duty_Cycle_Left  = PWM_MAX;

#ifndef DEBUG_ACTIVE
    // Apply the updated PWM duty cycle values to the motors
    Motor_Forward(Duty_Cycle_Left, Duty_Cycle_Right);
#endif
}

void Controller_2()
{
    // Check if both the left and right distance sensor readings are greater than the desired distance
        if ((Converted_Distance_Left > DESIRED_DISTANCE) && (Converted_Distance_Right > DESIRED_DISTANCE))
        {
            // Calculate the set point as the average of the left and right sensor distance readings
            Set_Point = (Converted_Distance_Left + Converted_Distance_Right) / 2;
        }
        else
        {
            // If at least one distance sensor reading is below the desired distance, assign the set point to the desired distance
            Set_Point = DESIRED_DISTANCE;
        }

        // Calculate the error based on the sensor readings
        if (Converted_Distance_Left < Converted_Distance_Right)
        {
            Error = Converted_Distance_Left - Set_Point;
        }
        else
        {
            Error = Set_Point - Converted_Distance_Right;
        }

        // Calculate the new duty cycle for the right motor based on the error and proportional constant (Kp)
        Duty_Cycle_Right = PWM_NOMINAL - (Kp * Error);

        // Calculate the new duty cycle for the left motor based on the error and proportional constant (Kp)
        Duty_Cycle_Left  = PWM_NOMINAL + (Kp * Error);

        // Ensure that the duty cycle for the right motor does not go below the minimum PWM value
        if (Duty_Cycle_Right < PWM_MIN) Duty_Cycle_Right = PWM_MIN;

        // Ensure that the duty cycle for the right motor does not exceed the maximum PWM value
        if (Duty_Cycle_Right > PWM_MAX) Duty_Cycle_Right = PWM_MAX;

        // Ensure that the duty cycle for the left motor does not go below the minimum PWM value
        if (Duty_Cycle_Left  < PWM_MIN) Duty_Cycle_Left  = PWM_MIN;

        // Ensure that the duty cycle for the left motor does not exceed the maximum PWM value
        if (Duty_Cycle_Left  > PWM_MAX) Duty_Cycle_Left  = PWM_MAX;

#ifndef DEBUG_ACTIVE
    // Apply the updated PWM duty cycle values to the motors
    if((Converted_Distance_Center > DESIRED_DISTANCE) && (Converted_Distance_Center < 800)){
        Motor_Forward(Duty_Cycle_Left, Duty_Cycle_Right);
    }else{
        Motor_Stop();
    }
#endif

}

/**
 * @brief
 *
 * @param None
 *
 * @return None
 */
void Controller_3()
{
    // Check if both the left and right distance sensor readings are greater than the desired distance
            if ((Converted_Distance_Left > DESIRED_DISTANCE) && (Converted_Distance_Right > DESIRED_DISTANCE))
            {
                // Calculate the set point as the average of the left and right sensor distance readings
                Set_Point = (Converted_Distance_Left + Converted_Distance_Right) / 2;
            }
            else
            {
                // If at least one distance sensor reading is below the desired distance, assign the set point to the desired distance
                Set_Point = DESIRED_DISTANCE;
            }

            // Calculate the error based on the sensor readings
            if (Converted_Distance_Left < Converted_Distance_Right)
            {
                Error = Converted_Distance_Left - Set_Point;
            }
            else
            {
                Error = Set_Point - Converted_Distance_Right;
            }

            // Calculate the new duty cycle for the right motor based on the error and proportional constant (Kp)
            Duty_Cycle_Right = PWM_NOMINAL - (Kp * Error);

            // Calculate the new duty cycle for the left motor based on the error and proportional constant (Kp)
            Duty_Cycle_Left  = PWM_NOMINAL + (Kp * Error);

            // Ensure that the duty cycle for the right motor does not go below the minimum PWM value
            if (Duty_Cycle_Right < PWM_MIN) Duty_Cycle_Right = PWM_MIN;

            // Ensure that the duty cycle for the right motor does not exceed the maximum PWM value
            if (Duty_Cycle_Right > PWM_MAX) Duty_Cycle_Right = PWM_MAX;

            // Ensure that the duty cycle for the left motor does not go below the minimum PWM value
            if (Duty_Cycle_Left  < PWM_MIN) Duty_Cycle_Left  = PWM_MIN;

            // Ensure that the duty cycle for the left motor does not exceed the maximum PWM value
            if (Duty_Cycle_Left  > PWM_MAX) Duty_Cycle_Left  = PWM_MAX;

    #ifndef DEBUG_ACTIVE
        // Apply the updated PWM duty cycle values to the motors
        if((Converted_Distance_Center >= DESIRED_DISTANCE) && (Converted_Distance_Center < 800)){
            Motor_Forward(Duty_Cycle_Left, Duty_Cycle_Right);
        }else if (Converted_Distance_Center < (DESIRED_DISTANCE - 50)){
            Motor_Backward(Duty_Cycle_Left, Duty_Cycle_Right);
        }
        else{
            Motor_Stop();
        }
    #endif

}


/**
 * @brief This function is the handler for the SysTick periodic interrupt with a rate of 100 Hz.
 *
 * The SysTick_Handler generates a periodic interrupt that calls a specific controller function based on the selected
 * active configuration. Only one of the options can be defined at a time: CONTROLLER_1, CONTROLLER_2, or CONTROLLER_3.
 *
 * @param None
 *
 * @return None
 */
void SysTick_Handler(void)
{
#if defined CONTROLLER_1

    //Controller_1();

#elif defined CONTROLLER_2
    #if defined CONTROLLER_1 || CONTROLLER_3
        #error "Only CONTROLLER_1, CONTROLLER_2, or CONTROLLER_3 can be active at the same time."
    #endif

    // Your function for Task 1 goes here (Controller_2)
    //Controller_2();

#elif defined CONTROLLER_3
    #if defined CONTROLLER_1 || CONTROLLER_2
#error "Only CONTROLLER_1, CONTROLLER_2, or CONTROLLER_3 can be active at the same time."
    #endif

    // Your function for Task 2 goes here (Controller_3)
    Controller_3();

#else
    #error "Define either one of the options: CONTROLLER_1, CONTROLLER_2, or CONTROLLER_3."
#endif
}

/**
 * @brief User-defined function executed by Timer A1 using a periodic interrupt at a rate of 2 kHz.
 *
 * The Timer_A1_Periodic_Task function generates a periodic interrupt with a rate of 2 kHz. It samples the
 * distance values measured by three Sharp GP2Y0A21YK0F analog distance sensors.
 *
 * @param None
 *
 * @return None
 */
void Timer_A1_Periodic_Task(void)
{
    Sample_Analog_Distance_Sensor();
}

int main(void)
{
    // Declare local variables for the Sharp GP2Y0A21YK0F Analog Distance Sensors
    // before passing their addresses
    uint32_t Raw_A17;
    uint32_t Raw_A14;
    uint32_t Raw_A16;

    // Initialize the 48 MHz Clock
    Clock_Init48MHz();

    // Ensure that interrupts are disabled during initialization
    DisableInterrupts();

    // Initialize EUSCI_A0_UART to use the printf function
    EUSCI_A0_UART_Init_Printf();

    // Initialize the DC motors
    Motor_Init();

    // Initialize motor duty cycle values
    Duty_Cycle_Left  = PWM_NOMINAL;
    Duty_Cycle_Right = PWM_NOMINAL;

    // Initialize the Analog Distance Sensor using the ADC14 module
    Analog_Distance_Sensor_Init();

    // Start conversion of Analog Distance Sensor raw values
    Analog_Distance_Sensor_Start_Conversion(&Raw_A17, &Raw_A14, &Raw_A16);

    // Initialize low-pass filters for the Analog Distance Sensor
    LPF_Init(Raw_A17, 64);
    LPF_Init2(Raw_A14, 64);
    LPF_Init3(Raw_A16, 64);

    // Initialize SysTick periodic interrupt with a rate of 100 Hz
    SysTick_Interrupt_Init(SYSTICK_INT_NUM_CLK_CYCLES, SYSTICK_INT_PRIORITY);

    // Initialize Timer A1 with interrupts enabled and an interrupt rate of 2 kHz
    Timer_A1_Interrupt_Init(&Timer_A1_Periodic_Task, TIMER_A1_INT_CCR0_VALUE);

    // Enable the interrupts used by Timer A1 and other modules
    EnableInterrupts();

    while(1)
    {
#ifdef DEBUG_ACTIVE
        printf("Left: %d mm | Center: %d mm | Right: %d mm\n", Converted_Distance_Left, Converted_Distance_Center, Converted_Distance_Right);
        Clock_Delay1us(500);
#endif
    }
}
