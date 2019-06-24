#include "Device/Include/stm32f10x.h"   // Device header
#include "LCD.h"

#define TextColor GREEN
#define TextBackgroundColor GRAY2
#define FrameColor BLACK
#define BackgroundColor GRAY1
#define SlideColor BLUE

#define TextX 3
#define TextY 100
#define SlideX (TextX+35)
#define SlideY (TextY-2)
#define SlideWidth 20
#define SlideHeight 20
#define BorderWidth 2

void VolDisp(u8 Vol)
{
	LCDDrawText(TextX,TextY,TextColor,TextBackgroundColor,"VOL:");
	LCDFill(SlideX+BorderWidth,SlideY,SlideX+BorderWidth+255+SlideWidth,SlideY+BorderWidth,FrameColor);
	LCDFill(SlideX+BorderWidth,SlideY+SlideHeight-BorderWidth,SlideX+BorderWidth+255+SlideWidth,SlideY+SlideHeight,FrameColor);
	LCDFill(SlideX,SlideY,SlideX+BorderWidth,SlideY+SlideHeight,FrameColor);
	LCDFill(SlideX+BorderWidth+255+SlideWidth,SlideY,SlideX+BorderWidth*2+255+SlideWidth,SlideY+SlideHeight,FrameColor);
	
	LCDFill(SlideX+BorderWidth,SlideY+BorderWidth,SlideX+BorderWidth+Vol,SlideY+SlideHeight-BorderWidth,BackgroundColor);
	LCDFill(SlideX+BorderWidth+Vol+SlideWidth,SlideY+BorderWidth,SlideX+BorderWidth+255+SlideWidth,SlideY+SlideHeight-BorderWidth,BackgroundColor);
	
	LCDFill(SlideX+BorderWidth+Vol,SlideY+BorderWidth,SlideX+BorderWidth+Vol+SlideWidth,SlideY+SlideHeight-BorderWidth,SlideColor);	
}
