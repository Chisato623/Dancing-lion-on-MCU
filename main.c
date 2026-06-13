/* Lion Dance Robot - Integrated Control */
#include "stdio.h"
#include "board.h"
#include "voice.h"
#include "led.h"
#include "motor.h"
#include "BSP/LCD/lcd.h"
#include "BSP/LCD/lcd_init.h"

typedef enum {
    ACTION_NONE = 0,
    ACTION_FORWARD,
    ACTION_DANCE
} ActionState_t;

static ActionState_t current_action = ACTION_NONE;
static uint8_t current_display_cmd = VOICE_CMD_NONE;

static void MP3_SetDanceOutput(uint8_t enabled)
{
    if (enabled) {
        DL_GPIO_setPins(MP3_PORT, MP3_PIN_0_PIN);
    } else {
        DL_GPIO_clearPins(MP3_PORT, MP3_PIN_0_PIN);
    }
}

static void LCD_DrawCommandEmoji(uint8_t cmd)
{
    if (cmd == VOICE_CMD_NONE) {
        return;
    }

    LCD_Fill(0, 0, 240, 240, BLACK);

    if (cmd == VOICE_CMD_FORWARD) {
        LCD_DrawEmoji_O_O(120, 120, 3);
    } else if (cmd == VOICE_CMD_DANCE) {
        LCD_DrawEmoji_gtwlt(120, 120, 3);
    } else if (cmd == VOICE_CMD_WAKEUP) {
        LCD_DrawEmoji(120, 120, 3);
    } else if (cmd == VOICE_CMD_STAND) {
        LCD_DrawEmoji_O_O(120, 120, 3);
    } else if (cmd == VOICE_CMD_SIT) {
        LCD_DrawEmoji_dashwdash(120, 120, 3);
    } else if (cmd == VOICE_CMD_STOP) {
        LCD_DrawEmoji_dashwdash(120, 120, 3);
    } else {
        LCD_DrawEmoji_dashwdash(120, 120, 3);
    }
}

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

void ExecuteVoiceCommand(uint8_t cmd)
{
    switch (cmd) {
    case VOICE_CMD_FORWARD:
        printf("[Voice] CMD: FORWARD\r\n");
        LED_On();
        MP3_SetDanceOutput(0);
        current_display_cmd = cmd;
        reset_gait();
        current_action = ACTION_FORWARD;
        break;

    case VOICE_CMD_SIT:
        printf("[Voice] CMD: SIT\r\n");
        LED_On();
        MP3_SetDanceOutput(0);
        current_display_cmd = cmd;
        current_action = ACTION_NONE;
        sit();
        LED_Off();
        break;

    case VOICE_CMD_STAND:
        printf("[Voice] CMD: STAND\r\n");
        LED_On();
        MP3_SetDanceOutput(0);
        current_display_cmd = cmd;
        current_action = ACTION_NONE;
        stand();
        LED_Off();
        break;

    case VOICE_CMD_DANCE:
        printf("[Voice] CMD: DANCE\r\n");
        LED_On();
        MP3_SetDanceOutput(1);
        current_display_cmd = cmd;
        reset_gait();
        current_action = ACTION_DANCE;
        break;

    case VOICE_CMD_STOP:
        printf("[Voice] CMD: STOP\r\n");
        LED_Off();
        MP3_SetDanceOutput(0);
        current_display_cmd = cmd;
        current_action = ACTION_NONE;
        servo_zero();
        break;

    case VOICE_CMD_WAKEUP:
        printf("[Voice] CMD: WAKEUP\r\n");
        LED_Flash();
        MP3_SetDanceOutput(0);
        current_display_cmd = cmd;
        current_action = ACTION_NONE;
        servo_zero();
        break;

    case VOICE_CMD_NONE:
    default:
        break;
    }
}

int main(void)
{
    uint8_t voice_cmd;

    board_init();

    LCD_Init();
    LCD_DrawWelcome();

    Voice_Init();
    printf("[Voice] CI1302 voice module initialized\r\n");

    MP3_SetDanceOutput(0);

    LED_Init();
    LED_Flash();
    printf("[LED] LED initialized on PA12\r\n");

    servo_zero();
    reset_gait();
    printf("[Servo] 4-channel servo PWM ready\r\n");
    printf("==========================================\r\n");
    printf("  Lion Dance Robot Ready!\r\n");
    printf("  Command: Forward/Sit/Stand/Dance/Stop\r\n");
    printf("==========================================\r\n\r\n");

    while (1) {
        voice_cmd = Voice_ReadCommand();
        if (voice_cmd != VOICE_CMD_NONE) {
            ExecuteVoiceCommand(voice_cmd);
        }

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

        {
            static uint8_t last_display_cmd = 0xFF;
            static uint8_t dance_frame = 0;
            static uint16_t dance_counter = 0;
            #define DANCE_FRAME_LOOPS 22

            if (current_display_cmd != last_display_cmd) {
                last_display_cmd = current_display_cmd;
                dance_frame = 0;
                dance_counter = 0;

                LCD_DrawCommandEmoji(current_display_cmd);
            } else if (current_display_cmd == VOICE_CMD_DANCE) {
                dance_counter++;
                if (dance_counter >= DANCE_FRAME_LOOPS) {
                    dance_counter = 0;
                    dance_frame++;
                    if (dance_frame >= 3) {
                        dance_frame = 0;
                    }

                    LCD_Fill(0, 0, 240, 240, BLACK);
                    switch (dance_frame) {
                    case 0: LCD_DrawEmoji_gtwlt(120, 120, 3); break;
                    case 1: LCD_DrawEmoji_Owlt(120, 120, 3); break;
                    case 2: LCD_DrawEmoji_gtwO(120, 120, 3); break;
                    default: break;
                    }
                }
            }
        }

        switch (current_action) {
        case ACTION_FORWARD:
            robot_forward();
            break;
        case ACTION_DANCE:
            dance();
            break;
        case ACTION_NONE:
        default:
            delay_ms(30);
            break;
        }
    }
}
