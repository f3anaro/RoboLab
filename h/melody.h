/*
 * melody.h
 *
 *  Created on: 13.03.2013
 *      Author: Lucas
 */

#ifndef MELODY_H_
#define MELODY_H_

#include "defines.h"
#include "ecrobot_interface.h"

#define VOLUME 40

/*
 * Plays a tone with a specific frequency and duration.
 */
void melody_play_tone (int frequency, int duration, int *dance_left);

void melody_death (void);

/*
 * Plays the melody of Indiana Johns
 */
void melody_indiana (void);

#endif /* MELODY_H_ */
