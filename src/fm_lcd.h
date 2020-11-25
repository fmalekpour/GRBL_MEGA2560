//LCD Functions Developed by electroSome
#include <avr/io.h>

#define eS_PORTA0 0
#define eS_PORTA1 1
#define eS_PORTA2 2
#define eS_PORTA3 3
#define eS_PORTA4 4
#define eS_PORTA5 5
#define eS_PORTA6 6
#define eS_PORTA7 7
#define eS_PORTB0 10
#define eS_PORTB1 11
#define eS_PORTB2 12
#define eS_PORTB3 13
#define eS_PORTB4 14
#define eS_PORTB5 15
#define eS_PORTB6 16
#define eS_PORTB7 17
#define eS_PORTC0 20
#define eS_PORTC1 21
#define eS_PORTC2 22
#define eS_PORTC3 23
#define eS_PORTC4 24
#define eS_PORTC5 25
#define eS_PORTC6 26
#define eS_PORTC7 27
#define eS_PORTD0 30
#define eS_PORTD1 31
#define eS_PORTD2 32
#define eS_PORTD3 33
#define eS_PORTD4 34
#define eS_PORTD5 35
#define eS_PORTD6 36
#define eS_PORTD7 37

#define eS_PORTE0 40
#define eS_PORTE1 41
#define eS_PORTE2 42
#define eS_PORTE3 43
#define eS_PORTE4 44
#define eS_PORTE5 45
#define eS_PORTE6 46
#define eS_PORTE7 47

#define eS_PORTF0 50
#define eS_PORTF1 51
#define eS_PORTF2 52
#define eS_PORTF3 53
#define eS_PORTF4 54
#define eS_PORTF5 55
#define eS_PORTF6 56
#define eS_PORTF7 57

#define eS_PORTG0 60
#define eS_PORTG1 61
#define eS_PORTG2 62
#define eS_PORTG3 63
#define eS_PORTG4 64
#define eS_PORTG5 65
#define eS_PORTG6 66
#define eS_PORTG7 67

#define eS_PORTH0 70
#define eS_PORTH1 71
#define eS_PORTH2 72
#define eS_PORTH3 73
#define eS_PORTH4 74
#define eS_PORTH5 75
#define eS_PORTH6 76
#define eS_PORTH7 77

#define eS_PORTJ0 80
#define eS_PORTJ1 81
#define eS_PORTJ2 82
#define eS_PORTJ3 83
#define eS_PORTJ4 84
#define eS_PORTJ5 85
#define eS_PORTJ6 86
#define eS_PORTJ7 87

#define eS_PORTK0 90
#define eS_PORTK1 91
#define eS_PORTK2 92
#define eS_PORTK3 93
#define eS_PORTK4 94
#define eS_PORTK5 95
#define eS_PORTK6 96
#define eS_PORTK7 97

#define eS_PORTL0 100
#define eS_PORTL1 101
#define eS_PORTL2 102
#define eS_PORTL3 103
#define eS_PORTL4 104
#define eS_PORTL5 105
#define eS_PORTL6 106
#define eS_PORTL7 107


#ifndef FM_LCD_D0
#define FM_LCD_D0 eS_PORTA0
#define FM_LCD_D1 eS_PORTA1
#define FM_LCD_D2 eS_PORTA2
#define FM_LCD_D3 eS_PORTA3
#endif


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

#include<util/delay.h>

