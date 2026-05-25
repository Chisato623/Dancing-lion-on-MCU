#include "voice.h"

/* CI1302 I2C address */
#define CI1302_ADDR        0x64

/* CI1302 registers */
#define CI1302_REG_CMD     0x01
#define CI1302_REG_STATUS  0x02

static volatile uint8_t g_voice_cmd = VOICE_CMD_NONE;

/* Write register via I2C */
static bool CI1302_WriteReg(uint8_t reg, uint8_t val)
{
    uint8_t txBuf[2] = { reg, val };
    DL_I2C_startControllerTransfer(I2C_0_INST, CI1302_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_TX, 2);
    DL_I2C_fillControllerTXFIFO(I2C_0_INST, txBuf, 2);

    while (DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_BUSY);
    if (DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_ERROR)
    {
        return false;
    }
    return true;
}

/* Read register via I2C */
static bool CI1302_ReadReg(uint8_t reg, uint8_t *val)
{
    DL_I2C_startControllerTransfer(I2C_0_INST, CI1302_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_TX, 1);
    DL_I2C_fillControllerTXFIFO(I2C_0_INST, &reg, 1);
    while (DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_BUSY);

    DL_I2C_startControllerTransfer(I2C_0_INST, CI1302_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_RX, 1);
    while (DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_BUSY);

    *val = DL_I2C_receiveControllerData(I2C_0_INST);

    if (DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_ERROR)
    {
        return false;
    }
    return true;
}

/* Init voice module */
void Voice_Init(void)
{
    CI1302_WriteReg(CI1302_REG_CMD, 0x00);
}

/* Read current voice command from CI1302 */
uint8_t Voice_ReadCommand(void)
{
    uint8_t cmd = VOICE_CMD_NONE;
    if (CI1302_ReadReg(CI1302_REG_STATUS, &cmd))
    {
        g_voice_cmd = cmd;
    }
    return g_voice_cmd;
}