#ifndef fm_system_data_h
#define fm_system_data_h

#include "grbl.h"

typedef struct
{
	uint8_t structSize;
	uint8_t axisCount;
	uint8_t state;
	uint8_t posType;

	float feedSpeed;
	float pos[N_AXIS];

}FM_SYSTEM_DATA;


class FMSystemData
{
	private:

	public:
		FM_SYSTEM_DATA mSystemData;


		FMSystemData();
		void update();

};

#endif
