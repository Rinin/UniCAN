#include "unican.h"
#include "state_machine.h"
//TODO:Check all this terrible english comments

/*
This event occures when valid unican message received
it's calls from within can_receive_message () so if
can_receive_message () called in interrupt handler
you will not want to do a lot of work here
*/
void unican_RX_event (uint16 msg_id, uint16 length)
{
  /* User code */
	unican_take_message();
	//diag_printf("Received unican message Id = %d; Length = %d\n",msg_id,length);
  /*end of user code*/
}

/*
This function should perform required software
reaction to unican message received from another
device.
*/
void unican_RX_message (unican_message* msg)
{
  /* User code */

	switch (msg->unican_msg_id)
	{
		case AVS_TELEMETRY_X:
		{

			float val;
			cyg_int32 val2;
			cyg_int8 temp;
			temp = msg->data[5];
			val = *((float*)(&(msg->data[0])));
			val2 = val * 1000;
			diag_printf("X val = %d;\t X temp = %d\n",val2, temp);

		} break;
		case AVS_TELEMETRY_Y:
		{

		} break;
		case AVS_TELEMETRY_Z:
		{

		} break;
		case MACHINE_SWITCH_TO_STATE_1:
		{
			SM_machine_request_state(SM_STATE_IDLE);
		} break;
		case MACHINE_SWITCH_TO_STATE_2:
		{
			SM_machine_request_state(SM_STATE_1);
		} break;
		default:
		{

		} break;
	}

  /*end of user code*/
}

/*
This function should perform required software
reaction to errors in unican
*/
void unican_error (uint16 errcode)
{
  
  /* User code */

  /*end of user code*/
}

