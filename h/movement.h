#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "defines.h"
#include "ecrobot_interface.h"

/* function for driving forward */
void move_forward(int power);

/* function for turn left */
void rotate_left (int power);

/* function for turn right */
void rotate_right (int power);

/* function for stop */
void stop_motors();

/* drive forward for one rotation of the wheels */
void jump_forward();

#endif /* MOVEMENT_H_ */
