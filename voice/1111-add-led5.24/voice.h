#ifndef __VOICE_H
#define __VOICE_H

#include "ti_msp_dl_config.h"

#define VOICE_CMD_NONE       0x00

#define VOICE_CMD_WAKEUP     0x03

#define VOICE_CMD_FORWARD    0x01

#define VOICE_CMD_SIT   		 0x0C

#define VOICE_CMD_STAND      0x0A

#define VOICE_CMD_DANCE      0x6C

#define VOICE_CMD_STOP       0x09

void Voice_Init(void);

uint8_t Voice_ReadCommand(void);

#endif