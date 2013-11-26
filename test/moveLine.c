#include <stdlib.h>
#include <string.h>
#include "kernel.h"
/* Will be generated: */
#include "kernel_id.h"
#include "ecrobot_interface.h"

// user defined interfaces
#include "../h/movement.h"
#include "../h/display.h"

int threshold = 30;
int radius = 90;

/* nxtOSEK hook to be invoked from an ISR in category 2 */
void user_1ms_isr_type2(void) {
}

/*
 *
 */
void ecrobot_device_initialize(void) {
	ecrobot_set_light_sensor_active(NXT_PORT_S1);
}

void ecrobot_device_terminate(void) {
	ecrobot_set_light_sensor_inactive(NXT_PORT_S1);
	stop_motors();
}

BOOL search_line () {
	/*
	 * Turn right
	 */

	// stop left
	ecrobot_set_motor_mode_speed(NXT_PORT_A, 1, 0);
	// activate right
	ecrobot_set_motor_mode_speed(NXT_PORT_B, 0, 60);

	// store current sensor and revolution state
	int light = ecrobot_get_light_sensor(NXT_PORT_S1);
	int rev_start = ecrobot_get_motor_rev(NXT_PORT_B);

	while (ecrobot_get_motor_rev(NXT_PORT_B) - rev_start < radius) {
		int delta_light = ecrobot_get_light_sensor(NXT_PORT_S1) - light;

		display_action("Search Right");
		display_light_delta(delta_light);
		display_revolution_delta(ecrobot_get_motor_rev(NXT_PORT_B) - rev_start);

		// line hover detected
		if (delta_light > threshold) {
			return TRUE;
		}

		// update light sensor TODO
		// light = ecrobot_get_light_sensor(NXT_PORT_S1);
		systick_wait_ms(100);
	}

	/*
	 * Turn left
	 */

	// activate right motor
	ecrobot_set_motor_mode_speed(NXT_PORT_A, 0, 60);
	// stop left
	ecrobot_set_motor_mode_speed(NXT_PORT_B, 0, -62);

	// save current revolution and light density state
	light = ecrobot_get_light_sensor(NXT_PORT_S1);
	rev_start = ecrobot_get_motor_rev(NXT_PORT_A);

	while (ecrobot_get_motor_rev(NXT_PORT_A) - rev_start < radius) {
		int delta_light = ecrobot_get_light_sensor(NXT_PORT_S1) - light;

		display_action("Search Left");
		display_light_delta(delta_light);
		display_revolution_delta(ecrobot_get_motor_rev(NXT_PORT_A) - rev_start);

		// line hover detected
		if (delta_light > 50) {
			return TRUE;
		}

		// update light sensor TODO
		// light = ecrobot_get_light_sensor(NXT_PORT_S1);
		systick_wait_ms(100);
	}

	/*
	 * Center the NXT again
	 */


	// stop right
	ecrobot_set_motor_mode_speed(NXT_PORT_A, 0, -60);
	// activate left motor
	ecrobot_set_motor_mode_speed(NXT_PORT_B, 0, 60);
	// save current revolution state for delta computing
	rev_start = ecrobot_get_motor_rev(NXT_PORT_B);

	while (ecrobot_get_motor_rev(NXT_PORT_B) - rev_start < 0.5 * radius) {
		display_action("No Line. Center");
		display_revolution_delta(ecrobot_get_motor_rev(NXT_PORT_B) - rev_start);
		systick_wait_ms(50);
	}

	return FALSE;
}

/*
 * This function navigates the NXT straight forward on a line. If token is found,
 * found_token() is called.
 */
void follow_line () {
	BOOL line_end_reached = FALSE;

	int shadow_before = ecrobot_get_light_sensor(NXT_PORT_S1);
	int delta_max = 0;

	while (!line_end_reached) {
		// delta will be positive if the sensor leaves the line
		int light_delta = shadow_before - ecrobot_get_light_sensor(NXT_PORT_S1);

		display_action("Moving forward");
		if (light_delta > delta_max)
			delta_max = light_delta;
		display_light_delta(delta_max);

		// loose line detected. Try to find the line again
		if (light_delta > threshold) {
			// cannot find the line. Expect end of line
			if (!search_line()) {
				line_end_reached = TRUE;
				break;
			}
			delta_max = 0;
			shadow_before = ecrobot_get_light_sensor(NXT_PORT_S1);
		}

		// update current light density
		shadow_before = ecrobot_get_light_sensor(NXT_PORT_S1);

		// move forwards
		ecrobot_set_motor_mode_speed(NXT_PORT_B, 0, 65);

		// if left is father than right, trim the right motor
		if (ecrobot_get_motor_rev(NXT_PORT_A) > ecrobot_get_motor_rev(NXT_PORT_B)) {
			ecrobot_set_motor_mode_speed(NXT_PORT_A, 0, 60);
		} else {
			ecrobot_set_motor_mode_speed(NXT_PORT_A, 0, 65);
		}


		// continue moving forward for 100ms
		// 125
		// 100 => 47
		// 50 => Schwellwert von 29
		// 25 => Schwellwert von 15
		systick_wait_ms(125);
	}
	display_action("Reached EOL");

	stop_motors();
}

/*
 * This is the main loop of the control flow of the NXT
 */
TASK(OSEK_Main_Task) {
	TerminateTask();
}
