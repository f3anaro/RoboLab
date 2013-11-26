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
	BOOL light_sensor_active = FALSE;
	SENSOR_PORT_T port_light_sensor = NXT_PORT_S1;
	SENSOR_PORT_T port_touch_sensor = NXT_PORT_S2;

	ecrobot_set_light_sensor_active(port_light_sensor);

	while (1) {
		display_goto_xy(0,0);
		display_string("Touch Sensor:");
		display_goto_xy(0,2);

		if (ecrobot_get_touch_sensor(port_touch_sensor)) {
			display_string("Touch!");
			ecrobot_sound_tone(2093, 100, 2093);
		} else {
			display_string("No touch");
		}
		display_goto_xy(0,4);
		display_string("Light Sensor:");
		display_goto_xy(0,6);
		display_int(ecrobot_get_light_sensor(port_light_sensor), 4);

		display_update();
		systick_wait_ms(100);
	}
}
