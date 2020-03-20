#ifndef __DELAY_H
#define __DELAY_H
 
#ifdef __cplusplus
extern "C" {
#endif
 
#include "stm32f4xx.h"
 
void DelayInit(void);
void DelayUs(uint32_t us);
void DelayMs(uint32_t ms);
	
void Delay_MS(__IO uint32_t nTime);   
 
#ifdef __cplusplus
}
#endif
 
#endif