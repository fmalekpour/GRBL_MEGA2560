
#include <Wire.h>

/*
#include <Arduino.h>

char fm_i2c_buffer[1024];
int fm_i2c_buffer_index;
*/
void fm_i2c_init()
{
	
	//fm_i2c_buffer_index = 0;
	Wire.begin(4);
//	Wire.onReceive(fm_i2c_receiveEvent);
	Wire.onRequest(fm_i2c_receiveEvent);
	
}

void fm_i2c_receiveEvent()
{
	Wire.write("hello ");



	/*
	while(Wire.available()>0)
	{
		fm_i2c_buffer[fm_i2c_buffer_index] = Wire.read();
		if(fm_i2c_buffer[fm_i2c_buffer_index]==0x0D || fm_i2c_buffer[fm_i2c_buffer_index]==0x0A)
		{
			fm_i2c_buffer[fm_i2c_buffer_index] = 0;
			fm_i2c_buffer_index = 0;
			system_execute_line(fm_i2c_buffer);
			break;
		}
		else
		{
			fm_i2c_buffer_index++;
		}
	}
*/
}