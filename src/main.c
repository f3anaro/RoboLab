/*
 * This program will search for tokens in a unknown maze.
 *
 *
 */

#include "../h/main.h"

// node for search cancel callback "callback_from_equals_to"
node_t *__shortest_path_target = NULL;

// limits for edge detection
int hysteresis_low;
int hysteresis_high;

// 0 signals no line, 1 on line
int hysteresis_state;

// number of token being already found
int count_token = 0;

/*
 * nxtOSEK hook to be invoked from an ISR in category 2
 */
void user_1ms_isr_type2(void) {

}

/*
 * If the StartUp-Hook in the main.oil is set to TRUE, this
 * function will be executed on startup.
 */
void ecrobot_device_initialize(void) {
	ecrobot_set_light_sensor_active(SENSOR_LIGHT);
	count_token = 0;
}

/*
 * If the ShutDown-Hook in the main.oil is set to TRUE, this
 * function will be executed on shutdown.
 */
void ecrobot_device_terminate(void) {
	// turn of the IR-light
	ecrobot_set_light_sensor_inactive(SENSOR_LIGHT);
	// stop all servo-engines
	stop_motors();
	// turn of the light
	ecrobot_set_motor_mode_speed(LAMP, 1, 0);
}

void calculate_hysteresis (int max, int min) {
	int average = (max + min) / 2;

	// high level on 65%
	hysteresis_high = 1.1 * average;

	// low level on 55%
	hysteresis_low  = 0.9 * average;
}

void calibrate () {
	int max = 0;
	int min = 1023;
	int rev_start_right = ecrobot_get_motor_rev(MOTOR_RIGHT);

	rotate_left(67);

	display_action("Calibrate...");

	while (ecrobot_get_motor_rev(MOTOR_RIGHT) < rev_start_right + 780) {
		int light = ecrobot_get_light_sensor(SENSOR_LIGHT);

		if (light > max)
			max = light;
		else if (light < min)
			min = light;
	}
	stop_motors();
	calculate_hysteresis(max, min);
	hysteresis_state = 1;
	systick_wait_ms(200);
}


int is_on_line () {
	int light = ecrobot_get_light_sensor(SENSOR_LIGHT);
	int result = 0;

	// definite not on line
	if (light < hysteresis_low) {
		result = 0;
	}

	// definite on line
	else if (light > hysteresis_high) {
		result = 1;
	}
	// save hysteresis_state
	hysteresis_state = result;
	return result;
}

int is_touched () {
	return (ecrobot_get_touch_sensor(SENSOR_TOUCH_1) || ecrobot_get_touch_sensor(SENSOR_TOUCH_2));
}

/*
 * Radius of the wheels
 */
int search_line (int radius) {
	int power = 65;
	int target;
	MOTOR_PORT_T motor;

	if (radius < 0) {
		display_action("Search right");
		target = ecrobot_get_motor_rev(MOTOR_LEFT) - radius;
		motor = MOTOR_LEFT;
		rotate_right(power);
	} else {
		display_action("Search left");
		target = ecrobot_get_motor_rev(MOTOR_RIGHT) + radius;
		motor = MOTOR_RIGHT;
		rotate_left(power);
	}

	while (ecrobot_get_motor_rev(motor) < target) {
		if (is_on_line()) {
			stop_motors();
			return 1;
		}
	}
	stop_motors();
	return 0;
}

/*
 * This function navigates the NXT straight forward on a line. If token is found,
 * found_token() is called.
 */
