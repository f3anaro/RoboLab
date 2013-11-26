#include <stdlib.h>
#include "kernel.h"
/* Will be generated: */
#include "kernel_id.h"
#include "ecrobot_interface.h"

/* nxtOSEK hook to be invoked from an ISR in category 2 */
void user_1ms_isr_type2(void) {
}

void ecrobot_device_initialize(void) {
	ecrobot_set_light_sensor_active(NXT_PORT_S1);
}

void ecrobot_device_terminate(void) {
	ecrobot_set_light_sensor_inactive(NXT_PORT_S1);
}

TASK(OSEK_Main_Task) {
	ecrobot_set_motor_mode_speed(NXT_PORT_A, 0, 65);
	ecrobot_set_motor_mode_speed(NXT_PORT_B, 0, -65);

	int max_value = 0;

	while (1) {
		int light1 = ecrobot_get_light_sensor(NXT_PORT_S1);
		systick_wait_ms (125);
		int light2 = ecrobot_get_light_sensor(NXT_PORT_S1);
		int differenz = light2 - light1;

		if (differenz > max_value ) {
			max_value = differenz;

		}


		display_clear(1);
		display_goto_xy(0,0);
		display_string("DIFFERENZ");
		display_goto_xy(1,2);
		display_int( differenz, 4);

		display_goto_xy(0,3);
		display_string("MAX VALUE");
		display_goto_xy(1,4);
		display_int(max_value, 4);

		display_update();

	}

}
