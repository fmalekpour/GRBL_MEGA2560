
#include "grbl.h"
//#include <Wire.h>


/**
 * 32
 * 47
 * 45
 * 43
 * 41
 * 39
 * 37
 * 35
 * 33		*
 * 31		*
 * 29		*
 * 27		*
 * 25		*
 * 23		*
 * 17
 * 16
 * 
 */

#define FM_LCD_4		true
//#define FM_LCD_8		true

#define FM_LCD_WIDTH	20
#define FM_LCD_HEIGHT	4

#define FM_LCD_PIN_V0	32
#define FM_LCD_PIN_RS	47		//#		D42				23-PA1
#define FM_LCD_PIN_RW	45		
#define FM_LCD_PIN_E	43		//#		A5 D59			25-PA3
#define FM_LCD_PIN_D0	41		
#define FM_LCD_PIN_D1	39		
#define FM_LCD_PIN_D2	37		
#define FM_LCD_PIN_D3	35		
#define FM_LCD_PIN_D4	33		//#		A9 D63			27-PA5
#define FM_LCD_PIN_D5	31		//#		A10 D64			29-PA7
#define FM_LCD_PIN_D6	29		//#		A11 D65			31-PC6
#define FM_LCD_PIN_D7	27		//#		A12 D66			33-PC4

//#define FM_LCD_PIN_A	27
//#define FM_LCD_PIN_K	27
/*
(uint8_t rs, uint8_t rw, uint8_t enable,
		uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
		uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
*/

#ifdef FM_LCD_8

#define FM_LCD_D0 eS_PORTG0
#define FM_LCD_D1 eS_PORTG2
#define FM_LCD_D2 eS_PORTC0
#define FM_LCD_D3 eS_PORTC2
#define FM_LCD_D4 eS_PORTC4
#define FM_LCD_D5 eS_PORTC6
#define FM_LCD_D6 eS_PORTA7
#define FM_LCD_D7 eS_PORTA5
#define FM_LCD_RS eS_PORTL2
#define FM_LCD_EN eS_PORTL6

#endif

#ifdef FM_LCD_4

#define FM_LCD_D4 eS_PORTA5
#define FM_LCD_D5 eS_PORTA7
#define FM_LCD_D6 eS_PORTC6
#define FM_LCD_D7 eS_PORTC4
#define FM_LCD_RS eS_PORTA1
#define FM_LCD_EN eS_PORTA3

#endif

#include "fm_lcd.h"

/*
#include <Arduino.h>

char fm_i2c_buffer[1024];
int fm_i2c_buffer_index;
*/

//char fm_i2c_send_buffer[64];

typedef struct
{
	uint8_t structSize;
	uint8_t axisCount;
	uint8_t state;
	uint8_t posType;

	float feedSpeed;
	float pos[N_AXIS];

}FM_I2C_DATA;



void fm_i2c_receiveEvent();
void fm_i2c_printFloat(char *out, float n, uint8_t decimal_places, int outSize);
void fm_i2c_printLong(char *out, long n, int outSize);
void fm_i2c_updateLcd();
void fm_i2c_updateSystemData();

uint32_t fm_i2c_timer5_counter;
bool fm_i2c_lcd_needs_clean;
uint8_t fm_i2c_timer_stage;
FM_I2C_DATA fm_i2c_global_data;

const uint8_t fm_i2c_lcd_buffer_count = FM_LCD_WIDTH*FM_LCD_HEIGHT;
char fm_i2c_lcd_buffer[FM_LCD_WIDTH*FM_LCD_HEIGHT];
uint8_t fm_i2c_lcd_buffer_index;