void follow_line () {
	// signalisiert, dass die linke Kante verlassen wurde
	int leave_left = 1;
	int eol = 0;
	int power = 75;
	int rev_right_init = ecrobot_get_motor_rev(MOTOR_RIGHT);
	display_action("Follow line");

	while (!eol) {
		// token handling
		if (is_touched()) {
			found_token();
		}

		// Kante wurde verlassen
		if (!is_on_line()) {
			// Logging
			display_on_line(0);
			stop_motors();

			int radius = 30;
			int rev_start_right = ecrobot_get_motor_rev(MOTOR_RIGHT);
			int distance = rev_start_right - rev_right_init;

			// linke Kante wurde verlassen
			if (leave_left) {
				ecrobot_sound_tone(880, 50, 25);

				// turn right and search left edge
				if (search_line(-radius)) {
					leave_left = 0;

					ecrobot_set_motor_mode_speed(MOTOR_RIGHT, 0, power);
					ecrobot_set_motor_mode_speed(MOTOR_LEFT, 0, power+10);
				}
				// turn left and search right edge
				// right edge not found
				else if (!search_line(2 * radius)) {

					// Validate distance
					if (distance < MIN_LINE_DISTANCE) {
						if (!(search_line(-3 * radius) || search_line(4 * radius))) {
							// Logging
							display_action("Error #03");
							display_goto_xy(0,1);
							display_string("Loose line");
							display_update();

							// exit
							light_error_signal();
							TerminateTask();
						}
					} else {
						display_action("leave left");
						eol = 1;

						// center NXT
						rotate_right(65);

						// right wheel has moved to far
						while (ecrobot_get_motor_rev(MOTOR_RIGHT) > rev_start_right) {
							systick_wait_ms(25);
						}
						stop_motors();
						systick_wait_ms(200);
					}

				}
				// falls die rechte Kante gefunden wurde, war die Annahme, dass die
				// linke Kante verlassen wurde falsch. D.h. beim nächsten mal wird
				// die linke verlassen.
			}
			// rechte Kante wurde verlassen
			else {
				ecrobot_sound_tone(440, 50, 25);

				// turn left and search right edge
				if (search_line(radius)) {
					leave_left = 1;

					ecrobot_set_motor_mode_speed(MOTOR_RIGHT, 0, power+7);
					ecrobot_set_motor_mode_speed(MOTOR_LEFT, 0, power);

				}
				// turn right and search left edge
				else if (!search_line(-2 * radius)) {

					// Validate distance
					if (distance < MIN_LINE_DISTANCE) {
						if (!(search_line(3 * radius) || search_line(-4 * radius))) {
							// Logging
							display_action("Error #04");
							display_goto_xy(0,1);
							display_string("Loose line");
							display_update();

							// exit
							light_error_signal();
							TerminateTask();
						}
					} else {
						display_action("leave left");
						eol = 1;

						// center
						rotate_left(power);

						// right wheel has moved to far backwards
						while (ecrobot_get_motor_rev(MOTOR_RIGHT) < rev_start_right) {
							systick_wait_ms(25);
						}
						stop_motors();
						systick_wait_ms(200);
					}

				}
			}
		} else {
			display_on_line(1);
			move_forward(power);
		}


		// follow line
		// systick_wait_ms(25);
	}
	stop_motors();

	// wait 200ms after reaching end of line
	systick_wait_ms(200);
}

void center_crossover () {
	// move forward for 210°
	int rev = 205;
	int power = 70;

	int rev_target = ecrobot_get_motor_rev(MOTOR_RIGHT) + rev;

	while (ecrobot_get_motor_rev(MOTOR_RIGHT) < rev_target) {
		if (is_touched()) {
			stop_motors();
			found_token();
		} else {
			ecrobot_set_motor_mode_speed(MOTOR_LEFT, 0, power+3);
			ecrobot_set_motor_mode_speed(MOTOR_RIGHT, 0, power);
		}
	}
	stop_motors();
	systick_wait_ms(200);
}

int append_child_node (direction dir, node_t *cnt_node, l_list *nodes) {
	int x;
	int y;

	display_action("Append Child");
	display_goto_xy(0,1);

	switch (dir) {
		// North
		case 0:
			display_string("North");
			x = cnt_node->x;
			y = cnt_node->y + 1;
			break;
		// East
		case 1:
			display_string("East");
			x = cnt_node->x + 1;
			y = cnt_node->y;
			break;
		// South
		case 2:
			display_string("South");
			x = cnt_node->x;
			y = cnt_node->y - 1;
			break;
		// West
		case 3:
			display_string("West");
			x = cnt_node->x - 1;
			y = cnt_node->y;
			break;
		default:
			display_action("Unknown direction");
			light_error_signal();
			TerminateTask();
			return 0;
	}
	// search existing node
	node_t *child_node = search_node_by_xy(x,y, nodes);
	display_goto_xy(1,2);

	// node does not exist. Create it.
	if (child_node == NULL) {
		child_node = node_create(x,y);
		l_list_push(child_node, nodes);
		display_string("create node");
	} else {
		display_string("in list");
	}

	display_goto_xy(1,3);
	display_string("(");
	display_int(child_node->x,3);
	display_string(",");
	display_int(child_node->y,3);
	display_string(")");
	display_update();

	// the child node was already visited. Edge does already exist
	if (child_node->visited) {
		display_goto_xy(1,4);
		display_string("Already visited");
		display_update();
		// systick_wait_ms(7000);
		return 0;
	}

	display_goto_xy(1,4);
	display_string("Create new");
	display_update();

	// create edge between current node and the new child node
	edge_create(cnt_node, child_node, dir);
	// systick_wait_ms(7000);
	return 1;
}

