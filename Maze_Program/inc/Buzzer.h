/**
 * @file Buzzer.h
 * @brief Header file for the Buzzer driver.
 *
 * This file contains the function definitions for the Buzzer driver.
 * It interfaces with a Piezo Buzzer using the P1.6 pin (GPIO).
 *
 * @author Aaron Nanas
 *
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include <stdint.h>
#include "msp.h"
#include "Clock.h"

/**
 * @brief The Buzzer_Init function initializes the pin used by the Piezo Buzzer (P1.6).
 *
 * This function configures the P1.6 pin used by the Piezo Buzzer as an output GPIO pin.
 *
 * @param None
 *
 * @return None
 */
void Buzzer_Init();

/**
 * @brief Turns on the buzzer by setting the P1.6 pin to 1.
 *
 * @param None
 *
 * @return None
 */
void Buzzer_On();

/**
 * @brief Turns on the buzzer by clearing the P1.6 pin to 0.
 *
 * @param None
 *
 * @return None
 */
void Buzzer_Off();

/**
 * @brief The Play_Note function produces a square wave signal based on the selected frequency.
 *
 * @param note_delay_value Determines the musical note to be played by the Piezo Buzzer by toggling its
 * GPIO output pin at the specified period.
 *
 *  Note    Frequency (Hz)      note_delay_value
 *  C4          261                 1980
 *  D4          293                 1750
 *  E4          330                 1555
 *  F4          349                 1470
 *  G4          392                 1310
 *  A4          440                 1165
 *  B4          493                 1040
 *
 * @return None
 */
void Play_Note(int note_delay_value);

/**
 * @brief The Play_Note_Pattern function plays multiple tones using the Piezo Buzzer.
 *
 * This function plays the following notes in order: C4, D4, E4, F4, G4, A4, and B4.
 *
 * @param None
 *
 * @return None
 */
void Play_Note_Pattern();

#endif /* INC_BUZZER_H_ */
