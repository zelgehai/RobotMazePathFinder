/**
 * @file Buzzer.c
 * @brief Source code for the Buzzer driver.
 *
 * This file contains the function definitions for the Buzzer driver.
 * It interfaces with a Piezo Buzzer using the P1.6 pin (GPIO).
 *
 * @author Aaron Nanas
 *
 */

#include "../inc/Buzzer.h"

static int BUZZER_DURATION = 100;
static int NOTE_INTERVAL = 100;

static int C4_NOTE = 1980; // C4 (261 Hz)
static int D4_NOTE = 1750; // D4 (293 Hz)
static int E4_NOTE = 1555; // E4 (330 Hz)
static int F4_NOTE = 1470; // F4 (349 Hz)
static int G4_NOTE = 1310; // G4 (392 Hz)
static int A4_NOTE = 1165; // A4 (440 Hz)
static int B4_NOTE = 1040; // B4 (493 Hz)

void Buzzer_Init()
{
    // Configure the following pin as an output GPIO pin: P1.6
    // by clearing Bit 6 in the SEL0 and SEL1 registers for P1
    // and setting Bit 6 in the DIR register for P1




    // Initialize the output of the Piezo Buzzer to zero
    // by clearing Bit 6 in the OUT register for P1

}

void Buzzer_On()
{
    // Turn on the buzzer by setting Bit 6 in the OUT register for P1

}

void Buzzer_Off()
{
    // Turn off the buzzer by clearing Bit 6 in the OUT register for P1

}

void Play_Note(int note_delay_value)
{
    Buzzer_On();
    Clock_Delay1us(note_delay_value);

    Buzzer_Off();
    Clock_Delay1us(note_delay_value);
}

void Play_Note_Pattern()
{
    int note_pattern[] = {C4_NOTE, D4_NOTE, E4_NOTE, F4_NOTE, G4_NOTE, A4_NOTE, B4_NOTE};
    int num_notes = sizeof(note_pattern) / sizeof(note_pattern[0]);

    for (int i = 0; i < num_notes; i++)
    {
        for (int j = 0; j < BUZZER_DURATION; j++)
        {
            Play_Note(note_pattern[i]);
        }
        Clock_Delay1ms(NOTE_INTERVAL);
    }
}
