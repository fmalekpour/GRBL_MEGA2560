#ifndef fm_lcd_h
#define fm_lcd_h

#include "grbl.h"
#include "fm_system_data.h"

#define FM_LCD_WIDTH	20
#define FM_LCD_HEIGHT	4

#define FM_LCD_PIN_RS		23
#define FM_LCD_PIN_EN		25
#define FM_LCD_PIN_D4		27
#define FM_LCD_PIN_D5		29
#define FM_LCD_PIN_D6		31
#define FM_LCD_PIN_D7		33

#define FM_LCD_PIN_RS_DDR	DDRA
#define FM_LCD_PIN_EN_DDR	DDRA
#define FM_LCD_PIN_D4_DDR	DDRA
#define FM_LCD_PIN_D5_DDR	DDRA
#define FM_LCD_PIN_D6_DDR	DDRC
#define FM_LCD_PIN_D7_DDR	DDRC

#define FM_LCD_PIN_RS_PORT	PORTA
#define FM_LCD_PIN_EN_PORT	PORTA
#define FM_LCD_PIN_D4_PORT	PORTA
#define FM_LCD_PIN_D5_PORT	PORTA
#define FM_LCD_PIN_D6_PORT	PORTC
#define FM_LCD_PIN_D7_PORT	PORTC

#define FM_LCD_PIN_RS_BIT	PA1
#define FM_LCD_PIN_EN_BIT	PA3
#define FM_LCD_PIN_D4_BIT	PA5
#define FM_LCD_PIN_D5_BIT	PA7
#define FM_LCD_PIN_D6_BIT	PC6
#define FM_LCD_PIN_D7_BIT	PC4


class FMLcd
{
	private:
		uint8_t mLcdWidth;
		uint8_t mLcdHeight;
		uint8_t mBufferCount;
		char *mBuffer;
		uint8_t mBufferIndex;
		char mUpdateLcdBuffer[26];
		char mPrintFuncsBuffer[16];


		void initLcd();
		void changePortsToOutput();
		void pinChange(int a, int b);
		void setValue(uint8_t a);
		void sendCommand(uint8_t a);
		void sendCommandFull(uint8_t a);

		void printLeft(const char *str, uint8_t col, uint8_t row);
		void printState(char *out, uint16_t state, int outSize);
		void printLong(char *out, long n, int outSize);
		void printFloat(char *out, float n, uint8_t decimal_places, int outSize);




	public:

		uint8_t mTimerStage;

		FMLcd(uint8_t lcdWidth, uint8_t lcdHeight);

		uint8_t runCycleFromStage();
		void updateLcd(FMSystemData *systemData);

};

#endif
