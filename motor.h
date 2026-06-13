#ifndef __MOTOR_H
#define __MOTOR_H

#include "ti_msp_dl_config.h"

/* ---- Core API (matches motor/empty.c) ---- */

/* Convert 0-180 degree to timer CCP value (range: 9000 ~ 10000) */
uint16_t ang(uint8_t a);

/* Set all 4 servos by angle in degrees (FL, FR, BL, BR) */
void servo_set(uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3);

/* Initialize: all servos to 90° mid position */
void servo_zero(void);

/* Sit pose: front horizontal, rear vertical */
void sit(void);

/* Stand pose: all 90° */
void stand(void);

/* Continuous walking gait (diagonal trot) - call per loop iteration */
void robot_forward(void);

/* Continuous dance gait: rear sit pose, front legs alternate */
void dance(void);

/* Reset gait phase accumulator */
void reset_gait(void);

#endif
