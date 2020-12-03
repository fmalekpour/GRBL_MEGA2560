
#include "fm_lcd.h"

#define LCD_CLEARDISPLAY   0x01
#define LCD_RETURNHOME     0x02
#define LCD_ENTRYMODESET   0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT    0x10
#define LCD_FUNCTIONSET    0x20
#define LCD_SETCGRAMADDR   0x40
#define LCD_SETDDRAMADDR   0x80

#define LCD_DISPLAYON  0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON   0x02
#define LCD_CURSOROFF  0x00
#define LCD_BLINKON    0x01
#define LCD_BLINKOFF   0x00


#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE    0x08
#define LCD_1LINE    0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS  0x00


FMLcd::FMLcd(uint8_t lcdWidth, uint8_t lcdHeight)
{
	this->mLcdWidth = lcdWidth;
	this->mLcdHeight = lcdHeight;
	this->mBufferCount = this->mLcdWidth*this->mLcdHeight;
	this->mBuffer = (char*)malloc(this->mBufferCount);

	for(uint8_t i=0;i<this->mBufferCount;i++)
	{
		this->mBuffer[i] = ' ';
	}
	this->mBufferIndex = 0;
	this->mTimerStage = 0;



	_delay_ms(50);
	// change port mode to OUTPUT
	this->changePortsToOutput();

	_delay_ms(100);
	this->initLcd();

}

void FMLcd::initLcd()
{
	this->pinChange(FM_LCD_PIN_RS,0);
	this->pinChange(FM_LCD_PIN_EN,0);

	this->setValue(0x00);
	_delay_ms(20);
	///////////// Reset process from datasheet /////////

	this->sendCommand(0x03);
	_delay_ms(5);
	this->sendCommand(0x03);
	_delay_ms(11);
	this->sendCommand(0x03);
	/////////////////////////////////////////////////////
	_delay_ms(1);
	this->sendCommand(0x02);

	this->sendCommandFull(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);

	this->sendCommand(0x00);
	this->sendCommand(0x0C);
	this->sendCommand(0x00);
	this->sendCommand(0x06);
}

void FMLcd::changePortsToOutput()
{
	FM_LCD_PIN_RS_DDR |= (1 << FM_LCD_PIN_RS_BIT);
	FM_LCD_PIN_EN_DDR |= (1 << FM_LCD_PIN_EN_BIT);
	FM_LCD_PIN_D4_DDR |= (1 << FM_LCD_PIN_D4_BIT);
	FM_LCD_PIN_D5_DDR |= (1 << FM_LCD_PIN_D5_BIT);
	FM_LCD_PIN_D6_DDR |= (1 << FM_LCD_PIN_D6_BIT);
	FM_LCD_PIN_D7_DDR |= (1 << FM_LCD_PIN_D7_BIT);
}


void FMLcd::pinChange(int a, int b)
{
	if(b == 0)
	{
		switch (a)
		{
			case FM_LCD_PIN_RS:		FM_LCD_PIN_RS_PORT &= ~(1<<FM_LCD_PIN_RS_BIT);	break;
			case FM_LCD_PIN_EN:		FM_LCD_PIN_EN_PORT &= ~(1<<FM_LCD_PIN_EN_BIT);	break;
			case FM_LCD_PIN_D4:		FM_LCD_PIN_D4_PORT &= ~(1<<FM_LCD_PIN_D4_BIT);	break;
			case FM_LCD_PIN_D5:		FM_LCD_PIN_D5_PORT &= ~(1<<FM_LCD_PIN_D5_BIT);	break;
			case FM_LCD_PIN_D6:		FM_LCD_PIN_D6_PORT &= ~(1<<FM_LCD_PIN_D6_BIT);	break;
			case FM_LCD_PIN_D7:		FM_LCD_PIN_D7_PORT &= ~(1<<FM_LCD_PIN_D7_BIT);	break;
		}
	}
	else
	{
		switch (a)
		{
			case FM_LCD_PIN_RS:		FM_LCD_PIN_RS_PORT |= (1<<FM_LCD_PIN_RS_BIT);	break;
			case FM_LCD_PIN_EN:		FM_LCD_PIN_EN_PORT |= (1<<FM_LCD_PIN_EN_BIT);	break;
			case FM_LCD_PIN_D4:		FM_LCD_PIN_D4_PORT |= (1<<FM_LCD_PIN_D4_BIT);	break;
			case FM_LCD_PIN_D5:		FM_LCD_PIN_D5_PORT |= (1<<FM_LCD_PIN_D5_BIT);	break;
			case FM_LCD_PIN_D6:		FM_LCD_PIN_D6_PORT |= (1<<FM_LCD_PIN_D6_BIT);	break;
			case FM_LCD_PIN_D7:		FM_LCD_PIN_D7_PORT |= (1<<FM_LCD_PIN_D7_BIT);	break;
		}
	}
}

