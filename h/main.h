#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>

// nxtOSEK headers
#include "kernel.h"
#include "kernel_id.h"	// Will be generated
#include "ecrobot_interface.h"

// user defined headers
#include "defines.h"
#include "movement.h"
#include "display.h"
#include "routing.h"
#include "melody.h"

/* -----------------------------------------------------------------------
 * Global variables for settings
 * ----------------------------------------------------------------------- */

// limits for edge detection
extern int hysteresis_low;
extern int hysteresis_high;

// 0 signals no line, 1 on line
extern int hysteresis_state;

// number of token being already found
extern int count_token;

/* -----------------------------------------------------------------------
 * Functions for the main loop
 * ----------------------------------------------------------------------- */

/*
 * Calculates the limits for the edge detection from the maximal and
 * minimal value that was found.
 */
void calculate_hysteresis (int max, int min);

/*
 * Calibrates the NXT edge detection. It will rotate 360° for this purpose
 * and scan the ground for the brightest and darkest value.
 */
void calibrate (void);

/*
 * Identifies if the NXT is over a line or not.
 */
int is_on_line (void);

/*
 * Dedicates if one of the two touch-sensors is pressed / active.
 */
int is_touched (void);

/*
 * Turns the signal light off
 */
void light_set_active (void);

/*
 * Turns the signal light on
 */
void light_set_inactive (void);

/*
 * The light begins blinking 10 times to signal an error.
 */
void light_error_signal (void);

/*
 * The NXT tries to go straight forward a given line until it gets
 * to a token or at a crossover
 */
void follow_line (void);

/*
 * The NXT drives to the center of the crossover.
 */
void center_crossover (void);

/*
 * The NXT rotates 360° and registers all edges found by it.
 */
void analyse_crossover (direction cnt_dir, node_t *cnt_node, l_list *nodes);

/*
 * This functions handles the behaviour of the NXT if a token
 * was found.
  */
void found_token (void);

/*
 * Handle some actions if the NXT is ready with its task. That means whether all
 * tokens are found and it is returned to the start point or all paths are passed.
 */
void ready (void);

/*
 * Turns the NXT on a crossover. if "left" is set to 1, the NXT will turn left and
 * turns right on 0
 */
void drive_turn (int left);

/*
 * Wrapper for drive_turn(1)
 */
void drive_left (void);

/*
 * Wrapper for drive_turn(0)
 */
void drive_right (void);

/*
 * Turns the NXT with 180°
 */
void drive_back (void);

/*
 * The NXT drives direct forward on a crossover
 */
void drive_forward (void);

/*
 * Turns the NXT from its current direction into the new direction.
 */
void translate_direction(direction cnt_dir, direction new_dir);

/*
 * The NXT will drive a route from the current node to the target node
 * of the route.
 */
void follow_route (route_t **route, node_t **cnt_node, direction *cnt_dir);

#endif /* CONTROLS_H_ */
