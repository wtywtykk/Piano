#define H_VIEW
#ifdef H_VIEW
	#define X_MAX_PIXEL 320
	#define Y_MAX_PIXEL 240
#else
	#define X_MAX_PIXEL 240
	#define Y_MAX_PIXEL 320
#endif

#define RED			0xf800
#define GREEN		0x07e0
#define BLUE		0x001f
#define WHITE		0xffff
#define BLACK		0x0000
#define YELLOW	0xFFE0
#define GRAY0		0xEF7D	//»ÒÉ«0	11101 111011 11101
#define GRAY1		0x8410	//»ÒÉ«1	10000 100000 10000
#define GRAY2		0x4208	//»ÒÉ«2	01000 010000 01000

#define LCDCTRL				GPIOD
#define LCDRS					GPIO_Pin_8	//MCU_PD8				TFT --RS/DC
#define LCDCS					GPIO_Pin_9	//MCU_PD9				TFT --CS
#define LCDRST				GPIO_Pin_10	//MCU_PD10				TFT --RST
#define LCDBG					GPIO_Pin_11	//MCU_PD11				TFT --LED
#define LCDSCK				GPIO_Pin_5	//MCU_PA5				TFT --SCK
#define LCDMISO				GPIO_Pin_6	//MCU_PA6 MISO	TFT --MISO
#define LCDMOSI				GPIO_Pin_7	//MCU_PA7	MOSI	TFT --MOSI

void LCDInit(void);
void LCDBGEnable(u8 Enable);
void LCDDrawPoint(u16 x,u16 y,u16 Data);
void LCDFill(u16 x_start,u16 y_start,u16 x_end,u16 y_end,u16 Color);
void LCDClear(u16 Color);
void LCDBitblt(u16 x_start,u16 y_start,u16 x_end,u16 y_end,u16* source);
void LCDDrawText(u16 x, u16 y, u16 fc, u16 bc, const char *s);
