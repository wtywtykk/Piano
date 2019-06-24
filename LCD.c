#include "Device/Include/stm32f10x.h"	 // Device header
#include "LCD.h"
#include "Delay.h"
#include "FONT.h"

u16 SPI_WriteDat(u16 Dat)
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, Dat);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI1);
}

void LcdGPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = LCDRS|LCDCS|LCDRST|LCDBG;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LCDCTRL, &GPIO_InitStructure);
}

void SPIInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef	SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure SPI1 pins: SCK and MOSI */
	GPIO_InitStructure.GPIO_Pin = LCDSCK | LCDMOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin = LCDMISO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Enable SPI clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	/* SPI1 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	/* Enable SPI1	*/
	SPI_Cmd(SPI1, ENABLE);
}

void LCDReset(void)
{
	GPIO_ResetBits(LCDCTRL,LCDRST);
	Delay(100);
	GPIO_SetBits(LCDCTRL,LCDRST);
	Delay(50);
}

void LCDWriteIndex(u8 Index)
{
	GPIO_ResetBits(LCDCTRL,LCDRS);
	GPIO_ResetBits(LCDCTRL,LCDCS);
	SPI_WriteDat(Index);
	GPIO_SetBits(LCDCTRL,LCDCS);
}

void LCDWriteData(u8 Data)
{
	GPIO_SetBits(LCDCTRL,LCDRS);
	GPIO_ResetBits(LCDCTRL,LCDCS);
	SPI_WriteDat(Data);
	GPIO_SetBits(LCDCTRL,LCDCS);
}

void LCDWriteData16Bit(u16 Data)
{
	GPIO_SetBits(LCDCTRL,LCDRS);
	GPIO_ResetBits(LCDCTRL,LCDCS);
	SPI_DataSizeConfig(SPI1,SPI_DataSize_16b);
	SPI_WriteDat(Data);
	SPI_DataSizeConfig(SPI1,SPI_DataSize_8b);
	GPIO_SetBits(LCDCTRL,LCDCS);
}

void LCDWriteIndex16Bit(u16 Data)
{
	GPIO_ResetBits(LCDCTRL,LCDRS);
	GPIO_ResetBits(LCDCTRL,LCDCS);
	SPI_DataSizeConfig(SPI1,SPI_DataSize_16b);
	SPI_WriteDat(Data);
	SPI_DataSizeConfig(SPI1,SPI_DataSize_8b);
	GPIO_SetBits(LCDCTRL,LCDCS);
}

void LCDInit(void)
{
	LcdGPIO_Init();
	SPIInit();
	LCDReset();

	LCDWriteIndex(0xCB);
	LCDWriteData(0x39);
	LCDWriteData(0x2C);
	LCDWriteData(0x00);
	LCDWriteData(0x34);
	LCDWriteData(0x02);

	LCDWriteIndex(0xCF);
	LCDWriteData(0x00);
	LCDWriteData(0XC1);
	LCDWriteData(0X30);

	LCDWriteIndex(0xE8);
	LCDWriteData(0x85);
	LCDWriteData(0x00);
	LCDWriteData(0x78);

	LCDWriteIndex(0xEA);
	LCDWriteData(0x00);
	LCDWriteData(0x00);

	LCDWriteIndex(0xED);
	LCDWriteData(0x64);
	LCDWriteData(0x03);
	LCDWriteData(0X12);
	LCDWriteData(0X81);

	LCDWriteIndex(0xF7);
	LCDWriteData(0x20);

	LCDWriteIndex(0xC0);		//Power control
	LCDWriteData(0x23);	 //VRH[5:0]

	LCDWriteIndex(0xC1);		//Power control
	LCDWriteData(0x10);	 //SAP[2:0];BT[3:0]

	LCDWriteIndex(0xC5);		//VCM control
	LCDWriteData(0x3e); //对比度调节
	LCDWriteData(0x28);

	LCDWriteIndex(0xC7);		//VCM control2
	LCDWriteData(0x86);	//--

	LCDWriteIndex(0x36);		// Memory Access Control
#ifdef H_VIEW
	LCDWriteData(0x28); //C8		 //48 68竖屏//28 E8 横屏
#else
	LCDWriteData(0x48);
#endif

	LCDWriteIndex(0x3A);
	LCDWriteData(0x55);

	LCDWriteIndex(0xB1);
	LCDWriteData(0x00);
	LCDWriteData(0x18);

	LCDWriteIndex(0xB6);		// Display Function Control
	LCDWriteData(0x08);
	LCDWriteData(0x82);
	LCDWriteData(0x27);

	LCDWriteIndex(0xF2);		// 3Gamma Function Disable
	LCDWriteData(0x00);

	LCDWriteIndex(0x26);		//Gamma curve selected
	LCDWriteData(0x01);

	LCDWriteIndex(0xE0);		//Set Gamma
	LCDWriteData(0x0F);
	LCDWriteData(0x31);
	LCDWriteData(0x2B);
	LCDWriteData(0x0C);
	LCDWriteData(0x0E);
	LCDWriteData(0x08);
	LCDWriteData(0x4E);
	LCDWriteData(0xF1);
	LCDWriteData(0x37);
	LCDWriteData(0x07);
	LCDWriteData(0x10);
	LCDWriteData(0x03);
	LCDWriteData(0x0E);
	LCDWriteData(0x09);
	LCDWriteData(0x00);

	LCDWriteIndex(0XE1);		//Set Gamma
	LCDWriteData(0x00);
	LCDWriteData(0x0E);
	LCDWriteData(0x14);
	LCDWriteData(0x03);
	LCDWriteData(0x11);
	LCDWriteData(0x07);
	LCDWriteData(0x31);
	LCDWriteData(0xC1);
	LCDWriteData(0x48);
	LCDWriteData(0x08);
	LCDWriteData(0x0F);
	LCDWriteData(0x0C);
	LCDWriteData(0x31);
	LCDWriteData(0x36);
	LCDWriteData(0x0F);

	LCDWriteIndex(0x11);		//Exit Sleep
	Delay(120);

	LCDWriteIndex(0x29);		//Display on
	LCDWriteIndex(0x2c);

	LCDClear(WHITE);
}

