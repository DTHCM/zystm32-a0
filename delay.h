#ifndef __DELAY_H
#define __DELAY_H

#include <stdint.h>

void delay_setup(void);
void delay_us(uint16_t us);
void delay_ms(int ms);

#endif
