#include "ti_msp_dl_config.h"

#include "voice.h"

#include "led.h"


int main(void)
{
    uint8_t cmd;

    SYSCFG_DL_init();

    Voice_Init();

    LED_Init();

    while(1)
    {
        /*
         * 读取语音命令
         */

        cmd = Voice_ReadCommand();


        /*
         * 如果识别到有效命令
         */

        if(cmd != VOICE_CMD_NONE)
        {
            /*
             * LED闪一下
             */

            LED_Flash();
        }


        /*
         * 根据命令执行动作
         */

        switch(cmd)
        {
            case VOICE_CMD_WAKEUP:

                break;


            case VOICE_CMD_FORWARD:

                break;


            case VOICE_CMD_SIT:

                break;


            case VOICE_CMD_STAND:

                break;


            case VOICE_CMD_DANCE:

                break;


            case VOICE_CMD_STOP:

                break;


            default:

                break;
        }


        delay_cycles(1600000);
    }
}