void pinChange(int a, int b)
{
	if(b == 0)
	{
		if(a == eS_PORTA0)	      PORTA &= ~(1<<PA0);
		else if(a == eS_PORTA1)	  PORTA &= ~(1<<PA1); 
		else if(a == eS_PORTA2)	  PORTA &= ~(1<<PA2);
		else if(a == eS_PORTA3)	  PORTA &= ~(1<<PA3); 
		else if(a == eS_PORTA4)	  PORTA &= ~(1<<PA4); 
		else if(a == eS_PORTA5)	  PORTA &= ~(1<<PA5); 
		else if(a == eS_PORTA6)	  PORTA &= ~(1<<PA6);  
		else if(a == eS_PORTA7)	  PORTA &= ~(1<<PA7);

		else if(a == eS_PORTB0)	  PORTB &= ~(1<<PB0);  
		else if(a == eS_PORTB1)	  PORTB &= ~(1<<PB1);
		else if(a == eS_PORTB2)	  PORTB &= ~(1<<PB2);  
		else if(a == eS_PORTB3)	  PORTB &= ~(1<<PB3);  
		else if(a == eS_PORTB4)	  PORTB &= ~(1<<PB4);  
		else if(a == eS_PORTB5)	  PORTB &= ~(1<<PB5);  
		else if(a == eS_PORTB6)	  PORTB &= ~(1<<PB6);  
		else if(a == eS_PORTB7)	  PORTB &= ~(1<<PB7);

		else if(a == eS_PORTC0)	  PORTC &= ~(1<<PC0);   
		else if(a == eS_PORTC1)	  PORTC &= ~(1<<PC1); 
		else if(a == eS_PORTC2)	  PORTC &= ~(1<<PC2);
		else if(a == eS_PORTC3)	  PORTC &= ~(1<<PC3);   
		else if(a == eS_PORTC4)	  PORTC &= ~(1<<PC4);  
		else if(a == eS_PORTC5)	  PORTC &= ~(1<<PC5);  
        else if(a == eS_PORTC6)   PORTC &= ~(1<<PC6);		
		else if(a == eS_PORTC7)	  PORTC &= ~(1<<PC7);

		else if(a == eS_PORTD0)	  PORTD &= ~(1<<PD0);
		else if(a == eS_PORTD1)	  PORTD &= ~(1<<PD1);
		else if(a == eS_PORTD2)	  PORTD &= ~(1<<PD2);
		else if(a == eS_PORTD3)	  PORTD &= ~(1<<PD3);
		else if(a == eS_PORTD4)	  PORTD &= ~(1<<PD4);
		else if(a == eS_PORTD5)	  PORTD &= ~(1<<PD5);
		else if(a == eS_PORTD6)	  PORTD &= ~(1<<PD6);
		else if(a == eS_PORTD7)	  PORTD &= ~(1<<PD7);

		else if(a == eS_PORTE0)	  PORTE &= ~(1<<PE0);
		else if(a == eS_PORTE1)	  PORTE &= ~(1<<PE1);
		else if(a == eS_PORTE2)	  PORTE &= ~(1<<PE2);
		else if(a == eS_PORTE3)	  PORTE &= ~(1<<PE3);
		else if(a == eS_PORTE4)	  PORTE &= ~(1<<PE4);
		else if(a == eS_PORTE5)	  PORTE &= ~(1<<PE5);
		else if(a == eS_PORTE6)	  PORTE &= ~(1<<PE6);
		else if(a == eS_PORTE7)	  PORTE &= ~(1<<PE7);

		else if(a == eS_PORTF0)	  PORTF &= ~(1<<PF0);
		else if(a == eS_PORTF1)	  PORTF &= ~(1<<PF1);
		else if(a == eS_PORTF2)	  PORTF &= ~(1<<PF2);
		else if(a == eS_PORTF3)	  PORTF &= ~(1<<PF3);
		else if(a == eS_PORTF4)	  PORTF &= ~(1<<PF4);
		else if(a == eS_PORTF5)	  PORTF &= ~(1<<PF5);
		else if(a == eS_PORTF6)	  PORTF &= ~(1<<PF6);
		else if(a == eS_PORTF7)	  PORTF &= ~(1<<PF7);

		else if(a == eS_PORTG0)	  PORTG &= ~(1<<PG0);
		else if(a == eS_PORTG1)	  PORTG &= ~(1<<PG1);
		else if(a == eS_PORTG2)	  PORTG &= ~(1<<PG2);
		else if(a == eS_PORTG3)	  PORTG &= ~(1<<PG3);
		else if(a == eS_PORTG4)	  PORTG &= ~(1<<PG4);
		else if(a == eS_PORTG5)	  PORTG &= ~(1<<PG5);
//		else if(a == eS_PORTG6)	  PORTG &= ~(1<<PG6);
//		else if(a == eS_PORTG7)	  PORTG &= ~(1<<PG7);

		else if(a == eS_PORTH0)	  PORTH &= ~(1<<PH0);
		else if(a == eS_PORTH1)	  PORTH &= ~(1<<PH1);
		else if(a == eS_PORTH2)	  PORTH &= ~(1<<PH2);
		else if(a == eS_PORTH3)	  PORTH &= ~(1<<PH3);
		else if(a == eS_PORTH4)	  PORTH &= ~(1<<PH4);
		else if(a == eS_PORTH5)	  PORTH &= ~(1<<PH5);
		else if(a == eS_PORTH6)	  PORTH &= ~(1<<PH6);
		else if(a == eS_PORTH7)	  PORTH &= ~(1<<PH7);

		else if(a == eS_PORTJ0)	  PORTJ &= ~(1<<PJ0);
		else if(a == eS_PORTJ1)	  PORTJ &= ~(1<<PJ1);
		else if(a == eS_PORTJ2)	  PORTJ &= ~(1<<PJ2);
		else if(a == eS_PORTJ3)	  PORTJ &= ~(1<<PJ3);
		else if(a == eS_PORTJ4)	  PORTJ &= ~(1<<PJ4);
		else if(a == eS_PORTJ5)	  PORTJ &= ~(1<<PJ5);
		else if(a == eS_PORTJ6)	  PORTJ &= ~(1<<PJ6);
		else if(a == eS_PORTJ7)	  PORTJ &= ~(1<<PJ7);

		else if(a == eS_PORTK0)	  PORTK &= ~(1<<PK0);
		else if(a == eS_PORTK1)	  PORTK &= ~(1<<PK1);
		else if(a == eS_PORTK2)	  PORTK &= ~(1<<PK2);
		else if(a == eS_PORTK3)	  PORTK &= ~(1<<PK3);
		else if(a == eS_PORTK4)	  PORTK &= ~(1<<PK4);
		else if(a == eS_PORTK5)	  PORTK &= ~(1<<PK5);
		else if(a == eS_PORTK6)	  PORTK &= ~(1<<PK6);
		else if(a == eS_PORTK7)	  PORTK &= ~(1<<PK7);

		else if(a == eS_PORTL0)	  PORTL &= ~(1<<PL0);
		else if(a == eS_PORTL1)	  PORTL &= ~(1<<PL1);
		else if(a == eS_PORTL2)	  PORTL &= ~(1<<PL2);
		else if(a == eS_PORTL3)	  PORTL &= ~(1<<PL3);
		else if(a == eS_PORTL4)	  PORTL &= ~(1<<PL4);
		else if(a == eS_PORTL5)	  PORTL &= ~(1<<PL5);
		else if(a == eS_PORTL6)	  PORTL &= ~(1<<PL6);
		else if(a == eS_PORTL7)	  PORTL &= ~(1<<PL7);

	}
	else
	{
		if(a == eS_PORTA0)	      PORTA |= (1<<PA0);
		else if(a == eS_PORTA1)	  PORTA |= (1<<PA1);
		else if(a == eS_PORTA2)	  PORTA |= (1<<PA2);
		else if(a == eS_PORTA3)	  PORTA |= (1<<PA3);
		else if(a == eS_PORTA4)	  PORTA |= (1<<PA4);
		else if(a == eS_PORTA5)	  PORTA |= (1<<PA5);
		else if(a == eS_PORTA6)	  PORTA |= (1<<PA6);
		else if(a == eS_PORTA7)	  PORTA |= (1<<PA7);

		else if(a == eS_PORTB0)	  PORTB |= (1<<PB0);
		else if(a == eS_PORTB1)	  PORTB |= (1<<PB1);
		else if(a == eS_PORTB2)	  PORTB |= (1<<PB2);
		else if(a == eS_PORTB3)	  PORTB |= (1<<PB3);
		else if(a == eS_PORTB4)	  PORTB |= (1<<PB4);
		else if(a == eS_PORTB5)	  PORTB |= (1<<PB5);
		else if(a == eS_PORTB6)	  PORTB |= (1<<PB6);
		else if(a == eS_PORTB7)	  PORTB |= (1<<PB7);

		else if(a == eS_PORTC0)	  PORTC |= (1<<PC0);
		else if(a == eS_PORTC1)	  PORTC |= (1<<PC1);
		else if(a == eS_PORTC2)	  PORTC |= (1<<PC2);
		else if(a == eS_PORTC3)	  PORTC |= (1<<PC3);
		else if(a == eS_PORTC4)	  PORTC |= (1<<PC4);
		else if(a == eS_PORTC5)	  PORTC |= (1<<PC5);
		else if(a == eS_PORTC6)	  PORTC |= (1<<PC6);  
		else if(a == eS_PORTC7)	  PORTC |= (1<<PC7);

		else if(a == eS_PORTD0)	  PORTD |= (1<<PD0);
		else if(a == eS_PORTD1)	  PORTD |= (1<<PD1);
		else if(a == eS_PORTD2)	  PORTD |= (1<<PD2);
		else if(a == eS_PORTD3)	  PORTD |= (1<<PD3);
		else if(a == eS_PORTD4)	  PORTD |= (1<<PD4);
		else if(a == eS_PORTD5)	  PORTD |= (1<<PD5);
		else if(a == eS_PORTD6)	  PORTD |= (1<<PD6);
		else if(a == eS_PORTD7)	  PORTD |= (1<<PD7);
		
		else if(a == eS_PORTE0)	  PORTE |= (1<<PE0);
		else if(a == eS_PORTE1)	  PORTE |= (1<<PE1);
		else if(a == eS_PORTE2)	  PORTE |= (1<<PE2);
		else if(a == eS_PORTE3)	  PORTE |= (1<<PE3);
		else if(a == eS_PORTE4)	  PORTE |= (1<<PE4);
		else if(a == eS_PORTE5)	  PORTE |= (1<<PE5);
		else if(a == eS_PORTE6)	  PORTE |= (1<<PE6);
		else if(a == eS_PORTE7)	  PORTE |= (1<<PE7);
		
		else if(a == eS_PORTF0)	  PORTF |= (1<<PF0);
		else if(a == eS_PORTF1)	  PORTF |= (1<<PF1);
		else if(a == eS_PORTF2)	  PORTF |= (1<<PF2);
		else if(a == eS_PORTF3)	  PORTF |= (1<<PF3);
		else if(a == eS_PORTF4)	  PORTF |= (1<<PF4);
		else if(a == eS_PORTF5)	  PORTF |= (1<<PF5);
		else if(a == eS_PORTF6)	  PORTF |= (1<<PF6);
		else if(a == eS_PORTF7)	  PORTF |= (1<<PF7);
		
		else if(a == eS_PORTG0)	  PORTG |= (1<<PG0);
		else if(a == eS_PORTG1)	  PORTG |= (1<<PG1);
		else if(a == eS_PORTG2)	  PORTG |= (1<<PG2);
		else if(a == eS_PORTG3)	  PORTG |= (1<<PG3);
		else if(a == eS_PORTG4)	  PORTG |= (1<<PG4);
		else if(a == eS_PORTG5)	  PORTG |= (1<<PG5);
//		else if(a == eS_PORTG6)	  PORTG |= (1<<PG6);
//		else if(a == eS_PORTG7)	  PORTG |= (1<<PG7);
		
		else if(a == eS_PORTH0)	  PORTH |= (1<<PH0);
		else if(a == eS_PORTH1)	  PORTH |= (1<<PH1);
		else if(a == eS_PORTH2)	  PORTH |= (1<<PH2);
		else if(a == eS_PORTH3)	  PORTH |= (1<<PH3);
		else if(a == eS_PORTH4)	  PORTH |= (1<<PH4);
		else if(a == eS_PORTH5)	  PORTH |= (1<<PH5);
		else if(a == eS_PORTH6)	  PORTH |= (1<<PH6);
		else if(a == eS_PORTH7)	  PORTH |= (1<<PH7);
		
		else if(a == eS_PORTJ0)	  PORTJ |= (1<<PJ0);
		else if(a == eS_PORTJ1)	  PORTJ |= (1<<PJ1);
		else if(a == eS_PORTJ2)	  PORTJ |= (1<<PJ2);
		else if(a == eS_PORTJ3)	  PORTJ |= (1<<PJ3);
		else if(a == eS_PORTJ4)	  PORTJ |= (1<<PJ4);
		else if(a == eS_PORTJ5)	  PORTJ |= (1<<PJ5);
		else if(a == eS_PORTJ6)	  PORTJ |= (1<<PJ6);
		else if(a == eS_PORTJ7)	  PORTJ |= (1<<PJ7);
		
		else if(a == eS_PORTK0)	  PORTK |= (1<<PK0);
		else if(a == eS_PORTK1)	  PORTK |= (1<<PK1);
		else if(a == eS_PORTK2)	  PORTK |= (1<<PK2);
		else if(a == eS_PORTK3)	  PORTK |= (1<<PK3);
		else if(a == eS_PORTK4)	  PORTK |= (1<<PK4);
		else if(a == eS_PORTK5)	  PORTK |= (1<<PK5);
		else if(a == eS_PORTK6)	  PORTK |= (1<<PK6);
		else if(a == eS_PORTK7)	  PORTK |= (1<<PK7);
		
		else if(a == eS_PORTL0)	  PORTL |= (1<<PL0);
		else if(a == eS_PORTL1)	  PORTL |= (1<<PL1);
		else if(a == eS_PORTL2)	  PORTL |= (1<<PL2);
		else if(a == eS_PORTL3)	  PORTL |= (1<<PL3);
		else if(a == eS_PORTL4)	  PORTL |= (1<<PL4);
		else if(a == eS_PORTL5)	  PORTL |= (1<<PL5);
		else if(a == eS_PORTL6)	  PORTL |= (1<<PL6);
		else if(a == eS_PORTL7)	  PORTL |= (1<<PL7);
		
	}
}