void analyse_crossover (direction cnt_dir, node_t *cnt_node, l_list *nodes) {
	display_action("Analyze");

	// store current revolution state
	int rev_start_right = ecrobot_get_motor_rev(MOTOR_RIGHT);
	int rev_target = rev_start_right + 820;
	int power = 70;

	// rotate left side
	rotate_left(power);

	// degrees of the wheels the NXT will ignore before an edge is detected
	int rev_ignore_start = rev_start_right + 90;

	while (ecrobot_get_motor_rev(MOTOR_RIGHT) < rev_target) {
		int rev_right = ecrobot_get_motor_rev(MOTOR_RIGHT);
		int delta_rev = rev_right - rev_start_right;

		// if the last line and the current line are less than 30°-wheel-degree
		// away ignore it
		if (is_on_line() && rev_right > rev_ignore_start) {
			// play sound to signal: line was found
			ecrobot_sound_tone(880, 100, 25);
			stop_motors();

			// left
			if (delta_rev > 135 && delta_rev < 300) {
				// Logging
				display_goto_xy(1,1);
				display_string("Left");
				display_update();
				append_child_node((cnt_dir + 3) % 4, cnt_node, nodes);

			}
			// behind. Will be ignored for appending
			else if (delta_rev >= 300 && delta_rev < 500) {
				// Logging
				display_action("Analyze");
				display_goto_xy(1,2);
				display_string("Back");
				display_update();
			}
			// right
			else if (delta_rev >= 500 && delta_rev < 700) {
				// Logging
				display_goto_xy(1,3);
				display_string("Right");
				display_update();

				append_child_node((cnt_dir + 1) % 4, cnt_node, nodes);
			}
			// top. stop analyzing
			else if (delta_rev >= 700) {
				// Logging
				display_goto_xy(1,3);
				display_string("Top");
				display_update();

				append_child_node(cnt_dir, cnt_node, nodes);
				break;
			}
			// invalid. Will be logged but not canceled
			else {
				// Logging
				display_action("Invalid Line");
				display_revolution_delta(ecrobot_get_motor_rev(MOTOR_RIGHT) - rev_start_right);

				light_error_signal();
			}
			systick_wait_ms(200);
			light_set_inactive();
			rev_ignore_start = ecrobot_get_motor_rev(MOTOR_RIGHT) + 45;
			rotate_left(power);
		}
		// systick_wait_ms(25);
	}
	// stop rotating
	stop_motors();

	// mark node as visited. This node will never be analyzed again.
	cnt_node->visited = 1;

	// Logging the currently analyzed node
	display_action("Analyzed (");
	display_int(cnt_node->x, 3);
	display_string(",");
	display_int(cnt_node->y, 3);
	display_string(")");
	display_update();

	systick_wait_ms(200);
}

/*
 * Helper function to turn on the light
 */
void light_set_active () {
	ecrobot_set_motor_speed(LAMP, 100);
}

/*
 * Helper function to turn off the light
 */
void light_set_inactive () {
	ecrobot_set_motor_mode_speed(LAMP, 1, 0);
}

void found_token () {
	// increment the found tokens
	count_token++;
	stop_motors ();

	light_set_active();

	int duration = 100;

	// Signal
	ecrobot_sound_tone(988, duration, 75);
	systick_wait_ms(duration);
	ecrobot_sound_tone(1319, 4 * duration, 75);
	systick_wait_ms(4 * duration);

	// wait for 10s
	systick_wait_ms (10000);

	light_set_inactive();
}

void light_error_signal () {
	int i = 0;

	while (i < 10) {
		light_set_active();
		systick_wait_ms(100);
		light_set_inactive();
		systick_wait_ms(100);
		i++;
	}
}

