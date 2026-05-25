#include "led.h"


/*
 * LED初始化
 */
void LED_Init(void)
{
    DL_GPIO_clearPins(
        LED_PORT,
        LED_PIN_12_PIN);
}


/*
 * LED亮
 */
void LED_On(void)
{
    DL_GPIO_setPins(
        LED_PORT,
        LED_PIN_12_PIN);
}


/*
 * LED灭
 */
void LED_Off(void)
{
    DL_GPIO_clearPins(
        LED_PORT,
        LED_PIN_12_PIN);
}


/*
 * LED翻转
 */
void LED_Toggle(void)
{
    DL_GPIO_togglePins(
        LED_PORT,
        LED_PIN_12_PIN);
}


/*
 * LED闪烁一次
 */
void LED_Flash(void)
{
    LED_On();

    delay_cycles(800000);

    LED_Off();
}