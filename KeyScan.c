#include "Device/Include/stm32f10x.h"   // Device header
#include <string.h>

u8 KeyVals[36];

void KeyScanInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_All);
	GPIO_SetBits(GPIOE, GPIO_Pin_All);
	
	memset(KeyVals,0,sizeof(KeyVals));
}

u8* KeyScanGetVals(void)
{
	u8 i;
	u32 Data=GPIO_ReadInputData(GPIOB) | (u32)GPIO_ReadInputData(GPIOE)<<16;
	for(i=0;i<24;i++)
	{
		KeyVals[i]=Data & 0x1?0:1;
		Data>>=1;
	}
	Data=GPIO_ReadInputData(GPIOA)>>8;
	for(;i<29;i++)
	{
		KeyVals[i]=Data & 0x1?0:1;
		Data>>=1;
	}
	Data>>=2;
	KeyVals[i]=Data & 0x1?0:1;
	i++;
	Data=GPIO_ReadInputData(GPIOD);
	for(;i<36;i++)
	{
		KeyVals[i]=Data & 0x1?0:1;
		Data>>=1;
	}
	return KeyVals;
}
