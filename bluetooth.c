/* https://github.com/libopencm3/libopencm3-examples/blob/master/examples/stm32/f1/lisa-m-2/usart_dma/usart_dma.c#L62 */
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/usart.h"
#include "libopencm3/stm32/dma.h"
#include "libopencm3/cm3/nvic.h"

#include "bluetooch.h"
#include "motor.h"

void bt_setup(void)
{
        rcc_periph_clock_enable(RCC_GPIOA);

        rcc_periph_clock_enable(RCC_AFIO);
        rcc_periph_clock_enable(RCC_USART2);
        rcc_periph_clock_enable(RCC_DMA1);

        /* Set up USART2 */
        gpio_set_mode(GPIO_BANK_USART2_TX, GPIO_MODE_OUTPUT_50_MHZ,
                        GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);
        gpio_set_mode(GPIO_BANK_USART2_RX, GPIO_MODE_INPUT,
                        GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);

        /* Configure USART2 TX. */
        usart_set_baudrate(USART2, 9600);
        usart_set_databits(USART2, 8);
        usart_set_stopbits(USART2, USART_STOPBITS_1);
        usart_set_mode(USART2, USART_MODE_TX_RX);
        usart_set_parity(USART2, USART_PARITY_NONE);
        usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

        /* Finally enable USART2. */
        usart_enable(USART2);

        nvic_set_priority(NVIC_DMA1_CHANNEL7_IRQ, 0);
	nvic_enable_irq(NVIC_DMA1_CHANNEL7_IRQ);

        nvic_set_priority(NVIC_DMA1_CHANNEL6_IRQ, 0);
	nvic_enable_irq(NVIC_DMA1_CHANNEL6_IRQ);
}

void bt_dma_write(char *data, int size)
{
	/*
	 * Using channel 7 for USART2_TX
	 */

	/* Reset DMA channel*/
	dma_channel_reset(DMA1, DMA_CHANNEL7);

	dma_set_peripheral_address(DMA1, DMA_CHANNEL7, (uint32_t)&USART2_DR);
	dma_set_memory_address(DMA1, DMA_CHANNEL7, (uint32_t)data);
	dma_set_number_of_data(DMA1, DMA_CHANNEL7, size);
	dma_set_read_from_memory(DMA1, DMA_CHANNEL7);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL7);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL7, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, DMA_CHANNEL7, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, DMA_CHANNEL7, DMA_CCR_PL_VERY_HIGH);

	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL7);

	dma_enable_channel(DMA1, DMA_CHANNEL7);

        usart_enable_tx_dma(USART2);
}

volatile int bt_transfered = 0;

void dma1_channel7_isr(void)
{
	if ((DMA1_ISR &DMA_ISR_TCIF7) != 0) {
		DMA1_IFCR |= DMA_IFCR_CTCIF7;

		bt_transfered = 1;
	}

	dma_disable_transfer_complete_interrupt(DMA1, DMA_CHANNEL7);

	usart_disable_tx_dma(USART2);

	dma_disable_channel(DMA1, DMA_CHANNEL7);
}

volatile int bt_received = 0;

void bt_dma_read(char *data, int size)
{
	/*
	 * Using channel 6 for USART2_RX
	 */

	/* Reset DMA channel*/
	dma_channel_reset(DMA1, DMA_CHANNEL6);

	dma_set_peripheral_address(DMA1, DMA_CHANNEL6, (uint32_t)&USART2_DR);
	dma_set_memory_address(DMA1, DMA_CHANNEL6, (uint32_t)data);
	dma_set_number_of_data(DMA1, DMA_CHANNEL6, size);
	dma_set_read_from_peripheral(DMA1, DMA_CHANNEL6);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL6);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL6, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, DMA_CHANNEL6, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, DMA_CHANNEL6, DMA_CCR_PL_HIGH);

	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL6);

	dma_enable_channel(DMA1, DMA_CHANNEL6);

        usart_enable_rx_dma(USART2);
}

void dma1_channel6_isr(void)
{
	if ((DMA1_ISR &DMA_ISR_TCIF6) != 0) {
		DMA1_IFCR |= DMA_IFCR_CTCIF6;

		bt_received = 1;
	}

	dma_disable_transfer_complete_interrupt(DMA1, DMA_CHANNEL6);

	usart_disable_rx_dma(USART2);

	dma_disable_channel(DMA1, DMA_CHANNEL6);
}
