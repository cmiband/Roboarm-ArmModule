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

#define SERVO_MIN 550
#define SERVO_MAX 2430
#define ANGLE_MIN 0
#define ANGLE_MAX 180

typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t channel;

    uint16_t stop;
    uint16_t deadband;

    uint16_t min;
    uint16_t max;
} servo_t;


void servoInit(servo_t *servo, TIM_HandleTypeDef *_htim, uint32_t _channel, uint16_t stop, uint16_t deadband, uint16_t min, uint16_t max);
void servoStart(servo_t *servo);
void servoStop(servo_t *servo);

void servoSetSpeed(servo_t *servo, int16_t speed);
void servoSetStop(servo_t *servo, uint16_t stop);
void servoSetDeadband(servo_t *servo, uint16_t deadband);

#endif /* INC_SERVO_H_ */