void FMLcd::setValue(uint8_t a)
{
	pinChange(FM_LCD_PIN_D4,(a & 1));
	pinChange(FM_LCD_PIN_D5,(a & 2));
	pinChange(FM_LCD_PIN_D6,(a & 4));
	pinChange(FM_LCD_PIN_D7,(a & 8));
}

void FMLcd::sendCommand(uint8_t a)
{
	this->pinChange(FM_LCD_PIN_RS, 0);
	this->setValue(a);
	this->pinChange(FM_LCD_PIN_EN, 1);
	_delay_ms(1);
	this->pinChange(FM_LCD_PIN_EN, 0);
	_delay_ms(1);
}

void FMLcd::sendCommandFull(uint8_t a)
{
	this->pinChange(FM_LCD_PIN_RS, 0);
	this->setValue((a & 0xF0)>>4);
	this->pinChange(FM_LCD_PIN_EN, 1);
	_delay_ms(1);
	this->pinChange(FM_LCD_PIN_EN, 0);
	_delay_ms(1);

	this->setValue((a & 0x0F));
	this->pinChange(FM_LCD_PIN_EN,1);
	_delay_ms(1);
	this->pinChange(FM_LCD_PIN_EN,0);
	_delay_ms(1);
}

uint8_t FMLcd::runCycleFromStage()
{
	switch (this->mTimerStage)
	{
		case 0:
			this->pinChange(FM_LCD_PIN_RS, 0);
			this->setValue((0x80) >> 4);
			this->pinChange(FM_LCD_PIN_EN, 1);
			return 1;
		case 1:
			this->pinChange(FM_LCD_PIN_EN, 0);
			return 2;

		case 2:
			this->setValue((0x80) & 0x0F);
			this->pinChange(FM_LCD_PIN_EN, 1);
			return 3;
		case 3:
			this->pinChange(FM_LCD_PIN_EN, 0);
			return 10;

		case 10:
			this->mBufferIndex = 0;
			return 20;

		case 20:
			this->pinChange(FM_LCD_PIN_RS, 1);
			this->setValue((this->mBuffer[this->mBufferIndex] & 0xF0) >> 4);
			this->pinChange(FM_LCD_PIN_EN, 1);
			return 21;
		case 21:
			this->pinChange(FM_LCD_PIN_EN, 0);
			return 30;


		case 30:
			this->setValue((this->mBuffer[this->mBufferIndex] & 0x0F));
			this->pinChange(FM_LCD_PIN_EN, 1);
			return 31;
		case 31:
			this->pinChange(FM_LCD_PIN_EN, 0);
			return 40;

		case 40:
			this->mBufferIndex++;
			if(this->mBufferIndex>=this->mBufferCount)
			{
				this->mBufferIndex=0;
				return 110;
			}
			else
			{
				return 20;
			}
			break;
	}
	return 110;
}

void FMLcd::updateLcd(FMSystemData *systemData)
{
	char *buf = this->mUpdateLcdBuffer;
/*
	buf[0] = '0'+fm_test_counter;
	buf[1] = 0;
	fm_test_counter++;
	if(fm_test_counter>=10)
		fm_test_counter = 0;

	fm_i2c_lcd_printLeft(buf,19,3);
*/


	buf[0] = 'X';
	buf[1] = ':';
	this->printFloat(buf+2, systemData->mSystemData.pos[0], 2, 7);
	this->printLeft(buf,0,0);

	buf[0] = 'Y';
	buf[1] = ':';
	this->printFloat(buf+2, systemData->mSystemData.pos[1], 2, 7);
	this->printLeft(buf,0,1);

	buf[0] = 'Z';
	buf[1] = ':';
	this->printFloat(buf+2, systemData->mSystemData.pos[2], 2, 7);
	this->printLeft(buf,11,0);

	buf[0] = 'F';
	buf[1] = ':';
	this->printFloat(buf+2, systemData->mSystemData.feedSpeed, 2, 7);
//	this->printLong(buf+2, systemData->mSystemData.feedSpeed, 7);
	this->printLeft(buf,11,1);

	buf[0] = 'S';
	buf[1] = 't';
	buf[2] = 'a';
	buf[3] = 't';
	buf[4] = 'e';
	buf[5] = ':';
	this->printState(buf+6, systemData->mSystemData.state, 14);
	this->printLeft(buf,0,2);
}

