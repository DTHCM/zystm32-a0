#ifndef _SONAR_H
#define _SONAR_H

#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/gpio.h"

#define SONAR_MAX_DISTANCE 400               /* in cm */
#define SONAR_TIM2_FREQ 10000

#define RCC_GPIO_TRIG_PORT RCC_GPIOC
#define RCC_GPIO_ECHO_PORT RCC_GPIOC

#define GPIO_TRIG_PORT GPIOC
#define GPIO_ECHO_PORT GPIOC

#define GPIO_TRIG_PIN GPIO0
#define GPIO_ECHO_PIN GPIO1

extern int sonar_distance;

void sonar_setup(void);
void sonar_trigger(void);

#endif
