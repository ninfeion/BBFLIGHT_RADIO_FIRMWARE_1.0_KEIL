#ifndef _PCAPP_H_
#define _PCAPP_H_
#include "stm32f10x.h"


void applicationLoop(void);
void applicationLoopInit(void);
uint8_t pcDataReceive(void);
#endif