void LCDBGEnable(u8 Enable)
{
	GPIO_WriteBit(LCDCTRL,LCDBG,Enable?Bit_SET:Bit_RESET);
}

void LCDSetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end)
{
	LCDWriteIndex(0x2a);
	LCDWriteData16Bit(x_start);
	LCDWriteData16Bit(x_end-1);
	LCDWriteIndex(0x2b);
	LCDWriteData16Bit(y_start);
	LCDWriteData16Bit(y_end-1);
	LCDWriteIndex(0x2c);
}

void LCDDrawPoint(u16 x,u16 y,u16 Data)
{
	LCDWriteIndex(0x2a);
	LCDWriteData16Bit(x);
	LCDWriteIndex(0x2b);
	LCDWriteData16Bit(y);
	LCDWriteIndex(0x2c);
	LCDWriteData16Bit(Data);
}

void LCDFill(u16 x_start,u16 y_start,u16 x_end,u16 y_end,u16 Color)
{
	unsigned int i,j;
	LCDSetRegion(x_start,y_start,x_end,y_end);
	GPIO_ResetBits(LCDCTRL,LCDCS);
	GPIO_SetBits(LCDCTRL,LCDRS);
	SPI_DataSizeConfig(SPI1,SPI_DataSize_16b);
	for(i=y_start;i<y_end;i++)
	{
		for(j=x_start;j<x_end;j++)
		{
			SPI_WriteDat(Color);
		}
	}
	SPI_DataSizeConfig(SPI1,SPI_DataSize_8b);
	GPIO_SetBits(LCDCTRL,LCDCS);
	LCDSetRegion(0,0,X_MAX_PIXEL,Y_MAX_PIXEL);
}

void LCDClear(u16 Color)
{
	 LCDFill(0,0,X_MAX_PIXEL,Y_MAX_PIXEL,Color);
}

void LCDBitblt(u16 x_start,u16 y_start,u16 x_end,u16 y_end,u16* source)
{
	unsigned int i,j;
	LCDSetRegion(x_start,y_start,x_end,y_end);
	GPIO_ResetBits(LCDCTRL,LCDCS);
	GPIO_SetBits(LCDCTRL,LCDRS);
	SPI_DataSizeConfig(SPI1,SPI_DataSize_16b);
	for(i=y_start;i<y_end;i++)
	{
		for(j=x_start;j<x_end;j++)
		{
			SPI_WriteDat(*(source++));
		}
	}
	SPI_DataSizeConfig(SPI1,SPI_DataSize_8b);
	GPIO_SetBits(LCDCTRL,LCDCS);
	LCDSetRegion(0,0,X_MAX_PIXEL,Y_MAX_PIXEL);
}

void LCDDrawText(u16 x, u16 y, u16 fc, u16 bc, const char *s)
{
	unsigned short x0=x,y0=y;

	if(!s)
	{
		return;
	}

	for(;*s;s++)
	{
		char chr=*s;
		int i=0;
		if(chr=='\r')
		{
			x0=x;
			y0+=16;
			if(*(s+1)=='\n')
			{
				s++;
			}
			continue;
		}
		
		if(chr>32)
		{
			chr-=32;
		}
		else
		{
			chr=0;
		}
		
		
		LCDSetRegion(x0,y0,x0+8,y0+16);
		GPIO_ResetBits(LCDCTRL,LCDCS);
		GPIO_SetBits(LCDCTRL,LCDRS);
		SPI_DataSizeConfig(SPI1,SPI_DataSize_16b);
		for(;i<16;i++)
		{
			int j=0;
			for(;j<8;j++)
			{
				if(asc16[chr*16+i]&(0x80>>j))
				{
					SPI_WriteDat(fc);
				}
				else
				{
					SPI_WriteDat(bc);
				}
			}
		}
		SPI_DataSizeConfig(SPI1,SPI_DataSize_8b);
		GPIO_SetBits(LCDCTRL,LCDCS);
		LCDSetRegion(0,0,X_MAX_PIXEL,Y_MAX_PIXEL);

		x0+=8;
	}
}
