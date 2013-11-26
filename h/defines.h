/*
 * Central collection of "#define" directives.
 *
 *  Created on:	11.03.2013
 *  Author: Lucas Kahlert
 */
#ifndef DEFINES_H_
#define DEFINES_H_

/* -----------------------------------------------------------------------
 * Global port definitions
 * ----------------------------------------------------------------------- */
#define MOTOR_RIGHT NXT_PORT_A
#define MOTOR_LEFT  NXT_PORT_B

#define LAMP NXT_PORT_C

#define SENSOR_LIGHT NXT_PORT_S2
#define SENSOR_TOUCH_1 NXT_PORT_S1
#define SENSOR_TOUCH_2 NXT_PORT_S3

/* -----------------------------------------------------------------------
 * Global settings
 * ----------------------------------------------------------------------- */
#define MAX_TOKEN_COUNT 3
#define MIN_LINE_DISTANCE 450

#endif /* DEFINES_H_ */
