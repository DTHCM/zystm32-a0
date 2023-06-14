#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "delay.h"

void delay_setup(void)
{
	/* set up a microsecond free running timer for ... things... */
	rcc_periph_clock_enable(RCC_TIM3);
	/* microsecond counter */
	timer_set_prescaler(TIM3, (rcc_apb1_frequency*2)/1000000 - 1);
	timer_set_period(TIM3, 0xffff);
	timer_one_shot_mode(TIM3);
}

void delay_us(uint16_t us)
{
	TIM_ARR(TIM3) = us;
	TIM_EGR(TIM3) = TIM_EGR_UG;
	TIM_CR1(TIM3) |= TIM_CR1_CEN;
	timer_enable_counter(TIM3);
	while (TIM_CR1(TIM3) & TIM_CR1_CEN);
}

void delay_ms(int ms)
{
        for (int i = 0; i < ms; i++)
                delay_us(1000);
}
