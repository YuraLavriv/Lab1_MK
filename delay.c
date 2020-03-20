#include "delay.h"

 
// For store tick counts in us
static __IO uint32_t usTicks;

static __IO uint32_t uwTimingDelay;

void Delay_MS(__IO uint32_t nTime)
{ 
  uwTimingDelay = nTime;

  while(uwTimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0)
  { 
    uwTimingDelay--;
  }
}
 
// SysTick_Handler function will be called every 1 us
void SysTick_Handler()
{
	/*
    if (usTicks != 0)
    {
        usTicks--;
    }
	*/
	TimingDelay_Decrement();
}
 
void DelayInit()
{
    // Update SystemCoreClock value
    SystemCoreClockUpdate();
    // Configure the SysTick timer to overflow every 1 us
    SysTick_Config(SystemCoreClock / 1000000);
}
 
void DelayUs(uint32_t us)
{
    // Reload us value
    usTicks = us;
    // Wait until usTick reach zero
    while (usTicks);
}
 
void DelayMs(uint32_t ms)
{
	Delay_MS(ms);
	/*
    // Wait until ms reach zero
    while (ms--)
    {
        // Delay 1ms
        DelayUs(1000);
    }
	*/
}