void fm_i2c_init()
{
//	Wire.begin(4);
//	Wire.onRequest(fm_i2c_receiveEvent);

	for(uint8_t i=0;i<fm_i2c_lcd_buffer_count;i++)
	{
//		fm_i2c_lcd_buffer[i] = 'A'+(i % 26);
		fm_i2c_lcd_buffer[i] = ' ';
	}
	fm_i2c_lcd_buffer_index = 0;

/*
	fm_i2c_lcd_buffer[0] = 'T';
	fm_i2c_lcd_buffer[1] = 'E';
	fm_i2c_lcd_buffer[2] = 'S';
	fm_i2c_lcd_buffer[3] = 'T';
	fm_i2c_lcd_buffer[4] = '1';
	fm_i2c_lcd_buffer[5] = '2';
	fm_i2c_lcd_buffer[6] = '3';
*/
//	memset(fm_i2c_lcd_buffer, 0, fm_i2c_lcd_buffer_count);

	// change port mode to OUTPUT
#ifdef FM_LCD_8
	DDRG |= (1 << 0);
	DDRG |= (1 << 2);
	DDRC |= (1 << 0);
	DDRC |= (1 << 2);
	DDRC |= (1 << 4);
	DDRC |= (1 << 6);
	DDRA |= (1 << 7);
	DDRA |= (1 << 5);
	DDRL |= (1 << 2);
	DDRL |= (1 << 6);
#endif

#ifdef FM_LCD_4

	DDRA |= (1 << 1);
	DDRA |= (1 << 3);
	DDRA |= (1 << 5);
	DDRA |= (1 << 7);
	DDRC |= (1 << 4);
	DDRC |= (1 << 6);

#endif


	fm_i2c_lcd_needs_clean = true;

	Lcd4_Init();
//	Lcd8_Set_Cursor(0,5);
//    Lcd8_Write_String("Loading...");

	fm_i2c_timer5_counter = 0;
	fm_i2c_timer_stage = 0;

	// Setup Timer5
	cli();

	TCCR5A = 0;
	TCCR5B = 0;
	TCNT5 = 0;

	// Runs every 50us
	// 20000 Hz (16000000/((99+1)*8))
	OCR5A = 99;
	// CTC
	TCCR5B |= (1 << WGM52);
	// Prescaler 8
	TCCR5B |= (1 << CS51);
	// Output Compare Match A Interrupt Enable
	TIMSK5 |= (1 << OCIE5A);

  	sei();
}