void FMLcd::printFloat(char *out, float n, uint8_t decimal_places, int outSize)
{
	uint8_t outx = 0;
	bool isNegative = false;
	if (n < 0) 
	{
		n = -n;
		isNegative = true;
	}
	memset(out, ' ', outSize);

	long n2 =  n*100;
	char *buf = this->mPrintFuncsBuffer;
	ltoa(n2, buf, 10);
	int s = strlen(buf);
	if(s==2)
	{
		buf[2] = buf[1];
		buf[1] = buf[0];
		buf[0] = '0';
		s++;
	}
	if(s==1)
	{
		buf[2] = buf[0];
		buf[1] = '0';
		buf[0] = '0';
		s+=2;
	}


	outx = outSize - 1 - s -1;
	if(outx<0)
	{
		out[0] = 'e'; out[1] = 0;
		return;
	}
	out[outx++] = (isNegative?'-':' ');
	for(int i=0;i<s-2;i++)
	{
		out[outx++] = buf[i];
	}
	out[outx++] = '.';
	for(int i=s-2;i<s;i++)
	{
		out[outx++] = buf[i];
	}
	out[outx++] = 0;
}

void FMLcd::printLong(char *out, long n, int outSize)
{
	uint8_t outx = 0;
	bool isNegative = false;
	if (n < 0) 
	{
		n = -n;
		isNegative = true;
	}
	memset(out, ' ', outSize);

	char *buf = this->mPrintFuncsBuffer;
	ltoa(n, buf, 10);
	int s = strlen(buf);
	if(s>outSize-1)
	{
		out[0] = 'e'; out[1] = 0;
		return;
	}
	outx = outSize - s -1;
	out[outx++] = (isNegative?'-':' ');
	for(int i=0;i<s;i++)
	{
		out[outx++] = buf[i];
	}
	out[outx++] = 0;
}

void FMLcd::printState(char *out, uint16_t state, int outSize)
{
	const char *buf;
	switch (state)
	{
		case 1:		buf = PSTR("Idle");				break;
		case 2:		buf = PSTR("Run");				break;
		case 3:		buf = PSTR("Can Resume");		break;
		case 4:		buf = PSTR("Hold");				break;
		case 5:		buf = PSTR("Jog");				break;
		case 6:		buf = PSTR("Home");				break;
		case 7:		buf = PSTR("Alarm");			break;
		case 8:		buf = PSTR("Check");			break;
		case 9:		buf = PSTR("Door Closed");		break;
		case 10:	buf = PSTR("Door Open");		break;
		case 11:	buf = PSTR("Retracting");		break;
		case 12:	buf = PSTR("Restoring");		break;
		case 13:	buf = PSTR("Sleep");			break;
		default:	buf = PSTR("UNKNOWN");			break;
	}
	memset(out, ' ', outSize);
	int s = strlen_P(buf);
	
	int outx = outSize - s;
	memcpy_P(out+outx, buf, s);
	
	out[outSize] = 0;
}

void FMLcd::printLeft(const char *str, uint8_t col, uint8_t row)
{
	if(row==2) 
		row=1;
	else if(row==1)
		row=2;

	if(col>=FM_LCD_WIDTH)
		col = FM_LCD_WIDTH-1;
	if(row>=FM_LCD_HEIGHT)
		row = FM_LCD_HEIGHT-1;

	uint16_t offset = (row*FM_LCD_WIDTH)+col;
	memcpy(this->mBuffer+offset, str, min((uint16_t)strlen(str), (uint16_t)(FM_LCD_WIDTH-col)));

}
