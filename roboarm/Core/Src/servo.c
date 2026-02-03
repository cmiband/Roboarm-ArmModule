/*
 * servo.c
 *
 *  Created on: Feb 3, 2026
 *      Author: Bartek
 */

#include "main.h"
#include "tim.h"
#include "servo.h"

static inline int16_t clampi16(int16_t x, int16_t lo, int16_t hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

static inline float clampf(float x, float lo, float hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

static inline uint16_t u16AbsDiff(uint16_t a, uint16_t b) {
    return (a > b) ? (a - b) : (b - a);
}


static void setCompare(servo_t *servo, uint16_t ccr) {
	__HAL_TIM_SET_COMPARE(servo->htim, servo->channel, ccr);
}


void servoInit(servo_t *servo, TIM_HandleTypeDef *htim, uint32_t channel,
                   uint16_t stop, uint16_t deadband,
                   uint16_t min, uint16_t max)
{
	servo->htim = htim;
	servo->channel = channel;

	servo->stop = stop;
	servo->deadband = deadband;

	servo->min = min;
	servo->max = max;
}


void servoStart(servo_t *servo) {
	HAL_TIM_PWM_Start(servo->htim, servo->channel);

	//to set CCR value of timer to stop, default value might be something else and that may trigger some sort
	//of "jump"
	servoStop(servo);
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

void servoSetSpeed(servo_t *servo, int16_t speed) {
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
