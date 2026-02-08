/*
 * servo.c
 *
 *  Created on: Feb 3, 2026
 *      Author: Bartek
 */

#include "main.h"
#include "tim.h"
#include "servo.h"
#include <stdbool.h>

static inline int16_t clampi16(int16_t x, int16_t lo, int16_t hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

static inline bool isServo360Type(servo_t *servo) {
	return servo->type == SERVO_360;
}

static inline uint16_t u16AbsDiff(uint16_t a, uint16_t b) {
    return (a > b) ? (a - b) : (b - a);
}

static uint16_t mapCcrFromAngle(int16_t angle, uint16_t minImpulse, uint16_t maxImpulse, uint16_t maxAngle) {
	uint16_t range = SERVO_180_IMPULSE_MAX - SERVO_180_IMPULSE_MIN;
	float fraction = (float)((uint16_t)angle)/(float)SERVO_180_ANGLE_MAX;

	return SERVO_180_IMPULSE_MIN+(uint16_t)(fraction*(float)range);
}

static void setCompare(servo_t *servo, uint16_t ccr) {
	__HAL_TIM_SET_COMPARE(servo->htim, servo->channel, ccr);
}

void servoInit(servo_t *servo, TIM_HandleTypeDef *htim, uint32_t channel, enum ServoType type)
{
	servo->htim = htim;
	servo->channel = channel;

    bool is360Servo = type == SERVO_360;
	servo->stop = is360Servo ? SERVO_360_IMPULSE_STOP : SERVO_UNUSED_PARAMETER;
	servo->deadband = is360Servo ? SERVO_360_IMPULSE_DEADBAND : SERVO_UNUSED_PARAMETER;

	servo->min = is360Servo ? SERVO_360_IMPULSE_MIN : SERVO_180_IMPULSE_MIN;
	servo->max = is360Servo ? SERVO_360_IMPULSE_MAX : SERVO_180_IMPULSE_MAX;

	servo->type = type;
}

void servoStart(servo_t *servo) {
	HAL_TIM_PWM_Start(servo->htim, servo->channel);

	//to set CCR value of timer to stop, default value might be something else and that may trigger some sort
	//of "jump"

	if(isServo360Type(servo)) {
		servoStop(servo);
	}
}

void servoStop(servo_t *servo) {
	setCompare(servo, servo->stop);
}

void servoSetStop(servo_t *servo, uint16_t stop) {
	servo->stop = stop;
}

void servoSetDeadband(servo_t *servo, uint16_t deadband) {
	servo->deadband = deadband;
}

void servoSetRotation(servo_t *servo, int16_t angle) {
	if(isServo360Type(servo)) {
		return;
	}

	angle = clampi16(angle, SERVO_180_ANGLE_MIN, SERVO_180_ANGLE_MAX);
	uint16_t ccr = mapCcrFromAngle(angle, servo->min, servo->max, SERVO_180_ANGLE_MAX);

	setCompare(servo, ccr);
}

void servoSetSpeed(servo_t *servo, int16_t speed) {
	if(!isServo360Type(servo)) {
		return;
	}

	speed = -clampi16(speed, -100, 100);

	if(speed == 0) {
		servoStop(servo);
		return;
	}

	uint16_t ccr = servo->stop;

	if (speed > 0) {
		uint16_t hStart = servo->stop + servo->deadband;
		ccr = (uint16_t)(hStart+(uint16_t)(speed-1) * (servo->max - hStart)/99u);
	} else {
		uint16_t lStart = (servo->stop > servo->deadband) ? (servo->stop - servo->deadband) : 0;
		int16_t mag = (int16_t)(-speed);

		ccr = (uint16_t)(lStart-(uint32_t)(mag-1) * (lStart - servo->min)/99u);
	}

	if(ccr < servo->min) ccr = servo->min;
	if(ccr > servo->max) ccr = servo->max;

	setCompare(servo, ccr);
}