//LCD 8 Bit Interfacing Functions
void Lcd8_Port(uint8_t a)
{
	pinChange(FM_LCD_D0,(a & 1));
	pinChange(FM_LCD_D1,(a & 2));
	pinChange(FM_LCD_D2,(a & 4));
	pinChange(FM_LCD_D3,(a & 8));
	pinChange(FM_LCD_D4,(a & 16));
	pinChange(FM_LCD_D5,(a & 32));
	pinChange(FM_LCD_D6,(a & 64));
	pinChange(FM_LCD_D7,(a & 128));
}

void Lcd8_Cmd(uint8_t a)
{
	pinChange(FM_LCD_RS,0);             // => RS = 0
	Lcd8_Port(a);             //Data transfer
	pinChange(FM_LCD_EN,1);             // => E = 1
	_delay_ms(1);
	pinChange(FM_LCD_EN,0);             // => E = 0
	_delay_ms(1);
}

void Lcd8_Clear()
{
	Lcd8_Cmd(1);
}

void Lcd8_Set_Cursor(char col, char row)
{
	static uint8_t offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	Lcd8_Cmd(0x80 | (col + offsets[row]));
/*
	if(a == 1)
		Lcd8_Cmd(0x80 + b);
	else if(a == 2)
		Lcd8_Cmd(0xC0 + b);
*/
}

