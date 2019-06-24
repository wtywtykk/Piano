#include "Device/Include/stm32f10x.h"   // Device header
#include "LCD.h"
#include "Menu.h"
#include "SpecialKeyScan.h"

#define MaxScrPos (Y_MAX_PIXEL/18)
#define MenuTextColor BLACK
#define MenuBackgroundColor WHITE
#define MenuTextColorSelected BLACK
#define MenuBackgroundColorSelected BLUE

void EnterMenu(const struct MenuStruct* Struct)
{
	const struct MenuStruct* StartMenu=Struct;
	u8 ScrPos=0;
	u8 i;
	u8 Refresh=1;
	while(SpecialKeyGetCtrl());
	while(1)
	{
		u8 Ctrl=SpecialKeyGetCtrl();
		signed short Enc=SpecialKeyGetEnc();
		if(Ctrl & 1)
		{
			while(SpecialKeyGetCtrl());
			return;
		}
		if(Ctrl & 2)
		{
			while(SpecialKeyGetCtrl());
			(StartMenu+ScrPos)->Callback((StartMenu+ScrPos)->Tag);
			while(SpecialKeyGetCtrl());
			Refresh=1;
		}
		if(Enc)
		{
			Refresh=1;
			if(Enc>0)
			{
				for(;Enc!=0;Enc--)
				{
					ScrPos++;
					if(!(StartMenu+ScrPos)->Callback)
					{
						ScrPos--;
						break;
					}
					if(ScrPos==MaxScrPos)
					{
						ScrPos=MaxScrPos-1;
						if(!(StartMenu+MaxScrPos)->Callback)
						{
							break;
						}
						StartMenu++;
					}
				}
			}
			else
			{
				for(;Enc!=0;Enc++)
				{
					ScrPos--;
					if(ScrPos==(u8)-1)
					{
						ScrPos=0;
						if(StartMenu==Struct)
						{
							break;
						}
						StartMenu--;
					}
				}
			}
		}
		if(Refresh)
		{
			u8 NoMoreItems=0;
			for(i=0;i<MaxScrPos;i++)
			{
				u16 Text=i==ScrPos?MenuTextColorSelected:MenuTextColor;
				u16 Background=i==ScrPos?MenuBackgroundColorSelected:MenuBackgroundColor;
				LCDFill(0,18*i,X_MAX_PIXEL,18*i+18,Background);
				if(!NoMoreItems)
				{
					if((StartMenu+i)->Callback)
					{
						LCDDrawText(10,18*i+1,Text,Background,(StartMenu+i)->Name);
					}
					else
					{
						NoMoreItems=1;
					}
				}
			}
			Refresh=0;
		}
	}
}