void drive_turn (int left) {
	// current direction is "top"
	int rev_start;
	int power = 65;

	// initialize the pseudo random generator
	srand(systick_get_ms());

	MOTOR_PORT_T motor;

	// turn left
	if (left) {
		rev_start = ecrobot_get_motor_rev(MOTOR_RIGHT);
		motor = MOTOR_RIGHT;
		rotate_left(power);
	}
	// turn right
	else {
		rev_start = ecrobot_get_motor_rev(MOTOR_LEFT);
		motor = MOTOR_LEFT;
		rotate_right(power);
	}

	while (1) {
		if (is_on_line()) {
			int delta_rev = ecrobot_get_motor_rev(motor) - rev_start;

			// if the rotation was greater than 90° right line is found
			if (delta_rev > 90) {
				stop_motors();
				ecrobot_sound_tone(880, 200, 25);

				// ERROR
				if (delta_rev > 270) {
					display_action("Error #01");
					display_revolution_delta(delta_rev);

					stop_motors();
					light_error_signal();
					TerminateTask();
					return;
				}

				break;
			}
			// too close on top line
			else {
				light_set_active();
			}
		}
		// off line
		else {
			light_set_inactive();
		}
	}
	stop_motors();
	systick_wait_ms(200);
}

void drive_left () {
	drive_turn(1);
}

void drive_right () {
	drive_turn(0);
}

void drive_back () {
	// current position is top
	int rev_start_right = ecrobot_get_motor_rev(MOTOR_RIGHT);
	int rev_ignore = rev_start_right + 300;
	int power = 65;

	while (1) {
		rotate_left(power);

		if (is_on_line() && ecrobot_get_motor_rev(MOTOR_RIGHT) > rev_ignore) {
			ecrobot_sound_tone(880, 100, 25);
			break;
		}
	}
	stop_motors();
	// wait 1s after a line was found
	systick_wait_ms(200);
}

void drive_forward () {
	if (search_line(30))
		return;
	if (search_line(-60))
		return;

	// No line was found
	light_error_signal();
	TerminateTask();
}

void translate_direction (direction cnt_dir, direction new_dir) {
	// create module of 4 for the difference of the current direction an
	// committed direction. The +4 is necessary to avoid negative module
	int diff_mod = (cnt_dir - new_dir + 4) % 4;
	int radius = 30;

	// interpret the module of the difference
	switch (diff_mod) {
		case 0:
			// search top line
			if (!search_line(radius) && !search_line(-2 * radius)) {
				display_action("Error #04");
				display_goto_xy(0,1);
				display_string("Loose top line");
				display_update();
				light_error_signal();
				TerminateTask();
			}
			break;
		case 1:
			drive_left();
			break;
		case 2:
			drive_back();
			break;
		case 3:
			drive_right();
			break;
	}
}

void follow_route (route_t **route, node_t **cnt_node, direction *cnt_dir) {
	l_element *element = (*route)->first;

	// Logging the route target
	l_element *element_2 = element;
	node_t *target = *cnt_node;

	while (element_2 != NULL) {
		int new_dir = *((direction*) element_2->data);
		target = edge_get_related_node(target, target->edges[new_dir]);
		element_2 = element_2->next;
	}

	display_action("Follow route to");
	display_goto_xy(1,1);
	display_string("(");
	display_int(target->x, 3);
	display_string(",");
	display_int(target->y, 3);
	display_string(")");
	display_update();

	// drive to next route node
	while (element != NULL) {
		direction new_dir = *((direction*) element->data);

		// drive to the center of the crossover
		// when processing the first element the NXT is already centered
		if (element != (*route)->first) {
			center_crossover();
		}

		// orientate the NXT and store the new direction
		translate_direction(*cnt_dir, new_dir);

		// store new direction
		*cnt_dir = new_dir;

		// lock edge
		edge_t *edge = (*cnt_node)->edges[new_dir];
		edge->is_locked = 1;

		// store new current node
		(*cnt_node) = edge_get_related_node(*cnt_node, edge);

		// follow the line
		follow_line();

		// go to next node in the route
		element = element->next;
	}
	display_action("Route end");

	// destruct the route
	l_list_destruct(1, route);
}

