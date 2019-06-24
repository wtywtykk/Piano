#include "Device/Include/stm32f10x.h"   // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_dac.h"              // Keil::Device:StdPeriph Drivers:DAC
#include "stm32f10x_dma.h"              // Keil::Device:StdPeriph Drivers:DMA
#include <math.h>
#include <string.h>

#define PI 3.14159265358979323846
#define DHR12R1_OFFSET ((uint32_t)0x00000008)
#define DACMax ((uint32_t)0x00000FFF)
#define Adjust (-18)

#define MaxActive 5
#define Scale 1000000
#define FrepToStep(Freq) (Scale*(Freq)/500)
#define Step 200
#define TuneCount 41
#define BufferSize 0x500

const u32 TuneStep[TuneCount]={
	FrepToStep(262),FrepToStep(277),FrepToStep(294),FrepToStep(311),FrepToStep(330),FrepToStep(349),FrepToStep(370),FrepToStep(392),FrepToStep(415),FrepToStep(440),FrepToStep(466),FrepToStep(494),
	FrepToStep(523),FrepToStep(554),FrepToStep(578),FrepToStep(622),FrepToStep(659),FrepToStep(698),FrepToStep(740),FrepToStep(784),FrepToStep(831),FrepToStep(880),FrepToStep(932),FrepToStep(988),
	FrepToStep(1047),FrepToStep(1109),FrepToStep(1175),FrepToStep(1245),FrepToStep(1319),FrepToStep(1397),FrepToStep(1480),FrepToStep(1568),FrepToStep(1661),FrepToStep(1760),FrepToStep(1865),FrepToStep(1976),
	FrepToStep(2049),
	//Only for test porpose
	FrepToStep(250),FrepToStep(500),FrepToStep(1000),FrepToStep(2000),
};

signed int WaveTable[Step];
u16 Buffer[BufferSize];
u8 Tunes[TuneCount];
u32 TuneCounter[TuneCount];
u8 ActiveCount;
u16 Volume;
u8 Vol8;
u8 WaveType;

void InitGPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}

void InitDAC(void)
{
	DAC_InitTypeDef DAC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
	DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude=0;
	DAC_InitStruct.DAC_OutputBuffer=DAC_OutputBuffer_Enable;
	DAC_InitStruct.DAC_Trigger=DAC_Trigger_T6_TRGO;
	DAC_InitStruct.DAC_WaveGeneration=DAC_WaveGeneration_None;
	
	DAC_Init(DAC_Channel_1,&DAC_InitStruct);
}

void InitNVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel=DMA2_Channel3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void InitDMA(void)
{
	DMA_InitTypeDef DMA_InitStruct;	
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

	DMA_InitStruct.DMA_BufferSize=BufferSize;
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralDST;
	DMA_InitStruct.DMA_M2M=DMA_M2M_Disable;
	DMA_InitStruct.DMA_MemoryBaseAddr=(uint32_t)Buffer;
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)DAC_BASE+DHR12R1_OFFSET+DAC_Align_12b_R;
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_Priority=DMA_Priority_VeryHigh;
	
	DMA_Init(DMA2_Channel3,&DMA_InitStruct);
	DMA_ITConfig(DMA2_Channel3,DMA_IT_TC | DMA_IT_HT,ENABLE);
}

void InitTimer(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=12000000/100000+Adjust;//100KHz
	TIM_TimeBaseInitStruct.TIM_Prescaler=6;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0x00;
	
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
	TIM_SelectOutputTrigger(TIM6,TIM_TRGOSource_Update);
}

void InitWaveTableSine(void)
{
	u32 i=0;
	for(;i<Step;i++)
	{
		WaveTable[i]=sin(2*PI*i/Step)*Volume;
	}
}

void InitWaveTableSquare(void)
{
	u32 i=0;
	for(;i<Step;i++)
	{
		WaveTable[i]=i<Step/2?Volume:-(signed int)Volume;
	}
}

