#include "Device/Include/stm32f10x.h"   // Device header
#include "SysTick.h"
#include "LCD.h"
#include "Composer.h"
#include "Delay.h"
#include "KeyScan.h"
#include "KeyDisp.h"
#include "Auto.h"
#include "SpecialKeyScan.h"
#include "VolDisp.h"
#include "WaveSel.h"

int main()
{
	SystemInit();
	SysTickInit();
	LCDInit();
	LCDClear(BLACK);
	LCDBGEnable(1);
	LCDDrawText(0,0,WHITE,BLACK,"Initializing Composer");
	InitComposer();
	LCDDrawText(0,16,WHITE,BLACK,"Initializing KeyScan");
	KeyScanInit();
	SpecialKeyScanInit();
	SetVol(255);
	LCDDrawText(0,32,WHITE,BLACK,"Initialize Complete");
	
	Delay(300);
	SetAllKeyDispDisable();
	RefreshKeyDisp();
	VolDisp(255);
	LCDDrawText(10,10,GREEN,GRAY2,"Left=AutoPlay");
	LCDDrawText(10,30,GREEN,GRAY2,"Right=Change wave type");
	
	while(1)
	{
		u8* KeyVals=KeyScanGetVals();
		u8 i;
		u8 Ctl=SpecialKeyGetCtrl();
		u8 Ctrl=SpecialKeyGetCtrl();
		signed short Enc=SpecialKeyGetEnc();
		if(Ctrl & 1)
		{
			SetAllTuneDisable();
			SetAllKeyDispDisable();
			DispSongList();
			RefreshKeyDisp();
			VolDisp(GetVol());
			LCDDrawText(10,10,GREEN,GRAY2,"Left=AutoPlay");
			LCDDrawText(10,30,GREEN,GRAY2,"Right=Change wave type");
		}
		if(Ctrl & 2)
		{
			SetAllTuneDisable();
			SetAllKeyDispDisable();
			WaveSel();
			RefreshKeyDisp();
			VolDisp(GetVol());
			LCDDrawText(10,10,GREEN,GRAY2,"Left=AutoPlay");
			LCDDrawText(10,30,GREEN,GRAY2,"Right=Change wave type");
		}
		if(Enc)
		{
			signed int Vol=GetVol()+Enc*5;
			if(Vol>255)
			{
				Vol=255;
			}
			if(Vol<0)
			{
				Vol=0;
			}
			SetVol(Vol);
			VolDisp(Vol);
		}
		for(i=0;i<36;i++)
		{
			if(SetTuneEnable(i,KeyVals[i]))
			{
				SetKeyDisp(i,KeyVals[i]);
			}
		}
	}
}
