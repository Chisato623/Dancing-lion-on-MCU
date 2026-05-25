#include "board.h"
#include "stdio.h"

#define RE_0_BUFF_LEN_MAX	128

volatile uint8_t  recv0_buff[RE_0_BUFF_LEN_MAX] = {0};
volatile uint16_t recv0_length = 0;
volatile uint8_t  recv0_flag = 0;

void board_init(void)
{
	SYSCFG_DL_init();
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
	
	printf("Board Init [[ ** Lion Dance Robot ** ]]\r\n");
}

/* microsecond delay using SysTick */
void delay_us(unsigned long __us) 
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 38;

    ticks = __us * (32000000 / 1000000);
    told = SysTick->VAL;

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += SysTick->LOAD - tnow + told;

            told = tnow;
            if (tcnt >= ticks)
                break;
        }
    }
}

/* millisecond delay */
void delay_ms(unsigned long ms) 
{
	delay_us( ms * 1000 );
}

void delay_1us(unsigned long __us){ delay_us(__us); }
void delay_1ms(unsigned long ms){ delay_ms(ms); }

/* UART send char */
void uart0_send_char(char ch)
{
	while( DL_UART_isBusy(UART_0_INST) == true );
	DL_UART_Main_transmitData(UART_0_INST, ch);
}

/* UART send string */
void uart0_send_string(char* str)
{
	while(*str!=0&&str!=0)
	{
		uart0_send_char(*str++);
	}
}

#if !defined(__MICROLIB)
#if (__ARMCLIB_VERSION <= 6000000)
struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

/* avoid semi-hosting */
void _sys_exit(int x)
{
	x = x;
}
#endif

/* fputc for printf */
int fputc(int ch, FILE *stream)
{
	while( DL_UART_isBusy(UART_0_INST) == true );
	DL_UART_Main_transmitData(UART_0_INST, ch);
	return ch;
}

/* UART0 interrupt handler */
void UART_0_INST_IRQHandler(void)
{
	uint8_t receivedData = 0;
	
	switch( DL_UART_getPendingInterrupt(UART_0_INST) )
	{
		case DL_UART_IIDX_RX:
			receivedData = DL_UART_Main_receiveData(UART_0_INST);

			if (recv0_length < RE_0_BUFF_LEN_MAX - 1)
			{
				recv0_buff[recv0_length++] = receivedData;
				uart0_send_char(receivedData);
			}
			else
			{
				recv0_length = 0;
			}

			recv0_flag = 1;
			break;
		
		default:
			break;
	}
}