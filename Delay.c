#include "Device/Include/stm32f10x.h"   // Device header
#include "SysTick.h"

void Delay(u32 nTime)
{
	u32 EndTicks=GetSysTick()+nTime*10;
	while(GetSysTick()<EndTicks);
}

void PreciseDelay(u32 nTime)
{
	static u32 EndTicks=0;
	while(GetSysTick()<EndTicks);
	EndTicks=GetSysTick()+nTime*10;
}

void Delay100us(u32 nTime)
{
	u32 EndTicks=GetSysTick()+nTime;
	while(GetSysTick()<EndTicks);
}