ISR(TIMER5_COMPA_vect)          // timer compare interrupt service routine
{
	fm_i2c_timer5_counter++;
	if(fm_i2c_timer5_counter>=10000)
	{
//	serial_write('.');
		fm_i2c_timer5_counter = 0;
		fm_i2c_updateSystemData();
		fm_i2c_updateLcd();
		fm_i2c_timer_stage = 0;
//		return;
	}

#ifdef FM_LCD_8
	switch (fm_i2c_timer_stage)
	{
		case 0:
			pinChange(FM_LCD_RS, 0);										// => RS = 0
			Lcd4_Port(0x80);												//Data transfer
			pinChange(FM_LCD_EN, 1);										// => E = 1			
			fm_i2c_timer_stage = 1;
			break;
		case 1:
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 2;
			break;
		case 2:
			pinChange(FM_LCD_RS, 1);										// => RS = 1
			Lcd4_Port(fm_i2c_lcd_buffer[fm_i2c_lcd_buffer_index]);			//Data transfer
			pinChange(FM_LCD_EN, 1);										// => E = 1
			fm_i2c_lcd_buffer_index++;
			if(fm_i2c_lcd_buffer_index>=fm_i2c_lcd_buffer_count)
			{
				fm_i2c_lcd_buffer_index=0;
				fm_i2c_timer_stage = 4;
			}
			else
			{
				fm_i2c_timer_stage = 3;
			}
			break;
		case 3:
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 2;
			break;
		case 4:
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 0;
			break;
	}
#endif

#ifdef FM_LCD_48

	switch (fm_i2c_timer_stage)
	{
		case 0:
			pinChange(FM_LCD_RS, 0);
			Lcd4_Port((0x80) >> 4);
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 1;
			break;
		case 1:
			pinChange(FM_LCD_EN, 1);
			fm_i2c_timer_stage = 2;
			break;
		case 2:
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 3;
			break;
		case 3:
			Lcd4_Port((0x80) & 0x0F);
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 4;
			break;
		case 4:
			pinChange(FM_LCD_EN, 1);
			fm_i2c_timer_stage = 5;
			break;
		case 5:
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 10;
			break;

		case 10:
			pinChange(FM_LCD_RS, 1);
			Lcd4_Port((fm_i2c_lcd_buffer[fm_i2c_lcd_buffer_index] & 0xF0) >> 4);
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 11;
			break;
		case 11:
			pinChange(FM_LCD_EN, 1);
			fm_i2c_timer_stage = 12;
			break;
		case 12:
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 13;
			break;


		case 13:
			Lcd4_Port((fm_i2c_lcd_buffer[fm_i2c_lcd_buffer_index] & 0x0F));
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 14;
			break;
		case 14:
			pinChange(FM_LCD_EN, 1);
			fm_i2c_timer_stage = 15;
			break;
		case 15:
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 20;
			break;

		case 20:
			fm_i2c_lcd_buffer_index++;
			if(fm_i2c_lcd_buffer_index>=fm_i2c_lcd_buffer_count)
			{
				fm_i2c_lcd_buffer_index=0;
				fm_i2c_timer_stage = 0;
			}
			else
			{
				fm_i2c_timer_stage = 10;
			}
			break;
	}
#endif

#ifdef FM_LCD_4

	switch (fm_i2c_timer_stage)
	{
		case 0:
			pinChange(FM_LCD_RS, 0);
			Lcd4_Port((0x80) >> 4);
			pinChange(FM_LCD_EN, 1);
			fm_i2c_timer_stage = 1;
			break;
		case 1:
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 2;
			break;

		case 2:
			Lcd4_Port((0x80) & 0x0F);
			pinChange(FM_LCD_EN, 1);
			fm_i2c_timer_stage = 3;
			break;
		case 3:
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 10;
			break;

		case 10:
			pinChange(FM_LCD_RS, 1);
			Lcd4_Port((fm_i2c_lcd_buffer[fm_i2c_lcd_buffer_index] & 0xF0) >> 4);
			pinChange(FM_LCD_EN, 1);
			fm_i2c_timer_stage = 11;
			break;
		case 11:
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 12;
			break;


		case 12:
			Lcd4_Port((fm_i2c_lcd_buffer[fm_i2c_lcd_buffer_index] & 0x0F));
			pinChange(FM_LCD_EN, 1);
			fm_i2c_timer_stage = 13;
			break;
		case 13:
			pinChange(FM_LCD_EN, 0);
			fm_i2c_timer_stage = 20;
			break;

		case 20:
			fm_i2c_lcd_buffer_index++;
			if(fm_i2c_lcd_buffer_index>=fm_i2c_lcd_buffer_count)
			{
				fm_i2c_lcd_buffer_index=0;
				fm_i2c_timer_stage = 100;
			}
			else
			{
				fm_i2c_timer_stage = 10;
			}
			break;
	}
#endif


}