void Lcd8_Init()
{

	pinChange(FM_LCD_RS,0);
	pinChange(FM_LCD_EN,0);
	_delay_ms(20);
	///////////// Reset process from datasheet /////////
	Lcd8_Cmd(0x30);
	_delay_ms(5);
	Lcd8_Cmd(0x30);
	_delay_ms(1);
	Lcd8_Cmd(0x30);
	_delay_ms(10);
	/////////////////////////////////////////////////////
	Lcd8_Cmd(0x38);    //function set
	Lcd8_Cmd(0x0C);    //display on,cursor off,blink off
	Lcd8_Cmd(0x01);    //clear display
	Lcd8_Cmd(0x06);    //entry mode, set increment
}

void Lcd8_Write_Char(char a)
{
	pinChange(FM_LCD_RS,1);             // => RS = 1
	Lcd8_Port(a);             //Data transfer
	pinChange(FM_LCD_EN,1);             // => E = 1
//	_delay_ms(1);
	_delay_ms(0.3);
	pinChange(FM_LCD_EN,0);             // => E = 04
//	_delay_ms(1);
	_delay_ms(0.3);
}

void Lcd8_Write_String(char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
		Lcd8_Write_Char(a[i]);
}

void Lcd8_Shift_Right()
{
	Lcd8_Cmd(0x1C);
}

