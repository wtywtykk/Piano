#include "Device/Include/stm32f10x.h"   // Device header
#include "Composer.h"
#include "Menu.h"
#include "Delay.h"

void MenuCallbackWaveSel(u32 Tag);

const struct MenuStruct WaveList[]={
	"Sine",&MenuCallbackWaveSel,0,
	"Square",&MenuCallbackWaveSel,1,
	"Triangle",&MenuCallbackWaveSel,2,
	"SawTooth",&MenuCallbackWaveSel,3,
	"DualSine",&MenuCallbackWaveSel,4,
	0,0,0,
};

void MenuCallbackWaveSel(u32 Tag)
{
	SetWaveType(Tag);
	SetTuneEnable(0,1);
	Delay(100);
	SetTuneEnable(0,0);
}

void WaveSel(void)
{
	EnterMenu(WaveList);
}
