
#include "fm_system_data.h"

FMSystemData::FMSystemData()
{

}

void FMSystemData::update()
{
	FM_SYSTEM_DATA *cdt = &this->mSystemData;
	cdt->structSize = sizeof(FM_SYSTEM_DATA);

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

	cdt->feedSpeed = st_get_realtime_rate();
	
}