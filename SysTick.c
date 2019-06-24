#include "Device/Include/stm32f10x.h"	 // Device header
#include "RTE_Device.h"								 // Keil::Device:Startup

volatile u32 Ticks;

void SysTickInit(void)
{
	NVIC_SetPriority(SysTick_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,0));
	Ticks=0;
	SysTick_Config(RTE_SYSCLK / 10000);
}

void SysTickIncreasement(void)
{
	Ticks++;
}

u32 GetSysTick(void)
{
	return Ticks;
}
