/* Lion Dance Robot - Integrated Control */
#include "stdio.h"
#include "board.h"
#include "voice.h"
#include "led.h"

/* Servo CCP mapping: 500us(min) ~ 2500us(max) -> 0deg ~ 180deg */
#define SERVO_FRONT_LEFT    DL_TIMERA_CAPTURE_COMPARE_0_INDEX  /* PA21 */
#define SERVO_FRONT_RIGHT   DL_TIMERA_CAPTURE_COMPARE_1_INDEX  /* PA22 */
#define SERVO_BEHIND_LEFT   DL_TIMERA_CAPTURE_COMPARE_2_INDEX  /* PA15 */
#define SERVO_BEHIND_RIGHT  DL_TIMERA_CAPTURE_COMPARE_3_INDEX  /* PA17 */

#define ANGLE_TO_CCP(angle_us)  ((uint32_t)((float)(angle_us) * 0.5f))

#define PWM_MOTOR2_PERIOD   10000U
#define PWM_SERVO_MIN       250U   /* 500us */
#define PWM_SERVO_MID       750U   /* 1500us */
#define PWM_SERVO_MAX       1250U  /* 2500us */

/* Set servo angle (value_us: 500~2500us) */
void Servo_SetAngle(uint32_t servo_idx, uint32_t value_us)
{
    uint32_t ccp_val;

    if (value_us < 500) value_us = 500;
    if (value_us > 2500) value_us = 2500;

    ccp_val = ANGLE_TO_CCP(value_us);
    if (ccp_val >= PWM_MOTOR2_PERIOD) ccp_val = PWM_MOTOR2_PERIOD - 1;

    DL_TimerA_setCaptureCompareValue(PWM_MOTOR2_INST, ccp_val, servo_idx);
}

/* Set all 4 servos to same angle */
void Servo_SetAll(uint32_t value_us)
{
    Servo_SetAngle(SERVO_FRONT_LEFT,  value_us);
    Servo_SetAngle(SERVO_FRONT_RIGHT, value_us);
    Servo_SetAngle(SERVO_BEHIND_LEFT,  value_us);
    Servo_SetAngle(SERVO_BEHIND_RIGHT, value_us);
}

/* All servos to mid position */
void Servo_GoHome(void)
{
    Servo_SetAll(1500);
    printf("[Servo] All to mid position (1500us)\r\n");
}

void Servo_ForwardPose(void)
{
    Servo_SetAngle(SERVO_FRONT_LEFT,  1800);
    Servo_SetAngle(SERVO_FRONT_RIGHT, 1200);
    Servo_SetAngle(SERVO_BEHIND_LEFT,  1200);
    Servo_SetAngle(SERVO_BEHIND_RIGHT, 1800);
}

void Servo_SitPose(void)
{
    Servo_SetAngle(SERVO_FRONT_LEFT,  1200);
    Servo_SetAngle(SERVO_FRONT_RIGHT, 1800);
    Servo_SetAngle(SERVO_BEHIND_LEFT,  1800);
    Servo_SetAngle(SERVO_BEHIND_RIGHT, 1200);
}

void Servo_StandPose(void)
{
    Servo_SetAngle(SERVO_FRONT_LEFT,  1500);
    Servo_SetAngle(SERVO_FRONT_RIGHT, 1500);
    Servo_SetAngle(SERVO_BEHIND_LEFT,  2500);
    Servo_SetAngle(SERVO_BEHIND_RIGHT, 500);
}

void Servo_Dance(void)
{
    uint32_t i;
    for (i = 0; i < 3; i++)
    {
        Servo_SetAngle(SERVO_FRONT_LEFT,  1800);
        Servo_SetAngle(SERVO_BEHIND_RIGHT, 1800);
        Servo_SetAngle(SERVO_FRONT_RIGHT, 1200);
        Servo_SetAngle(SERVO_BEHIND_LEFT,  1200);
        delay_ms(300);

        Servo_SetAngle(SERVO_FRONT_LEFT,  1200);
        Servo_SetAngle(SERVO_BEHIND_RIGHT, 1200);
        Servo_SetAngle(SERVO_FRONT_RIGHT, 1800);
        Servo_SetAngle(SERVO_BEHIND_LEFT,  1800);
        delay_ms(300);
    }
    Servo_GoHome();
}

