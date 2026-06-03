/* Lion Dance Robot - Integrated Control */
#include "stdio.h"
#include "board.h"
#include "voice.h"
#include "led.h"
#include "motor.h"
#include "BSP/LCD/lcd.h"
#include "BSP/LCD/lcd_init.h"

/* ================================================================
 *  Action state machine
 *  Continuous actions (Forward, Dance) need per-loop stepping;
 *  One-shot actions (Sit, Stand, Stop) complete immediately.
 * ================================================================ */
typedef enum {
    ACTION_NONE = 0,        /* Idle, waiting for command */
    ACTION_FORWARD,         /* Continuous walking gait */
    ACTION_DANCE            /* Continuous dance gait */
} ActionState_t;

static ActionState_t current_action = ACTION_NONE;

/* ================================================================
 *  UART character → voice command mapping
 * ================================================================ */
static uint8_t UartCharToCmd(char ch)
{
    switch (ch) {
    case 'F': case 'f': return VOICE_CMD_FORWARD;
    case 'S': case 's': return VOICE_CMD_STAND;
    case 'A': case 'a': return VOICE_CMD_SIT;
    case 'D': case 'd': return VOICE_CMD_DANCE;
    case 'X': case 'x': return VOICE_CMD_STOP;
    case 'H': case 'h': return VOICE_CMD_WAKEUP;
    default:            return VOICE_CMD_NONE;
    }
}

/* ================================================================
 *  Execute voice command
 *  One-shot commands: complete immediately
 *  Continuous commands: set action state for main loop stepping
 * ================================================================ */
void ExecuteVoiceCommand(uint8_t cmd)
{
    switch (cmd) {
    case VOICE_CMD_FORWARD:
        printf("[Voice] CMD: FORWARD\r\n");
        LED_On();
        reset_gait();
        current_action = ACTION_FORWARD;
        break;

    case VOICE_CMD_SIT:
        printf("[Voice] CMD: SIT\r\n");
        LED_On();
        current_action = ACTION_NONE;
        sit();
        LED_Off();
        break;

    case VOICE_CMD_STAND:
        printf("[Voice] CMD: STAND\r\n");
        LED_On();
        current_action = ACTION_NONE;
        stand();
        LED_Off();
        break;

    case VOICE_CMD_DANCE:
        printf("[Voice] CMD: DANCE\r\n");
        LED_On();
        reset_gait();
        current_action = ACTION_DANCE;
        break;

    case VOICE_CMD_STOP:
        printf("[Voice] CMD: STOP\r\n");
        LED_Off();
        current_action = ACTION_NONE;
        servo_zero();   /* all servos to mid position */
        break;

    case VOICE_CMD_WAKEUP:
        printf("[Voice] CMD: WAKEUP\r\n");
        LED_Flash();
        current_action = ACTION_NONE;
        servo_zero();   /* all servos to mid position */
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

    /* LCD init & welcome screen */
    LCD_Init();
    LCD_DrawWelcome();

    Voice_Init();
    printf("[Voice] CI1302 voice module initialized\r\n");

    LED_Init();
    LED_Flash();
    printf("[LED] LED initialized on PA12\r\n");

    /* Initialize motor: all servos to 90° mid position */
    servo_zero();
    printf("[Servo] 4-channel servo PWM ready\r\n");
    printf("==========================================\r\n");
    printf("  Lion Dance Robot Ready!\r\n");
    printf("  Command: Forward/Sit/Stand/Dance/Stop\r\n");
    printf("==========================================\r\n\r\n");

    while (1) {
        /* ---- Voice command polling ---- */
        voice_cmd = Voice_ReadCommand();
        if (voice_cmd != VOICE_CMD_NONE) {
            ExecuteVoiceCommand(voice_cmd);
        }

        /* ---- UART serial command processing ---- */
        if (recv0_flag) {
            uint8_t uart_cmd;
            for (uint16_t i = 0; i < recv0_length; i++) {
                uart_cmd = UartCharToCmd((char)recv0_buff[i]);
                if (uart_cmd != VOICE_CMD_NONE) {
                    ExecuteVoiceCommand(uart_cmd);
                }
            }
            recv0_flag = 0;
            recv0_length = 0;
        }

        /* ---- Continuous action stepping ---- */
        switch (current_action) {
        case ACTION_FORWARD:
            robot_forward();    /* single gait step, 15ms */
            break;
        case ACTION_DANCE:
            dance();            /* single dance step, 15ms */
            break;
        case ACTION_NONE:
        default:
            {
                /* Clear screen and draw emoji once when entering idle */
                static uint8_t emoji_drawn = 0;
                if (!emoji_drawn) {
                    LCD_Fill(0, 0, 240, 240, BLACK);
                    LCD_DrawEmoji(120, 120, 3);   /* center, size=3 */
                    emoji_drawn = 1;
                }
            }
            /* Idle: 500ms delay to match original timing */
            delay_ms(500);
            break;
        }
    }
}