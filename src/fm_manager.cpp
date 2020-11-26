
#include "grbl.h"
#include "fm_lcd.h"
#include "fm_system_data.h"
#include "fm_twi.h"

FMLcd *fm_manager_lcd;
FMSystemData *fm_system_data;
uint32_t fm_manager_timer_counter;



void fm_manager_setupTimer();
void I2C_received(uint8_t received_data);
void I2C_requested();


void fm_manager_init()
{
	fm_system_data = new FMSystemData();
	fm_manager_lcd = new FMLcd(20,4);

	fm_manager_timer_counter = 0;

	fm_manager_setupTimer();

	I2C_setCallbacks(I2C_received, I2C_requested);
	I2C_init(0x04);


}

void fm_manager_setupTimer()
{
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
	fm_manager_timer_counter++;
	if(fm_manager_timer_counter>=10000)
	{
//	serial_write('.');
		fm_manager_timer_counter = 0;
//		fm_i2c_updateSystemData();
		fm_system_data->update();
//		fm_i2c_updateLcd();
		fm_manager_lcd->updateLcd(fm_system_data);
		fm_manager_lcd->mTimerStage = 0;
//		return;
	}

	if(fm_manager_lcd->mTimerStage<100)
	{
		fm_manager_lcd->mTimerStage = fm_manager_lcd->runCycleFromStage();
	}
}

void I2C_received(uint8_t received_data)
{
}

void I2C_requested()
{
	int s = sizeof(FM_SYSTEM_DATA);
	uint8_t buf[sizeof(FM_SYSTEM_DATA)];
	memcpy(buf, (uint8_t*)&(fm_system_data->mSystemData), s);
	for(int i=0;i<s;i++)
	{
	  	I2C_transmitByte(buf[i]);
	}
}
