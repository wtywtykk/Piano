#include "Device/Include/stm32f10x.h"   // Device header
#include "LCD.h"
#include <string.h>

#define FullTuneTextColor BLACK
#define FullTuneColorNormal WHITE
#define FullTuneColorPressed RED
#define HalfTuneTextColor (BLUE | GREEN)
#define HalfTuneColorNormal BLACK
#define HalfTuneColorPressed 0x8000

#define TuneCount 36
#define FullTuneCount 21
#define HalfTuneCount 15

#define FullKeyHeight 100
#define HalfKeyHeight 70
const u8 StartY=Y_MAX_PIXEL-FullKeyHeight-3;
const u8 KeyWidth=X_MAX_PIXEL/FullTuneCount;
const u8 Marge=(X_MAX_PIXEL-(KeyWidth*FullTuneCount))/2;
const u8 KeyProp[TuneCount]={
	 1, 0, 2, 0, 3, 4, 0, 5, 0, 6, 0, 7,
	 8, 0, 9, 0,10,11, 0,12, 0,13, 0,14,
	15, 0,16, 0,17,18, 0,19, 0,20, 0,21,
};

const u32 Decoration[]={0x562B,0x73AE,0xDEA3,0x1028,0xF7FB,0x4108,0xDDAB,0x492E,0x7F6A,0x892A,0xFDEB,0x0480,0xFDFB,0x048A,0xDEEF,0x50A2};
u8 LastState[TuneCount];

void PrintNumberHint(u16 X,u16 EndY,u8 Index,u16 TextColor,u16 BGColor)
{
	char Num[2]={" \0"};
	u8 Tune=KeyProp[Index];
	if(Tune==0)
	{
		Index--;
		Tune=KeyProp[Index];
		LCDDrawText(X+4,EndY-48,TextColor,BGColor,"#");
	}
	if(Tune>14)
	{
		LCDDrawText(X+4,EndY-32,TextColor,BGColor,":");
		Tune-=14;
	}
	else if(Tune>7)
	{
		LCDDrawText(X+4,EndY-32,TextColor,BGColor,".");
		Tune-=7;
	}
	Num[0]=Tune+'0';
	LCDDrawText(X+4,EndY-16,TextColor,BGColor,Num);
}

void PrintKey(u8 Index)
{
	u16 X,Height;
	u16 Color;
	if(KeyProp[Index])
	{
		X=Marge+(KeyProp[Index]-1)*KeyWidth;
		Height=FullKeyHeight;
		Color=LastState[Index]?FullTuneColorPressed:FullTuneColorNormal;
	}
	else
	{
		X=Marge+(KeyProp[Index-1]-1)*KeyWidth+KeyWidth/2;
		Height=HalfKeyHeight;
		Color=LastState[Index]?HalfTuneColorPressed:HalfTuneColorNormal;
	}
	LCDFill(X+1,StartY+1,X+KeyWidth-1,StartY+Height-1,Color);
	if(LastState[Index])
	{
		PrintNumberHint(X,StartY+Height-2,Index,KeyProp[Index]?FullTuneTextColor:HalfTuneTextColor,Color);
	}
}

void SetKeyDisp(u8 Index,u8 Enable)
{
	if(Index>=TuneCount)
	{
		return;
	}
	if(LastState[Index]==Enable)
	{
		return;
	}
	else
	{
		LastState[Index]=Enable;
	}
	PrintKey(Index);
	if(Index>0)
	{
		if(KeyProp[Index-1]==0)
		{
			PrintKey(Index-1);
		}
	}
	if(Index<TuneCount-1)
	{
		if(KeyProp[Index+1]==0)
		{
			PrintKey(Index+1);
		}
	}
}

void DrawDecoration(void)
{
	u16 X,Y;
	for(Y=0;Y<StartY-5;Y++)
	{
		for(X=0;X<X_MAX_PIXEL;X++)
		{
			if(Decoration[Y/3%16] & 0x8000>>(X/3%16))
			{
				LCDDrawPoint(X,Y,BLUE | RED);
			}
		}
	}
}

void RefreshKeyDisp(void)
{
	u8 i;
	LCDClear(GRAY2);
	DrawDecoration();
	LCDFill(0,StartY-5,X_MAX_PIXEL,Y_MAX_PIXEL,GRAY1);
	for(i=0;i<TuneCount;i++)
	{
		if(KeyProp[i])
		{
			PrintKey(i);
		}
	}
	for(i=0;i<TuneCount;i++)
	{
		if(!KeyProp[i])
		{
			PrintKey(i);
		}
	}
}

void SetAllKeyDispDisable(void)
{
	memset(LastState,0,sizeof(LastState));
}