void Lcd8_Shift_Left()
{
	Lcd8_Cmd(0x18);
}
//End LCD 8 Bit Interfacing Functions

//LCD 4 Bit Interfacing Functions

void Lcd4_Port(uint8_t a)
{
	pinChange(FM_LCD_D4,(a & 1));
	pinChange(FM_LCD_D5,(a & 2));
	pinChange(FM_LCD_D6,(a & 4));
	pinChange(FM_LCD_D7,(a & 8));
}
void Lcd4_Cmd(uint8_t a)
{
	pinChange(FM_LCD_RS,0);             // => RS = 0
	Lcd4_Port(a);
	pinChange(FM_LCD_EN,1);            // => E = 1
	_delay_ms(1);
	pinChange(FM_LCD_EN,0);             // => E = 0
	_delay_ms(1);
}
void Lcd4_Cmd_Full(uint8_t a)
{
	pinChange(FM_LCD_RS,0);             // => RS = 0
	Lcd4_Port((a & 0xF0)>>4);
	pinChange(FM_LCD_EN,1);            // => E = 1
	_delay_ms(1);
	pinChange(FM_LCD_EN,0);             // => E = 0
	_delay_ms(1);

	Lcd4_Port((a & 0x0F));
	pinChange(FM_LCD_EN,1);            // => E = 1
	_delay_ms(1);
	pinChange(FM_LCD_EN,0);             // => E = 0
	_delay_ms(1);
}

