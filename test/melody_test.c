/*
 * This program will search for tokens in a unknown maze.
 *
 *
 */

// nxtOSEK headers
#include "kernel.h"
#include "kernel_id.h"	// Will be generated
#include "ecrobot_interface.h"

#include "../h/melody.h"

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
}

/*
 * If the ShutDown-Hook in the main.oil is set to TRUE, this
 * function will be executed on shutdown.
 */
void ecrobot_device_terminate(void) {
}

/*
 * This is the main loop of the control flow of the NXT. It will
 * be executed until all tokens are found or all nodes in the maze
 * are visited.
 */
TASK(OSEK_Main_Task) {
	melody_indiana();
//	melody_death();
	TerminateTask();
}
