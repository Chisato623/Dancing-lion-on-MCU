#include "led.h"
#include "board.h"

/* LED init */
void LED_Init(void)
{
    DL_GPIO_clearPins(GPIO_PORT, GPIO_LED_PIN);
}

/* LED on */
void LED_On(void)
{
    DL_GPIO_setPins(GPIO_PORT, GPIO_LED_PIN);
}

/* LED off */
void LED_Off(void)
{
    DL_GPIO_clearPins(GPIO_PORT, GPIO_LED_PIN);
}

/* LED toggle */
void LED_Toggle(void)
{
    DL_GPIO_togglePins(GPIO_PORT, GPIO_LED_PIN);
}

/* LED flash 3 times */
void LED_Flash(void)
{
    LED_Toggle();
    delay_ms(250);
    LED_Toggle();
    delay_ms(250);
    LED_Toggle();
    delay_ms(250);
    LED_Toggle();
}