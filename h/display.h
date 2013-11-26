/*
 * A little interface for logging some messages on
 * the NXT display. Very helpful for debugging.
 *
 *
 */
#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdlib.h>
#include "defines.h"
#include "ecrobot_interface.h"
#include "routing.h"

/*
 * Clears the display and shows the new action on it at
 * the first line.
 */
void display_action (char action[]);

/*
 * Updates the display with the current difference between
 * the old and current light value from the light sensors.
 *
 * This will be displayed in line 2 and 3.
 */
void display_light();

/*
 * Displays if the NXT is on a line or not.
 */
void display_on_line (int state);

/*
 * Updates the display with the current revolution (absolute
 * or relative) of one wheel.
 *
 * It will be displayed in line 5 and 6;
 */
void display_revolution_delta(int delta);

/*
 * Displays the last 7 crossovers that was found by the NXT relative
 * to its orientation at the point in time when it was on the crossover.
 */
void display_crossover_stack (l_list *stack);

#endif /* DISPLAY_H_  */
