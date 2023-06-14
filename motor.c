#include "motor.h"

#include "libopencm3/stm32/timer.h"
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/gpio.h"

motor_t motor_left, motor_right;

static void motor_left_controller(uint16_t speed, enum dir dir)
{
        uint16_t oc_value;
        if (dir == MOTOR_DIR_FORWORD)
                oc_value = MOTOR_TIM4_PEROID \
                           - (speed * MOTOR_TIM4_PEROID / 100);
        else
                oc_value = speed * MOTOR_TIM4_PEROID / 100;

        if (oc_value == 0)
                oc_value = 1;
        if (oc_value == MOTOR_TIM4_PEROID)
                oc_value -= 1;

        timer_set_oc_value(TIM4, TIM_OC3, oc_value);

        if (dir == MOTOR_DIR_FORWORD)
                gpio_clear(GPIOB, GPIO7);
        else
                gpio_set(GPIOB, GPIO7);
}

static void motor_right_controller(uint16_t speed, enum dir dir)
{
        uint16_t oc_value;
        if (dir == MOTOR_DIR_FORWORD)
                oc_value = MOTOR_TIM4_PEROID \
                           - (speed * MOTOR_TIM4_PEROID / 100);
        else
                oc_value = speed * MOTOR_TIM4_PEROID / 100;

        if (oc_value == 0)
                oc_value = 1;
        if (oc_value == MOTOR_TIM4_PEROID)
                oc_value -= 1;

        timer_set_oc_value(TIM4, TIM_OC4, oc_value);

        if (dir == MOTOR_DIR_FORWORD)
                gpio_clear(GPIOA, GPIO4);
        else
                gpio_set(GPIOA, GPIO4);
}

void motor_setup(void)
{
        rcc_periph_clock_enable(RCC_GPIOA);
        rcc_periph_clock_enable(RCC_GPIOB);

        rcc_periph_clock_enable(RCC_AFIO);

        rcc_periph_clock_enable(RCC_TIM4);

        /* Left motor
         * PB7 general GPIO, PB8 PWM.
         */
        gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                        GPIO_CNF_OUTPUT_PUSHPULL, GPIO7);
        gpio_clear(GPIOB, GPIO7);
        gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                        GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_TIM4_CH3);
        /* Right motor
         * PA4 general GPIO, PB9 PWM.
         */
        gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                        GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);
        gpio_clear(GPIOA, GPIO4);
        gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                        GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_TIM4_CH4);
        
        timer_set_prescaler(TIM4, (rcc_apb1_frequency*2)/MOTOR_TIM4_FREQ + 1);
        timer_set_period(TIM4, MOTOR_TIM4_PEROID);
        timer_set_mode(TIM4, TIM_CR1_CKD_CK_INT,
                        TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

        /* Left motor, PB8 */
        timer_set_oc_mode(TIM4, TIM_OC3, TIM_OCM_PWM2);
        timer_set_oc_polarity_high(TIM4, TIM_OC3);
        timer_enable_oc_preload(TIM4, TIM_OC3);
        timer_set_oc_value(TIM3, TIM_OC3, 0xffff);
        /* Right motor, PB9 */
        timer_set_oc_mode(TIM4, TIM_OC4, TIM_OCM_PWM2);
        timer_set_oc_polarity_high(TIM4, TIM_OC4);
        timer_enable_oc_preload(TIM4, TIM_OC4);
        timer_set_oc_value(TIM3, TIM_OC4, 0xffff);

        timer_enable_oc_output(TIM4, TIM_OC3);
        timer_enable_oc_output(TIM4, TIM_OC4);

        timer_enable_preload(TIM4);
        timer_enable_counter(TIM4);

        motor_init(&motor_left, &motor_left_controller);
        motor_init(&motor_right, &motor_right_controller);
}

void motor_init(motor_t *motor, motor_controller controller)
{
        motor->speed = 0;
        motor->dir = MOTOR_DIR_FORWORD;
        motor->controller = controller;
}

void motor_set(motor_t *motor, uint16_t speed, enum dir dir)
{
        if (speed > 100)
                speed = 100;
        motor->speed = speed;

        motor->dir = dir;
}

void motor_ctrl(motor_t *motor, uint16_t speed, enum dir dir)
{
        motor_set(motor, speed, dir);
        motor->controller(speed, dir);
        
}

void motor_reverse(motor_t *motor)
{
        motor_ctrl(motor, motor->speed,
                        (motor->dir == MOTOR_DIR_FORWORD)
                        ? MOTOR_DIR_REVERSE
                        : MOTOR_DIR_FORWORD
                  );
}

void motor_stop(motor_t *motor)
{
        motor_ctrl(motor, 0, motor->dir);
}
