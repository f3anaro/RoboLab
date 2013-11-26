/*
 * Implementation Module for the display-interface
 */
#include "../h/display.h"

void display_action (char action[]) {
	display_clear(1);
	display_goto_xy(0,0);
	display_string(action);
	display_update();
}

void display_light () {
	display_goto_xy(0,2);
	display_string("Light:");
	display_goto_xy(1,3);
	display_int(ecrobot_get_light_sensor(SENSOR_LIGHT), 3);
	display_update();
}

void display_on_line (int state) {
	display_goto_xy(0,4);

	if (state) {
		display_string("On Line: true ");
	} else {
		display_string("On Line: false");
	}
	display_update();
}

void display_revolution_delta (int delta) {
	display_goto_xy(0,5);
	display_string("Rev:");
	display_goto_xy(1,6);
	display_int(delta, 4);
	display_update();
}

void display_crossover_stack (l_list *stack) {
	display_clear(1);

	l_element *element = stack->first;
	int i = 0;

	while (element != NULL && i < 8) {
		display_goto_xy(0, i);

		switch (*((int*) element->data)) {
			case 0:
				display_string("forwards");
				break;
			case 1:
				display_string("left");
				break;
			case 2:
				display_string("back");
				break;
			case 3:
				display_string("right");
				break;
			default:
				display_string("Unknown direction");
				break;
		}
		// go to next element
		element = element->next;
		i++;
	}
	display_update();
}