void InitWaveTableTriangle(void)
{
	u32 i=0;
	for(;i<Step;i++)
	{
		if(i<Step/4)
		{
			WaveTable[i]=4*i*Volume/Step;
		}
		else if(i<3*Step/4)
		{
			WaveTable[i]=Volume-4*(i-Step/4)*Volume/Step;
		}
		else
		{
			WaveTable[i]=-(signed int)(4*(Step-i)*Volume/Step);
		}
	}
}

void InitWaveTableSawTooth(void)
{
	u32 i=0;
	for(;i<Step;i++)
	{
		WaveTable[i]=i*2*Volume/Step-Volume;
	}
}

void InitWaveTableDualSine(void)
{
	u32 i=0;
	for(;i<Step;i++)
	{
		WaveTable[i]=sin(2*PI*i/Step)*Volume*2/3+sin(4*PI*i/Step)*Volume/3;
	}
}

void RefreshWaveTable(void)
{
	switch(WaveType)
	{
		default:
		case 0:
			InitWaveTableSine();
			break;
		case 1:
			InitWaveTableSquare();
			break;
		case 2:
			InitWaveTableTriangle();
			break;
		case 3:
			InitWaveTableSawTooth();
			break;
		case 4:
			InitWaveTableDualSine();
			break;
	}
}

void SetWaveType(u8 Type)
{
	WaveType=Type;
	RefreshWaveTable();
}

void InitComposer(void)
{
	ActiveCount=0;
	memset(Buffer,0,sizeof(Buffer));
	memset(Tunes,0xFF,sizeof(Tunes));
	memset(TuneCounter,0,sizeof(TuneCounter));
	InitGPIO();
	InitDAC();
	InitNVIC();
	InitDMA();
	InitTimer();
	DMA_Cmd(DMA2_Channel3,ENABLE);
	DAC_Cmd(DAC_Channel_1,ENABLE);
	DAC_DMACmd(DAC_Channel_1,ENABLE);
	TIM_Cmd(TIM6,ENABLE);
	Volume=0;
	Vol8=0;
	WaveType=0;
	RefreshWaveTable();
}

void ConmosePart(u8 FirstPart)
{
	u16* Base=FirstPart?Buffer:Buffer+BufferSize/2;
	u16 i=0;
	for(;i<BufferSize/2;i++)
	{
		u8 j=0;
		signed int CurVal=0;
		for(;j<ActiveCount;j++)
		{
			u8 k=Tunes[j];
			TuneCounter[k]+=TuneStep[k];
			if(TuneCounter[k]>=Step*Scale)
			{
				TuneCounter[k]-=Step*Scale;
			}
			CurVal+=WaveTable[TuneCounter[k]/Scale];
		}
		CurVal/=ActiveCount;
		Base[i]=DACMax/2+CurVal;
	}
}

void Compose(void)
{
	ConmosePart(1);
	ConmosePart(0);
}

u8 SetTuneEnable(u8 Index,u8 Enable)
{
	u8 Success=0;
	if(Index<TuneCount)
	{
		u8 i=0;
		if(Enable)
		{
			if(ActiveCount>MaxActive)
			{
				return 0;
			}
			Success=1;
			for(;i<ActiveCount;i++)
			{
				if(Tunes[i]==Index)
				{
					Success=0;
					break;
				}
			}
			if(Success)
			{
				Tunes[i]=Index;
				ActiveCount++;
			}
		}
		else
		{
			for(;i<ActiveCount;i++)
			{
				if(Tunes[i]==Index)
				{
					Success=1;
					break;
				}
			}
			if(Success)
			{
				for(;i<ActiveCount-1;i++)
				{
					Tunes[i]=Tunes[i+1];
				}
				Tunes[ActiveCount-1]=0xFF;
				ActiveCount--;
			}
		}
	}
	return 1;
}

void SetAllTuneDisable(void)
{
	ActiveCount=0;
}

void SetVol(u8 Vol)
{
	if(Vol!=Vol8)
	{
		Vol8=Vol;
		Volume=DACMax*Vol/2/255-80;
		if(Volume>DACMax)
		{
			Volume=0;
		}
		RefreshWaveTable();
	}
}

u8 GetVol(void)
{
	return Vol8;
}
