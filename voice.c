#include "voice.h"


/*
 * CI1302 I2C地址
 */
#define ASR_ADDR          0x34

/*
 * 识别结果寄存器
 */
#define ASR_RESULT_ADDR   0x64


/*
 * UART发送单字符
 */
static void UART_SendChar(char ch)
{
    DL_UART_Main_transmitData(UART_0_INST, ch);

    while(DL_UART_isBusy(UART_0_INST));
}


/*
 * UART发送字符串
 */
static void UART_SendString(char *str)
{
    while(*str)
    {
        UART_SendChar(*str++);
    }
}


/*
 * UART发送16进制
 */
static void UART_SendHex(uint8_t data)
{
    char buf[3];

    const char table[] = "0123456789ABCDEF";

    buf[0] = table[(data >> 4) & 0x0F];

    buf[1] = table[data & 0x0F];

    buf[2] = '\0';

    UART_SendString(buf);
}


/*
 * 语音模块初始化
 */
void Voice_Init(void)
{
    UART_SendString("VOICE INIT\r\n");
}


/*
 * 读取语音识别结果
 */
uint8_t Voice_ReadCommand(void)
{
    uint8_t reg = ASR_RESULT_ADDR;

    uint8_t result = 0;

    uint32_t timeout;


    /*
     * Step1:
     * 发送寄存器地址
     */

    DL_I2C_fillControllerTXFIFO(
            I2C_0_INST,
            &reg,
            1);

    DL_I2C_startControllerTransfer(
            I2C_0_INST,
            ASR_ADDR,
            DL_I2C_CONTROLLER_DIRECTION_TX,
            1);

    timeout = 100000;

    while(DL_I2C_getControllerStatus(I2C_0_INST)
          & DL_I2C_CONTROLLER_STATUS_BUSY_BUS)
    {
        timeout--;

        if(timeout == 0)
        {
            UART_SendString("TX TIMEOUT\r\n");

            return VOICE_CMD_NONE;
        }
    }


    /*
     * 检查ACK
     */

    if(DL_I2C_getControllerStatus(I2C_0_INST)
       & DL_I2C_CONTROLLER_STATUS_ERROR)
    {
        UART_SendString("TX ERROR\r\n");

        return VOICE_CMD_NONE;
    }


    /*
     * 给CI1302一点时间
     */

    delay_cycles(10000);


    /*
     * Step2:
     * 读取1字节
     */

    DL_I2C_startControllerTransfer(
            I2C_0_INST,
            ASR_ADDR,
            DL_I2C_CONTROLLER_DIRECTION_RX,
            1);

    /*
     * Wait for transfer to complete before checking RXFIFO
     */
    timeout = 100000;

    while(DL_I2C_getControllerStatus(I2C_0_INST)
          & DL_I2C_CONTROLLER_STATUS_BUSY_BUS)
    {
        timeout--;

        if(timeout == 0)
        {
            UART_SendString("RX BUSY TIMEOUT\r\n");

            return VOICE_CMD_NONE;
        }
    }

    /*
     * Check for I2C error (NACK from CI1302)
     */
    if(DL_I2C_getControllerStatus(I2C_0_INST)
       & DL_I2C_CONTROLLER_STATUS_ERROR)
    {
        UART_SendString("RX ERROR\r\n");

        return VOICE_CMD_NONE;
    }

    timeout = 1000000;

    while(DL_I2C_isControllerRXFIFOEmpty(I2C_0_INST))
    {
        timeout--;

        if(timeout == 0)
        {
            UART_SendString("RX TIMEOUT\r\n");

            return VOICE_CMD_NONE;
        }
    }


    /*
     * 获取识别结果
     */

    result = DL_I2C_receiveControllerData(I2C_0_INST);


    /*
     * 调试打印 —— 仅在识别到有效指令时打印
     */
    if (result != VOICE_CMD_NONE) {
        UART_SendString("[Voice] RESULT = 0x");
        UART_SendHex(result);
        UART_SendString("\r\n");
    }

    return result;
}