void fm_i2c_updateSystemData()
{
	FM_I2C_DATA *cdt = &fm_i2c_global_data;
	cdt->structSize = sizeof(FM_I2C_DATA);

	uint8_t idx;
	int32_t current_position[N_AXIS]; // Copy current state of the system position variable
	memcpy(current_position,sys_position,sizeof(sys_position));
	float print_position[N_AXIS];
	system_convert_array_steps_to_mpos(print_position,current_position);

	cdt->state = 0;
	switch (sys.state) 
	{
		case STATE_IDLE: cdt->state = 1; break;
		case STATE_CYCLE: cdt->state = 2; break;
		case STATE_HOLD:
		if (!(sys.suspend & SUSPEND_JOG_CANCEL))
		{
			if (sys.suspend & SUSPEND_HOLD_COMPLETE) 
			{ 
				cdt->state = 3;
			} // Ready to resume
			else 
			{ 
				cdt->state = 4;
			} // Actively holding
			break;
		} // Continues to print jog state during jog cancel.
		case STATE_JOG: cdt->state = 5; break;
		case STATE_HOMING: cdt->state = 6; break;
		case STATE_ALARM: cdt->state = 7; break;
		case STATE_CHECK_MODE: cdt->state = 8; break;
		case STATE_SAFETY_DOOR:
			if (sys.suspend & SUSPEND_INITIATE_RESTORE) 
			{
				cdt->state = 12;		// Restoring
			} 
			else 
			{
				if (sys.suspend & SUSPEND_RETRACT_COMPLETE) 
				{
					if (sys.suspend & SUSPEND_SAFETY_DOOR_AJAR) 
					{
						cdt->state = 10;	// Door ajar
					} 
					else 
					{
						cdt->state = 9;	// Door closed and ready to resume
					} 
				} 
				else 
				{
					cdt->state = 11; // Retracting
				}
			}
		break;
		case STATE_SLEEP: cdt->state = 13; break;
	}

	float wco[N_AXIS];
	if (bit_isfalse(settings.status_report_mask,BITFLAG_RT_STATUS_POSITION_TYPE) || (sys.report_wco_counter == 0) ) 
	{
		for (idx=0; idx< N_AXIS; idx++) 
		{
			// Apply work coordinate offsets and tool length offset to current position.
			wco[idx] = gc_state.coord_system[idx]+gc_state.coord_offset[idx];
			if (idx == TOOL_LENGTH_OFFSET_AXIS) { wco[idx] += gc_state.tool_length_offset; }
			if (bit_isfalse(settings.status_report_mask,BITFLAG_RT_STATUS_POSITION_TYPE)) 
			{
				print_position[idx] -= wco[idx];
			}
		}
	}

	// Report machine position
	if (bit_istrue(settings.status_report_mask,BITFLAG_RT_STATUS_POSITION_TYPE)) 
	{
		//MPos
		cdt->posType = 'M';
	} else {
		//WPos
		cdt->posType = 'W';
	}

	cdt->axisCount = N_AXIS;
	for (uint8_t idx=0; idx<N_AXIS; idx++) 
	{
		if (bit_istrue(settings.flags,BITFLAG_REPORT_INCHES)) 
		{
			cdt->pos[idx] = print_position[idx]*INCH_PER_MM;
		} else {
			cdt->pos[idx] = print_position[idx];
		}
	}

	cdt->feedSpeed = sys.spindle_speed;
}


void fm_i2c_receiveEvent()
{
	/*
	FM_I2C_DATA cdt = fm_i2c_getSystemData();
	Wire.write((const uint8_t*)&cdt, cdt.structSize);
	*/
}

/*
void fm_i2c_printFloat(char *out, float n, uint8_t decimal_places)
{
	uint8_t outx = 0;
	if (n < 0) 
	{
		//serial_write('-');
		out[outx++] = '-';
		n = -n;
	}

	uint8_t decimals = decimal_places;
	while (decimals >= 2) { // Quickly convert values expected to be E0 to E-4.
		n *= 100;
		decimals -= 2;
	}
	if (decimals) { n *= 10; }
	n += 0.5; // Add rounding factor. Ensures carryover through entire value.

	// Generate digits backwards and store in string.
	unsigned char buf[13];
	uint8_t i = 0;
	uint32_t a = (long)n;
	while(a > 0) {
		buf[i++] = (a % 10) + '0'; // Get digit
		a /= 10;
	}
	while (i < decimal_places) {
		buf[i++] = '0'; // Fill in zeros to decimal point for (n < 1)
	}
	if (i == decimal_places) { // Fill in leading zero, if needed.
		buf[i++] = '0';
	}

	// Print the generated string.
	for (; i > 0; i--) 
	{
		if (i == decimal_places) { out[outx++] = '.'; } // Insert decimal point in right place.
		out[outx++] = buf[i-1]; 
		//serial_write(buf[i-1]);
	}
	out[outx++] = 0;
}
*/

