#ifndef __BOARD_H
#define __BOARD_H


#include "ti_msp_dl_config.h"

void board_init(void);

void delay_1us(unsigned long __us);
void delay_1ms(unsigned long ms);
void delay_us(unsigned long __us);
void delay_ms(unsigned long ms);

extern volatile uint8_t  recv0_buff[128];
extern volatile uint16_t recv0_length;
extern volatile uint8_t  recv0_flag;


#endif