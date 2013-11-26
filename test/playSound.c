#include <stdlib.h>
#include "kernel.h"
/* Will be generated: */
#include "kernel_id.h"
#include "ecrobot_interface.h"

/* nxtOSEK hook to be invoked from an ISR in category 2 */
void user_1ms_isr_type2(void) {
}

void ecrobot_device_initialize(void) {}

void ecrobot_device_terminate(void) {}

TASK(OSEK_Main_Task) {
	while (1) {
		ecrobot_sound_tone(2093, 250, 10);
		systick_wait_ms(250);
		ecrobot_sound_tone(2093, 250, 25);
		systick_wait_ms(250);
		ecrobot_sound_tone(2093, 250, 50);
		systick_wait_ms(250);
		ecrobot_sound_tone(2093, 250, 25);
		systick_wait_ms(250);
	}
}
