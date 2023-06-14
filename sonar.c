/* #define SONAR_USE_BOTH_IT */

#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/exti.h"
#include "libopencm3/cm3/nvic.h"
#include "libopencm3/stm32/timer.h"

#include "sonar.h"
#include "delay.h"

int sonar_distance = SONAR_MAX_DISTANCE;

void sonar_setup(void)
{
        rcc_periph_clock_enable(RCC_GPIO_TRIG_PORT);
        rcc_periph_clock_enable(RCC_GPIO_ECHO_PORT);
        rcc_periph_clock_enable(RCC_TIM2);

        gpio_set_mode(GPIO_TRIG_PORT, GPIO_MODE_OUTPUT_50_MHZ,
                        GPIO_CNF_OUTPUT_PUSHPULL, GPIO_TRIG_PIN);
        gpio_set_mode(GPIO_ECHO_PORT, GPIO_MODE_INPUT,
                        GPIO_CNF_INPUT_PULL_UPDOWN, GPIO_ECHO_PIN);

        /* gpio_clear(GPIO_TRIG_PORT, GPIO_TRIG_PIN); */

        exti_select_source(EXTI1, GPIO_ECHO_PORT);

#ifdef SONAR_USE_BOTH_IT
        exti_set_trigger(EXTI1, EXTI_TRIGGER_BOTH);
#else
        exti_set_trigger(EXTI1, EXTI_TRIGGER_RISING);
#endif

        timer_set_prescaler(TIM2, (rcc_apb1_frequency*2)/SONAR_TIM2_FREQ + 1);
        timer_set_period(TIM2, 0xffff);
        timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
                        TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
        timer_enable_preload(TIM2);

        nvic_set_priority(NVIC_EXTI1_IRQ, 2);

        nvic_enable_irq(NVIC_EXTI1_IRQ);
        exti_enable_request(EXTI1);
}

void sonar_trigger(void)
{
        gpio_set(GPIO_TRIG_PORT, GPIO_TRIG_PIN);
        delay_us(12);
        gpio_clear(GPIO_TRIG_PORT, GPIO_TRIG_PIN);
}

#ifdef SONAR_USE_BOTH_IT
void exti1_isr(void)
{
        exti_reset_request(EXTI1);

        if (gpio_get(GPIO_ECHO_PORT, GPIO_ECHO_PIN)) {
                        timer_set_counter(TIM2, 0);
                        timer_enable_counter(TIM2);
        }

        if (!gpio_get(GPIO_ECHO_PORT, GPIO_ECHO_PIN)) {
                uint16_t cnt = timer_get_counter(TIM2);
                timer_disable_counter(TIM2);

                sonar_distance =
                        ((cnt*34000/SONAR_TIM2_FREQ / 2) < SONAR_MAX_DISTANCE)
                        ? (cnt * 34000 / SONAR_TIM2_FREQ / 2)
                        : SONAR_MAX_DISTANCE;
        }
}
#else
void exti1_isr(void)
{
        exti_reset_request(EXTI1);
        delay_us(20);

        timer_set_counter(TIM2, 0);
        timer_enable_counter(TIM2);

        while (gpio_get(GPIO_ECHO_PORT, GPIO_ECHO_PIN));

        timer_disable_counter(TIM2);
        uint16_t cnt = timer_get_counter(TIM2);

        sonar_distance =
                ((cnt*34000/SONAR_TIM2_FREQ / 2) < SONAR_MAX_DISTANCE)
                ? (cnt * 34000 / SONAR_TIM2_FREQ / 2)
                : SONAR_MAX_DISTANCE;
}
#endif

