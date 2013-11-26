#include <stdlib.h>
#include <string.h>
#include "kernel.h"
/* Will be generated: */
#include "kernel_id.h"
#include "ecrobot_interface.h"

void stop_motors (void);


/* nxtOSEK hook to be invoked from an ISR in category 2 */
void user_1ms_isr_type2(void) {
}

void ecrobot_device_initialize(void) {
	ecrobot_set_light_sensor_active(NXT_PORT_S1);
}

void ecrobot_device_terminate() {
	ecrobot_set_light_sensor_inactive(NXT_PORT_S1);
	stop_motors();
}

void hello_world() {
	ecrobot_status_monitor("Hello, World!");
}


void display_status (int before_darkness) {
	display_clear(1);
	display_goto_xy(0,0);
	display_string("Motor speed:");

	// left
	display_goto_xy(1,2);
	display_string("Left:");
	display_goto_xy(7,2);
	display_int(ecrobot_get_motor_rev(NXT_PORT_A), 3);

	// right
	display_goto_xy(1,4);
	display_string("Right:");
	display_goto_xy(7,4);
	display_int(ecrobot_get_motor_rev(NXT_PORT_B), 3);

	display_goto_xy(0, 6);
	display_string("Light:");
	display_goto_xy(7, 6);
	display_int(before_darkness - ecrobot_get_light_sensor(NXT_PORT_S1),3);
	display_update();
}

void stop_motors () {
	ecrobot_set_motor_mode_speed(NXT_PORT_A, 1, 0);
	ecrobot_set_motor_mode_speed(NXT_PORT_B, 1, 0);
}

void move_jumpy_forward () {
	ecrobot_set_motor_mode_speed(NXT_PORT_A, 0, 60);

	// correct slower left wheel
	if (ecrobot_get_motor_rev(NXT_PORT_A) - ecrobot_get_motor_rev(NXT_PORT_B) > 0) {
		ecrobot_set_motor_mode_speed(NXT_PORT_B, 0, 70);
	} else {
		ecrobot_set_motor_mode_speed(NXT_PORT_B, 0, 60);
	}

	systick_wait_ms(25);
	// stop_motors();
	// systick_wait_ms(100);
}

void rotate_left () {
	ecrobot_set_motor_mode_speed(NXT_PORT_A, 0, 60);
	ecrobot_set_motor_mode_speed(NXT_PORT_B, 0, -65);
}

void rotate_right () {
	ecrobot_set_motor_mode_speed(NXT_PORT_A, 0, -60);
	ecrobot_set_motor_mode_speed(NXT_PORT_B, 0, 65);
}

/*
 *
 */
void move_to_corssroad_center () {
	// get current distance
	// TODO was passiert bei negativen Werten?
	int start_revolution = ecrobot_get_motor_rev(NXT_PORT_A);

	// move 210° forward
	while (ecrobot_get_motor_rev(NXT_PORT_A) - start_revolution < 210) {
		display_status(ecrobot_get_light_sensor(NXT_PORT_S1));

		// move forward left twisted
		ecrobot_set_motor_mode_speed(NXT_PORT_A, 0, 60);
		ecrobot_set_motor_mode_speed(NXT_PORT_B, 0, 60);

		systick_wait_ms(25);
	}

	// stop the motors
	ecrobot_set_motor_mode_speed(NXT_PORT_A, 1, 0);
	ecrobot_set_motor_mode_speed(NXT_PORT_B, 1, 0);
}

void log_line_found (char *direction, BOOL found, int light_delta, int rev_delta) {
	// Logging
	display_clear(1);
	display_goto_xy(0,0);
	display_string(direction);
	display_goto_xy(6,0);

	if (found)
		display_string("Line found!");
	else
		display_string("No line");

	// show light delta
	display_goto_xy(0,2);
	display_string("Light Delta:");
	display_goto_xy(1,3);
	display_int(light_delta, 3);

	// show revolution delta
	display_goto_xy(0,5);
	display_string("Rev Delta:");
	display_goto_xy(1,6);
	display_int(rev_delta, 5);

	display_update();
}

/*
 * This is the main loop of the control flow of the NXT
 */
TASK(OSEK_Main_Task) {

	BOOL ready = FALSE;

	int distance = 0;

	// while (!ready) {
		// center the NXT with left twist
		move_to_corssroad_center();

		// TODO was passiert bei negativen Werten?
		int start_revolution = ecrobot_get_motor_rev(NXT_PORT_A);
		int start_darkness = ecrobot_get_light_sensor(NXT_PORT_S1);
		int delta_light = 0;
		int delta_rev = 0;
		BOOL found_line = FALSE;
		rotate_left();

		// rotate max. 150°
		while (delta_rev < 150) {
			// hover a line
			if (delta_light > 100) {
				found_line = TRUE;
				break;
			} else {
				// Display current status
				display_status(start_darkness);
				systick_wait_ms(50);
			}
			delta_light = ecrobot_get_light_sensor(NXT_PORT_S1) - start_darkness;
			delta_rev = ecrobot_get_motor_rev(NXT_PORT_A) - start_revolution;

			// update current darkness for dynamic line detection
			start_darkness = ecrobot_get_light_sensor(NXT_PORT_S1);
		}
		stop_motors();

		log_line_found("Left", found_line, delta_light, delta_rev);

		// start rotating right back to the start position
		rotate_right();

		while (ecrobot_get_motor_rev(NXT_PORT_A) > start_revolution + 20) {
			// display_status(start_darkness);
			systick_wait_ms(25);
		}

		/* Search for the line in front of the NXT */

		// reset delta and revolution
		start_revolution = ecrobot_get_motor_rev(NXT_PORT_B);
		start_darkness = ecrobot_get_light_sensor(NXT_PORT_S1);
		delta_light = 0;
		delta_rev = 0;
		found_line = FALSE;

		// rotate max. 60° right
		while (delta_rev < 60) {

			// hover a line
			if (delta_light > 100) {
				found_line = TRUE;
				break;
			} else {
				// Display current status
				// display_status(start_darkness);
				systick_wait_ms(50);
			}
			delta_light = ecrobot_get_light_sensor(NXT_PORT_S1) - start_darkness;
			delta_rev = ecrobot_get_motor_rev(NXT_PORT_B) - start_revolution;

			// update current darkness for dynamic line detection
			start_darkness = ecrobot_get_light_sensor(NXT_PORT_S1);
		}

		// Logging
		log_line_found("Top", found_line, delta_light, delta_rev);

		// reset delta and revolution
		start_revolution = ecrobot_get_motor_rev(NXT_PORT_B);
		start_darkness = ecrobot_get_light_sensor(NXT_PORT_S1);
		delta_light = 0;
		delta_rev = 0;
		found_line = FALSE;

		/* Search for the line right to the NXT */
		while (delta_rev < 270) {
			// hover a line
			if (delta_light > 100) {
				found_line = TRUE;
				break;
			} else {
				// Display current status
				// display_status(start_darkness);
				systick_wait_ms(50);
			}
			delta_light = ecrobot_get_light_sensor(NXT_PORT_S1) - start_darkness;
			delta_rev = ecrobot_get_motor_rev(NXT_PORT_B) - start_revolution;

			// update current darkness for dynamic line detection
			start_darkness = ecrobot_get_light_sensor(NXT_PORT_S1);
		}
		stop_motors();

		// Logging
		log_line_found("Right", found_line, delta_light, delta_rev);

		// Looking at the right line

	//}

	TerminateTask();
}
