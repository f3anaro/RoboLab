#include "../h/movement.h"

/*
 * The NXT will drive straight forward.
 */
void move_forward (int power) {
	ecrobot_set_motor_mode_speed(MOTOR_LEFT, 0, power);
	ecrobot_set_motor_mode_speed(MOTOR_RIGHT, 0, power);
}

/*
 * The NXT starts rotating left
 */
void rotate_left (int power) {
	ecrobot_set_motor_mode_speed(MOTOR_LEFT, 0, -power);
	ecrobot_set_motor_mode_speed(MOTOR_RIGHT, 0, power);
}

/*
 * The NXT start rotating right side
 */
void rotate_right (int power) {
	ecrobot_set_motor_mode_speed(MOTOR_LEFT, 0, power);
	ecrobot_set_motor_mode_speed(MOTOR_RIGHT, 0, -power);
}

/*
 * The NXT will stop all movings.
 */
void stop_motors () {
	ecrobot_set_motor_mode_speed(MOTOR_LEFT, 1, 0);
	ecrobot_set_motor_mode_speed(MOTOR_RIGHT, 1, 0);
}

/*
 * Drive forward for the committed revolution of the wheels
 * with a specific power.
 * After that the NXT stops.
 */
void jump_forward (int rev, int power) {
	int rev_target = ecrobot_get_motor_rev(MOTOR_RIGHT) + rev;

	while (ecrobot_get_motor_rev(MOTOR_RIGHT) < rev_target) {
		ecrobot_set_motor_mode_speed(MOTOR_LEFT, 0, power);
		ecrobot_set_motor_mode_speed(MOTOR_RIGHT, 0, power);
	}
	stop_motors();
}
