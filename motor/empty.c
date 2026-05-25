#include "ti_msp_dl_config.h"

/* ?????? (32MHz) */
void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 3200; i++);
}

/* ?? ? ??? (0.5MHz ??, ?? 10000) */
uint16_t ang(uint8_t a) {
    if (a > 180) a = 180;
    return 9000 + ((uint32_t)a * 1000) / 180;
}

/* ?????????? (??, ??, ??, ??) */
void servo_set(uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3) {
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR2_INST, ang(a0), GPIO_PWM_MOTOR2_C0_IDX);//front_left
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR2_INST, ang(a1), GPIO_PWM_MOTOR2_C1_IDX);//front_right
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR2_INST, ang(a2), GPIO_PWM_MOTOR2_C2_IDX);//behind_left
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR2_INST, ang(a3), GPIO_PWM_MOTOR2_C3_IDX);//behind_right
}
void servo_zero(void)
{
    // ??:servo_set(??1, ??1, ??2, ??2);
    servo_set(90, 90, 90, 90);
    delay_ms(500); // ??????
}

void sit(void)
{
	  servo_set(90, 90, 45, 45);    
	  delay_ms(500);    
}
void stand(void)
{
	  servo_set(90, 90, 90, 90);
	  delay_ms(500);
}
void robot_forward(void)
{
    #define CENTER      90
    #define AMPLITUDE   40
    #define STEP_DELAY  15
    #define PERIOD_MS   800

    // ????:???????? ? ??????
    static uint32_t t = 0;
    uint32_t half = PERIOD_MS / 2;
    int aA, aB;

    // ===== ?????????? =====
    float phaseA = (float)(t % PERIOD_MS) / PERIOD_MS;
    if (phaseA < 0.5f)
        phaseA = 2.0f * phaseA;
    else
        phaseA = 2.0f * (1.0f - phaseA);

    float phaseB = (float)((t + half) % PERIOD_MS) / PERIOD_MS;
    if (phaseB < 0.5f)
        phaseB = 2.0f * phaseB;
    else
        phaseB = 2.0f * (1.0f - phaseB);

    aA = CENTER + (int)((phaseA - 0.5f) * 2 * AMPLITUDE);
    aB = CENTER + (int)((phaseB - 0.5f) * 2 * AMPLITUDE);

    if (aA < 0) aA = 0;
    if (aA > 180) aA = 180;
    if (aB < 0) aB = 0;
    if (aB > 180) aB = 180;

    servo_set((uint8_t)aA, (uint8_t)aB, (uint8_t)aB, (uint8_t)aA);

    delay_ms(STEP_DELAY);
    t += STEP_DELAY;
}
void dance(void)
{
	  #define CENTER      90
    #define AMPLITUDE   40
    #define STEP_DELAY  15
    #define PERIOD_MS   800

    // ????:???????? ? ??????
    static uint32_t t = 0;
    uint32_t half = PERIOD_MS / 2;
    int aA, aB;

    // ===== ?????????? =====
    float phaseA = (float)(t % PERIOD_MS) / PERIOD_MS;
    if (phaseA < 0.5f)
        phaseA = 2.0f * phaseA;
    else
        phaseA = 2.0f * (1.0f - phaseA);

    float phaseB = (float)((t + half) % PERIOD_MS) / PERIOD_MS;
    if (phaseB < 0.5f)
        phaseB = 2.0f * phaseB;
    else
        phaseB = 2.0f * (1.0f - phaseB);

    aA = CENTER + (int)((phaseA - 0.5f) * 2 * AMPLITUDE);
    aB = CENTER + (int)((phaseB - 0.5f) * 2 * AMPLITUDE);

    if (aA < 0) aA = 0;
    if (aA > 180) aA = 180;
    if (aB < 0) aB = 0;
    if (aB > 180) aB = 180;

    servo_set((uint8_t)aA, (uint8_t)aB,50, 130);

    delay_ms(STEP_DELAY);
    t += STEP_DELAY;
}
int main(void) {
    
	      

    SYSCFG_DL_init();
	  servo_zero();
	  int ammm =1;

    while (1) {
        
	      switch (ammm)
				{
					case 1:robot_forward();
					  break;
					case 2:servo_zero();
					  break;
					case 3:sit();
					  break;
					case 4:stand();
					  break;
					case 5:dance();
					  break;
					default :
						break;
				}
				
    }
}
void SysTick_Handler(void)
{
    // ????,??????
}