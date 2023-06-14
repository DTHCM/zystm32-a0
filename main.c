#include "libopencm3/stm32/rcc.h"
#include "libopencm3/cm3/systick.h"
#include "libopencm3/cm3/nvic.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/usart.h"
#include "libopencm3/stm32/dma.h"

#include <stdio.h>

#include "delay.h"
#include "motor.h"
#include "sonar.h"
#include "bluetooch.h"

char rx[6] = "ONN\r\n";
int rx_len = 3;
char tx[9] = "ONNEND\r\n";
int tx_len = 9;
void parse_rx(void);

void avoid(void);

volatile int do_avoid = 1;

int main(void)
{
        rcc_clock_setup_in_hse_8mhz_out_72mhz();
        
        delay_setup();

        motor_setup();
        motor_stop(&motor_left);
        motor_stop(&motor_right);

        bt_setup();
        sonar_setup();

        /* sys tick setup */
        systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
        systick_set_reload(rcc_ahb_frequency / 1000); // 1ms
        systick_interrupt_enable();
        systick_counter_enable();

        motor_foreach_stop();

        bt_transfered = 0;
        bt_dma_write(tx, tx_len);
        bt_received = 0;
        bt_dma_read(rx, rx_len);

        while (1) {
                if (bt_received) {
                        tx[0] = rx[0];
                        tx[1] = rx[1];
                        tx[2] = rx[2];

                        parse_rx();
                        bt_received = 0;
                        bt_dma_read(rx, rx_len);

                }
                if (do_avoid)
                        avoid();
        }

        return 0;
}

void sys_tick_handler(void)
{
        static uint16_t cnt = 0;
        cnt++;
        if (cnt == 100) {
                sonar_trigger();
                cnt = 0;
        }
}

void parse_rx(void)
{
        /* bt_transfered = 0; */
        /* bt_dma_write(tx, tx_len); */

        if (!(rx[0] == 'O' && rx[1] == 'N'))
		return ;

        switch (rx[2])
        {

                case '1':
                        do_avoid = 0;
                        motor_foreach_stop();
                        break;
                case '2':
                        do_avoid = 1;
                        motor_foreach_stop();
                        break;
                default:
                        break;
        }

        if (do_avoid)
                return ;

        switch (rx[2])
        {
                case 'A':
                        motor_foreach_forward(50);
                        break;
                case 'B':
                        motor_foreach_reverse(50);
                        break;
                case 'C': /* turn left */
                        motor_ctrl(&motor_right, 50,
                                        MOTOR_DIR_FORWORD);
                        motor_stop(&motor_left);
                        break;
                case 'D': /* turn right */
                        motor_ctrl(&motor_left, 50,
                                        MOTOR_DIR_FORWORD);
                        motor_stop(&motor_right);
                        break;
                case 'E':
                        motor_foreach_stop();
                        break;
                case 'F':
                        motor_foreach_stop();
                        break;
                default:
                        break;
        }
}

void avoid(void)
{
        if (sonar_distance < 40) {
                motor_foreach_stop();
                delay_ms(300);
                motor_foreach_reverse(50);
                delay_ms(400);
                motor_ctrl(&motor_right, 50, MOTOR_DIR_FORWORD);
                motor_stop(&motor_left);
                delay_ms(500);
        }
        motor_foreach_forward(50);
}