void ready () {
	// Logging
	display_action("Ready");
	display_goto_xy(0,1);

	// Light signal
	light_set_active();

	if (count_token >= MAX_TOKEN_COUNT) {
		display_string("All tokens found");
		display_update();
		melody_indiana();
	} else {
		display_string("All nodes");
		display_goto_xy(0,2);
		display_string("visited");
		display_update();
		melody_death();
	}
}


/*
 * This is the main loop of the control flow of the NXT. It will
 * be executed until all tokens are found or all nodes in the maze
 * are visited.
 */
TASK(OSEK_Main_Task) {
	calibrate();

	display_goto_xy(0,0);
	display_string("Hys_max");
	display_goto_xy(1,1);
	display_int(hysteresis_high, 3);
	display_goto_xy(0,2);
	display_string("Hys_min");
	display_goto_xy(1,3);
	display_int(hysteresis_low, 3);
	display_update();

	l_list *nodes = l_list_create();
	node_t *cnt_node = node_create(0,0);
	direction cnt_dir = 0;
	count_token = 0;

	l_list_push(cnt_node, nodes);

	// drive into maze
	follow_line();

	while (1) {
		// center the NXT on the crossover
		center_crossover();

		// all tokens are found
		if (count_token >= MAX_TOKEN_COUNT) {
			display_action("Drive home");
			// systick_wait_ms(5000);

			// search shortest way out of the maze
			__shortest_path_target = search_node_by_xy(0,0, nodes);
			route_t *route_to_home = shortest_path(cnt_node, callback_from_equals_to);

			// follow route if the current node is not (0,0)
			if (route_to_home != NULL) {
				follow_route(&route_to_home, &cnt_node, &cnt_dir);
			}
			// drive south
			center_crossover();
			translate_direction(cnt_dir, SOUTH);
			cnt_dir = SOUTH;
			follow_line();
			break;
		}

		// if the node was not visited yet, analyze it
		if (!cnt_node->visited) {

			// check if all node around the current node exist and are visited
			node_t *northern_node = search_node_by_xy(cnt_node->x, cnt_node->y + 1, nodes);
			node_t *eastern_node  = search_node_by_xy(cnt_node->x + 1, cnt_node->y, nodes);
			node_t *southern_node = search_node_by_xy(cnt_node->x, cnt_node->y - 1, nodes);
			node_t *western_node  = search_node_by_xy(cnt_node->x - 1, cnt_node->y, nodes);

			if (!northern_node || !northern_node->visited) {
				analyse_crossover(cnt_dir, cnt_node, nodes);
			} else if (!eastern_node || !eastern_node->visited) {
				analyse_crossover(cnt_dir, cnt_node, nodes);
			} else if (!southern_node || !southern_node->visited) {
				analyse_crossover(cnt_dir, cnt_node, nodes);
			} else if (!western_node || !western_node->visited) {
				analyse_crossover(cnt_dir, cnt_node, nodes);
			} else {
				// all neighbours was visited. Do not analyze this crossover
				cnt_node->visited = 1;
			}
		}

		// search for local (unvisited) edges
		if (node_has_unvisited_neighbors(cnt_node)) {
			display_action("Local search");
//			systick_wait_ms(2000);

			direction new_dir = select_random_direction(cnt_node);

			// orientate the NXT and store the new direction
			translate_direction(cnt_dir, new_dir);

			// store new absolute direction
			cnt_dir = new_dir;

			// lock the edge
			edge_t *edge = cnt_node->edges[new_dir];
			edge->is_locked = 1;

			// store new current node
			cnt_node = edge_get_related_node(cnt_node, edge);

			follow_line();
		}
		// search for global (unvisited) edges
		else {
			display_action("Global search");
//			systick_wait_ms(2000);

			l_list* route = shortest_path(cnt_node, callback_unvisited_neighbors);

			// No unvisited edges. Ready!
			if (route == NULL) {
				break;
			}
			// follow route to the node with unvisited
			else {
				follow_route(&route, &cnt_node, &cnt_dir);
			}
		}
	}
	// Ready signal and display
	ready();

	// Free allocated memory
	l_element *element = nodes->first;

	while (element != NULL) {
		// store temporary pointer to next element
		l_element *next = element->next;

		// cast node from list element
		node_t *node = (node_t*) element->data;
		element->data = NULL;

		// free node
		node_destruct(&node);

		// free list element
		free(element);

		// go to next element
		element = next;
	}
	free(nodes);

	TerminateTask();
}