void fm_i2c_printFloat(char *out, float n, uint8_t decimal_places, int outSize)
{
	uint8_t outx = 0;
	bool isNegative = false;
	if (n < 0) 
	{
		//serial_write('-');
		//out[outx++] = '-';
		n = -n;
		isNegative = true;
	}
	memset(out, ' ', outSize);

	long n2 =  n*100;
	char buf[13];
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

void fm_i2c_printLong(char *out, long n, int outSize)
{
	uint8_t outx = 0;
	bool isNegative = false;
	if (n < 0) 
	{
		//serial_write('-');
		//out[outx++] = '-';
		n = -n;
		isNegative = true;
	}
	memset(out, ' ', outSize);

	char buf[13];
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

void fm_i2c_printState(char *out, uint16_t state, int outSize)
{
	const char *buf;
	switch (state)
	{
		case 0:
			break;
		default:
			buf = PSTR("UNKNOWN");
			break;
	}
}

void fm_i2c_lcd_printLeft(const char *str, uint8_t col, uint8_t row)
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
	memcpy(fm_i2c_lcd_buffer+offset, str, min(strlen(str), FM_LCD_WIDTH-col));

}

uint16_t fm_test_counter = 0;
void fm_i2c_updateLcd()
{
	char buf[20];
	/*
	FM_I2C_DATA cdt = fm_i2c_getSystemData();
	char buf[32];

	if(fm_i2c_lcd_needs_clean)
	{
		fm_i2c_lcd_needs_clean = false;
		Lcd8_Clear();
	}
	*/
/*
	fm_i2c_lcd_printLeft("A",0,0);
	fm_i2c_lcd_printLeft("B",0,1);
	fm_i2c_lcd_printLeft("C",0,2);
	fm_i2c_lcd_printLeft("D",0,3);

	fm_i2c_lcd_printLeft("E",19,0);
	fm_i2c_lcd_printLeft("F",19,1);
	fm_i2c_lcd_printLeft("G",19,2);
	fm_i2c_lcd_printLeft("H",19,3);

*/
	buf[0] = '0'+fm_test_counter;
	buf[1] = 0;
	fm_test_counter++;
	if(fm_test_counter>=10)
		fm_test_counter = 0;

	fm_i2c_lcd_printLeft(buf,19,3);



	buf[0] = 'X';
	buf[1] = ':';
	fm_i2c_printFloat(buf+2, fm_i2c_global_data.pos[0], 2, 7);
	fm_i2c_lcd_printLeft(buf,0,0);

	buf[0] = 'Y';
	buf[1] = ':';
	fm_i2c_printFloat(buf+2, fm_i2c_global_data.pos[1], 2, 7);
	fm_i2c_lcd_printLeft(buf,0,1);

	buf[0] = 'Z';
	buf[1] = ':';
	fm_i2c_printFloat(buf+2, fm_i2c_global_data.pos[2], 2, 7);
	fm_i2c_lcd_printLeft(buf,11,0);

	buf[0] = 'F';
	buf[1] = ':';
	fm_i2c_printLong(buf+2, fm_i2c_global_data.feedSpeed, 7);
	fm_i2c_lcd_printLeft(buf,11,1);

	buf[0] = 'S';
	buf[1] = 'T';
	buf[2] = ':';
	fm_i2c_printState(buf+3, fm_i2c_global_data.state, 17);


/*
	Lcd8_Set_Cursor(0,0);
    Lcd8_Write_String("X: ");
	fm_i2c_printFloat(buf, cdt.pos[0], 2);
	Lcd8_Set_Cursor(4,0);
    Lcd8_Write_String(buf);

	Lcd8_Set_Cursor(0,1);
    Lcd8_Write_String("Y: ");
	fm_i2c_printFloat(buf, cdt.pos[1], 2);
	Lcd8_Set_Cursor(4,1);
    Lcd8_Write_String(buf);
*/
	/*
	fm_lcd.setCursor(0,0);
	fm_lcd.print(F("X:"));
	fm_lcd.setCursor(0,1);
	fm_lcd.print(F("Y:"));
	fm_lcd.setCursor(0,2);
	fm_lcd.print(F("Z:"));
*/

}