/* Stop all servos (CCP > period = output low) */
void Servo_Stop(void)
{
    DL_TimerA_setCaptureCompareValue(PWM_MOTOR2_INST, PWM_MOTOR2_PERIOD, DL_TIMER_CC_0_INDEX);
    DL_TimerA_setCaptureCompareValue(PWM_MOTOR2_INST, PWM_MOTOR2_PERIOD, DL_TIMER_CC_1_INDEX);
    DL_TimerA_setCaptureCompareValue(PWM_MOTOR2_INST, PWM_MOTOR2_PERIOD, DL_TIMER_CC_2_INDEX);
    DL_TimerA_setCaptureCompareValue(PWM_MOTOR2_INST, PWM_MOTOR2_PERIOD, DL_TIMER_CC_3_INDEX);
}

/* Execute action by voice command */
void ExecuteVoiceCommand(uint8_t cmd)
{
    switch (cmd)
    {
    case VOICE_CMD_FORWARD:
        printf("[Voice] CMD: FORWARD\r\n");
        LED_On();
        Servo_ForwardPose();
        delay_ms(1000);
        Servo_GoHome();
        LED_Off();
        break;

    case VOICE_CMD_SIT:
        printf("[Voice] CMD: SIT\r\n");
        LED_On();
        Servo_SitPose();
        delay_ms(1000);
        Servo_GoHome();
        LED_Off();
        break;

    case VOICE_CMD_STAND:
        printf("[Voice] CMD: STAND\r\n");
        LED_On();
        Servo_StandPose();
        delay_ms(1000);
        Servo_GoHome();
        LED_Off();
        break;

    case VOICE_CMD_DANCE:
        printf("[Voice] CMD: DANCE\r\n");
        LED_On();
        Servo_Dance();
        LED_Off();
        break;

    case VOICE_CMD_STOP:
        printf("[Voice] CMD: STOP\r\n");
        LED_Off();
        Servo_Stop();
        break;

    case VOICE_CMD_WAKEUP:
        printf("[Voice] CMD: WAKEUP\r\n");
        LED_Flash();
        Servo_GoHome();
        break;

    case VOICE_CMD_NONE:
    default:
        break;
    }
}

/* ================================================================
 *  main
 * ================================================================ */
int main(void)
{
    uint8_t voice_cmd;

    board_init();

    Voice_Init();
    printf("[Voice] CI1302 voice module initialized\r\n");

    LED_Init();
    LED_Flash();
    printf("[LED] LED initialized on PA12\r\n");

    Servo_GoHome();
    printf("[Servo] 4-channel servo PWM ready\r\n");
    printf("==========================================\r\n");
    printf("  Lion Dance Robot Ready!\r\n");
    printf("  Command: Forward/Sit/Stand/Dance/Stop\r\n");
    printf("==========================================\r\n\r\n");

    while (1)
    {
        voice_cmd = Voice_ReadCommand();

        if (voice_cmd != VOICE_CMD_NONE)
        {
            ExecuteVoiceCommand(voice_cmd);
        }

        /* UART command processing */
        if (recv0_flag)
        {
            uint8_t uart_cmd;
            for (uint16_t i = 0; i < recv0_length; i++)
            {
                uart_cmd = recv0_buff[i];
                switch (uart_cmd)
                {
                case 'F':
                case 'f':
                    ExecuteVoiceCommand(VOICE_CMD_FORWARD);
                    break;
                case 'S':
                case 's':
                    ExecuteVoiceCommand(VOICE_CMD_STAND);
                    break;
                case 'A':
                case 'a':
                    ExecuteVoiceCommand(VOICE_CMD_SIT);
                    break;
                case 'D':
                case 'd':
                    ExecuteVoiceCommand(VOICE_CMD_DANCE);
                    break;
                case 'X':
                case 'x':
                    ExecuteVoiceCommand(VOICE_CMD_STOP);
                    break;
                case 'H':
                case 'h':
                    Servo_GoHome();
                    break;
                default:
                    break;
                }
            }
            recv0_flag = 0;
            recv0_length = 0;
        }

        delay_ms(50);
    }
}