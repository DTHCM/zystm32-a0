#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>

#define MOTOR_TIM4_FREQ 50000
#define MOTOR_TIM4_PEROID 1000

enum dir {
        MOTOR_DIR_FORWORD, MOTOR_DIR_REVERSE,
};

typedef void (*motor_controller)(uint16_t, enum dir);

typedef struct _motor_t {
        uint16_t speed;
        enum dir dir;
        motor_controller controller;
} motor_t;

extern motor_t motor_left, motor_right;


void motor_setup(void);
void motor_init(motor_t *motor, motor_controller controller);
void motor_set(motor_t *motor, uint16_t speed, enum dir dir);
/*
 * @param speed - Motor speed percentage, raning from 0 to 100.
 */
void motor_ctrl(motor_t *motor, uint16_t speed, enum dir dir);
void motor_reverse(motor_t *motor);
void motor_stop(motor_t *motor);
void motor_speedup(motor_t *motor, uint16_t speed);
void motor_speeddown(motor_t *motor, uint16_t speed);

static inline void motor_foreach_forward(uint16_t speed)
{
        motor_ctrl(&motor_left, speed, MOTOR_DIR_FORWORD);
        motor_ctrl(&motor_right, speed, MOTOR_DIR_FORWORD);
}

static inline void motor_foreach_reverse(uint16_t speed)
{
        motor_ctrl(&motor_left, speed, MOTOR_DIR_REVERSE);
        motor_ctrl(&motor_right, speed, MOTOR_DIR_REVERSE);
}

static inline void motor_foreach_stop(void)
{
        motor_stop(&motor_left);
        motor_stop(&motor_right);
}

#endif
