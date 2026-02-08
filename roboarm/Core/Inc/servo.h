/*
 * servo.h
 *
 *  Created on: Feb 3, 2026
 *      Author: Bartek
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_

#include "tim.h"
#include <stdint.h>

#define SERVO_UNUSED_PARAMETER 1

#define SERVO_360_IMPULSE_STOP 1500
#define SERVO_360_IMPULSE_DEADBAND 20
#define SERVO_360_IMPULSE_MIN 1000
#define SERVO_360_IMPULSE_MAX 2000

#define SERVO_180_ANGLE_MIN 0
#define SERVO_180_ANGLE_MAX 180
#define SERVO_180_IMPULSE_MIN 450
#define SERVO_180_IMPULSE_MAX 2550

enum ServoType {
	SERVO_180,
	SERVO_360
};

typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t channel;

    uint16_t stop;
    uint16_t deadband;

    uint16_t min;
    uint16_t max;

    enum ServoType type;
} servo_t;


void servoInit(servo_t *servo, TIM_HandleTypeDef *_htim, uint32_t _channel, enum ServoType type);
void servoStart(servo_t *servo);
void servoStop(servo_t *servo);

void servoSetSpeed(servo_t *servo, int16_t speed);
void servoSetStop(servo_t *servo, uint16_t stop);
void servoSetDeadband(servo_t *servo, uint16_t deadband);

void servoSetRotation(servo_t *servo, int16_t angle);

#endif /* INC_SERVO_H_ */
