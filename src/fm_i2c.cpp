
#include "grbl.h"
#include <Wire.h>


/**
 * 32
 * 47
 * 45
 * 43
 * 41
 * 39
 * 37
 * 35
 * 33
 * 31
 * 29
 * 27
 * 
 */

#define FM_LCD_PIN_V0	32
#define FM_LCD_PIN_RS	47
#define FM_LCD_PIN_RW	45
#define FM_LCD_PIN_E	43
#define FM_LCD_PIN_D0	41
#define FM_LCD_PIN_D1	39
#define FM_LCD_PIN_D2	37
#define FM_LCD_PIN_D3	35
#define FM_LCD_PIN_D4	33
#define FM_LCD_PIN_D5	31
#define FM_LCD_PIN_D6	29
#define FM_LCD_PIN_D7	27

//#define FM_LCD_PIN_A	27
//#define FM_LCD_PIN_K	27
/*
(uint8_t rs, uint8_t rw, uint8_t enable,
		uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
		uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
*/

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
void fm_i2c_printFloat(char *out, float n, uint8_t decimal_places);
void fm_i2c_updateLcd();

uint32_t fm_i2c_timer5_counter;
bool fm_i2c_lcd_needs_clean;

void fm_i2c_init()
{
	Wire.begin(4);
	Wire.onRequest(fm_i2c_receiveEvent);

	// change port mode to OUTPUT
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

	fm_i2c_lcd_needs_clean = true;

	Lcd8_Init();
	Lcd8_Set_Cursor(0,5);
    Lcd8_Write_String("Loading...");

	fm_i2c_timer5_counter = 0;

	// Setup Timer5
	cli();
/*	
	TCCR5A = 0;
	TCCR5B = 0;

	TCNT5 = 34286;            // preload timer 65536-16MHz/256/2Hz
	
	TCCR5B |= (1 << WGM12);   // CTC mode
//	TCCR5B |= (1 << CS12);    // 256 prescaler 
	TCCR5B |= (1 << CS51);    // 256 prescaler 
	TIMSK5 |= (1 << OCIE1A);  // enable timer compare interrupt
*/
	

	TCCR5B |= (1 << WGM12); // Configure timer 1 for CTC mode
	TIMSK5 |= (1 << OCIE1A); // Enable CTC interrupt
	sei();
	OCR5A   = 15624;
	TCCR5B |= ((1 << CS10) | (1 << CS11)); // Start timer at Fcpu/64



//	sei();
}

ISR(TIMER5_COMPA_vect)          // timer compare interrupt service routine
{
	fm_i2c_timer5_counter++;
	if(fm_i2c_timer5_counter>=10)
	{
	//serial_write('.');
		fm_i2c_timer5_counter = 0;
		fm_i2c_updateLcd();
	}
}

FM_I2C_DATA fm_i2c_getSystemData()
{
	FM_I2C_DATA cdt;
	cdt.structSize = sizeof(cdt);

	uint8_t idx;
	int32_t current_position[N_AXIS]; // Copy current state of the system position variable
	memcpy(current_position,sys_position,sizeof(sys_position));
	float print_position[N_AXIS];
	system_convert_array_steps_to_mpos(print_position,current_position);

	cdt.state = 0;
	switch (sys.state) 
	{
		case STATE_IDLE: cdt.state = 1; break;
		case STATE_CYCLE: cdt.state = 2; break;
		case STATE_HOLD:
		if (!(sys.suspend & SUSPEND_JOG_CANCEL))
		{
			if (sys.suspend & SUSPEND_HOLD_COMPLETE) 
			{ 
				cdt.state = 3;
			} // Ready to resume
			else 
			{ 
				cdt.state = 4;
			} // Actively holding
			break;
		} // Continues to print jog state during jog cancel.
		case STATE_JOG: cdt.state = 5; break;
		case STATE_HOMING: cdt.state = 6; break;
		case STATE_ALARM: cdt.state = 7; break;
		case STATE_CHECK_MODE: cdt.state = 8; break;
		case STATE_SAFETY_DOOR:
			if (sys.suspend & SUSPEND_INITIATE_RESTORE) 
			{
				cdt.state = 12;		// Restoring
			} 
			else 
			{
				if (sys.suspend & SUSPEND_RETRACT_COMPLETE) 
				{
					if (sys.suspend & SUSPEND_SAFETY_DOOR_AJAR) 
					{
						cdt.state = 10;	// Door ajar
					} 
					else 
					{
						cdt.state = 9;	// Door closed and ready to resume
					} 
				} 
				else 
				{
					cdt.state = 11; // Retracting
				}
			}
		break;
		case STATE_SLEEP: cdt.state = 13; break;
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
		cdt.posType = 'M';
	} else {
		//WPos
		cdt.posType = 'W';
	}

	cdt.axisCount = N_AXIS;
	for (uint8_t idx=0; idx<N_AXIS; idx++) 
	{
		if (bit_istrue(settings.flags,BITFLAG_REPORT_INCHES)) 
		{
			cdt.pos[idx] = print_position[idx]*INCH_PER_MM;
		} else {
			cdt.pos[idx] = print_position[idx];
		}
	}

	cdt.feedSpeed = sys.spindle_speed;

	return cdt;
}


void fm_i2c_receiveEvent()
{
	FM_I2C_DATA cdt = fm_i2c_getSystemData();

	Wire.write((const uint8_t*)&cdt, cdt.structSize);
}


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

void fm_i2c_updateLcd()
{
	FM_I2C_DATA cdt = fm_i2c_getSystemData();
	char buf[32];

	if(fm_i2c_lcd_needs_clean)
	{
		fm_i2c_lcd_needs_clean = false;
		Lcd8_Clear();
	}

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
