/*
 * This program will search for tokens in a unknown maze.
 *
 *
 */

#include "../h/melody.h"

void melody_play_tone (int freq, int ms, int *dance_left) {
	// start dancing
	if (*dance_left) {
		*dance_left = 0;
		ecrobot_set_motor_mode_speed(MOTOR_RIGHT, 0, -70);
		ecrobot_set_motor_mode_speed(MOTOR_LEFT, 0, 70);
	} else {
		*dance_left = 1;
		ecrobot_set_motor_mode_speed(MOTOR_RIGHT, 0, 70);
		ecrobot_set_motor_mode_speed(MOTOR_LEFT, 0, -70);
	}
	ecrobot_sound_tone(freq, ms, VOLUME);
	systick_wait_ms(ms);

	// stop motors
	ecrobot_set_motor_mode_speed(MOTOR_LEFT, 1, 0);
	ecrobot_set_motor_mode_speed(MOTOR_RIGHT, 1, 0);

}

void melody_death () {
	int quarter = 150;
	int minim = 2 * quarter;
	int quaver = quarter / 2;
	int quaver_dotted = 1.5 * quaver;
	int demiquaver = quarter / 4;

	ecrobot_sound_tone(1046, demiquaver, 50); // C6
	systick_wait_ms(demiquaver);
	ecrobot_sound_tone(1108, demiquaver, 50); // Cis6
	systick_wait_ms(demiquaver);
	ecrobot_sound_tone(1174, quaver, 75); // Cis6
	systick_wait_ms(quaver);

	systick_wait_ms(3 * quarter); // pause

	ecrobot_sound_tone(988, quarter, 50); // H5
	systick_wait_ms(quarter);
	ecrobot_sound_tone(1397, quarter, 50); // F6
	systick_wait_ms(quarter);

	systick_wait_ms(quarter);

	ecrobot_sound_tone(1397, quarter, 50); // F6
	systick_wait_ms(quarter);

	int triole = 4/3 * quarter;

	ecrobot_sound_tone(1397, triole, 50); // F6
	systick_wait_ms(triole);
	ecrobot_sound_tone(1318, triole, 50); // E6
	systick_wait_ms(triole);
	ecrobot_sound_tone(1174, triole, 50); // D6
	systick_wait_ms(triole);

	ecrobot_sound_tone(1046, quarter, 50); // D6
	systick_wait_ms(quarter);
	ecrobot_sound_tone(659, quarter, 50); // E5
	systick_wait_ms(quarter);

	systick_wait_ms(quarter); // pause

	ecrobot_sound_tone(659, quarter, 50); // E5
	systick_wait_ms(quarter);

	ecrobot_sound_tone(523, quarter, 50); // C5
	systick_wait_ms(quarter);
}

void melody_indiana () {
	int quarter = 350;
	int minim = 2 * quarter;
	int quaver = quarter / 2;
	int quaver_dotted = 1.5 * quaver;
	int demiquaver = quarter / 4;
	int dance_left = 1;

	melody_play_tone(659, quaver_dotted, &dance_left); // E5
	melody_play_tone(698, demiquaver, &dance_left); // F5

	melody_play_tone(783, quaver, &dance_left); // G5
	melody_play_tone(1046, minim + quaver, &dance_left); // C6
	melody_play_tone(587, quaver_dotted, &dance_left); // D5
	melody_play_tone(659, demiquaver, &dance_left); // E5

	melody_play_tone(698, minim + quarter,&dance_left); // F5
	melody_play_tone(783, quaver_dotted, &dance_left); // G5
	melody_play_tone(880, demiquaver, &dance_left); // A5

	melody_play_tone(988, quaver, &dance_left); // H5
	melody_play_tone(1396, quaver + minim, &dance_left); // F6
	melody_play_tone(880, quaver_dotted, &dance_left); // A5
	melody_play_tone(988, demiquaver, &dance_left); // H5

	melody_play_tone(1046, quarter, &dance_left); // C6
	melody_play_tone(1175, quarter, &dance_left); // D6
	melody_play_tone(1319, quarter, &dance_left); // E6
	melody_play_tone(659, quaver_dotted, &dance_left); // E5
	melody_play_tone(698, demiquaver, &dance_left); // F5

	melody_play_tone(783, quaver, &dance_left); // G5
	melody_play_tone(1046, minim + quaver, &dance_left); // C6
	melody_play_tone(1175, quaver_dotted, &dance_left); // D6
	melody_play_tone(1319, demiquaver, &dance_left); // E6

	melody_play_tone(1396, quaver + minim, &dance_left); // F6
	melody_play_tone(783, quaver_dotted, &dance_left); // G5
	melody_play_tone(783, demiquaver, &dance_left); // G5

	melody_play_tone(1319, quarter, &dance_left); // E6
	melody_play_tone(1175, quaver_dotted, &dance_left); // D6
	melody_play_tone(783, demiquaver, &dance_left); // G5
	melody_play_tone(1319, quarter, &dance_left); // E6
	melody_play_tone(1175, quaver_dotted, &dance_left); // D6
	melody_play_tone(783, demiquaver, &dance_left); // G5

	melody_play_tone(1396, quarter, &dance_left); // F6
	melody_play_tone(1319, quaver_dotted, &dance_left); // E6
	melody_play_tone(1175, demiquaver, &dance_left); // D6
	melody_play_tone(1046, quarter, &dance_left); // C6
}