void Lcd4_Clear()
{
	Lcd4_Cmd(0);
	Lcd4_Cmd(1);
}

void Lcd4_Set_Cursor(uint8_t a, uint8_t b)
{
	char temp,z,y;
	if(a == 1)
	{
		temp = 0x80 + b;
		z = temp>>4;
		y = (0x80+b) & 0x0F;
		Lcd4_Cmd(z);
		Lcd4_Cmd(y);
	}
	else if(a == 2)
	{
		temp = 0xC0 + b;
		z = temp>>4;
		y = (0xC0+b) & 0x0F;
		Lcd4_Cmd(z);
		Lcd4_Cmd(y);
	}
}

void Lcd4_Init()
{
	pinChange(FM_LCD_RS,0);
	pinChange(FM_LCD_EN,0);
/*	
	_delay_ms(20);

	
	Lcd4_Port(0x00);
	_delay_ms(20);
	Lcd4_Cmd(0x03);
	_delay_ms(5);
	Lcd4_Cmd(0x03);
	_delay_ms(11);
	Lcd4_Cmd(0x03);
*/

//	pinChange(FM_LCD_D4,0);
//	pinChange(FM_LCD_D5,(a & 2));
//	pinChange(FM_LCD_D6,(a & 4));
//	pinChange(FM_LCD_D7,(a & 8));


	Lcd4_Port(0x00);
	_delay_ms(20);
	///////////// Reset process from datasheet /////////

	Lcd4_Cmd(0x03);
	_delay_ms(5);
	Lcd4_Cmd(0x03);
	_delay_ms(11);
	Lcd4_Cmd(0x03);
	/////////////////////////////////////////////////////
	_delay_ms(1);
	Lcd4_Cmd(0x02);

	Lcd4_Cmd_Full(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
//	Lcd4_Cmd_Full(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS);
//	Lcd4_Cmd_Full(LCD_DISPLAYCONTROL | LCD_CURSOROFF | LCD_BLINKOFF);


/*
	Lcd4_Cmd(0x02);
	Lcd4_Cmd(0x08);
*/
	Lcd4_Cmd(0x00);
	Lcd4_Cmd(0x0C);
	Lcd4_Cmd(0x00);
	Lcd4_Cmd(0x06);

/*
	Lcd4_Cmd(0x02);
	Lcd4_Cmd(0x08);
	Lcd4_Cmd(0x00);
	Lcd4_Cmd(0x0C);
	Lcd4_Cmd(0x00);
	Lcd4_Cmd(0x06);
*/


//	Lcd4_Cmd(0x01);
//	_delay_ms(5);

}

void Lcd4_Write_Char(uint8_t a)
{
	char temp,y;
	temp = a&0x0F;
	y = a&0xF0;
	pinChange(FM_LCD_RS,1);             // => RS = 1
	Lcd4_Port(y>>4);             //Data transfer
	pinChange(FM_LCD_EN,1);
	_delay_ms(1);
	pinChange(FM_LCD_EN,0);
	_delay_ms(1);
	Lcd4_Port(temp);
	pinChange(FM_LCD_EN,1);
	_delay_ms(1);
	pinChange(FM_LCD_EN,0);
	_delay_ms(1);
}

void Lcd4_Write_String(uint8_t *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	Lcd4_Write_Char(a[i]);
}

void Lcd4_Shift_Right()
{
	Lcd4_Cmd(0x01);
	Lcd4_Cmd(0x0C);
}

void Lcd4_Shift_Left()
{
	Lcd4_Cmd(0x01);
	Lcd4_Cmd(0x08);
}
//End LCD 4 Bit Interfacing Functions
