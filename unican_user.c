#include "unican.h"
#include "state_machine.h"
#include "telemetry.h"
#include "obc_setup.h"
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
		case UNICAN_AVS_TELEMETRY_X:
		{
			magnetic_data data;
			data.temp = msg->data[5];
			data.val = *((float*)(&(msg->data[0])));
			add_magnetic_data (&data, 0);
		} break;
		case UNICAN_AVS_TELEMETRY_Y:
		{
			magnetic_data data;
			data.temp = msg->data[5];
			data.val = *((float*)(&(msg->data[0])));
			add_magnetic_data (&data, 1);
		} break;
		case UNICAN_AVS_TELEMETRY_Z:
		{
			magnetic_data data;
			data.temp = msg->data[5];
			data.val = *((float*)(&(msg->data[0])));
			add_magnetic_data (&data, 2);
		} break;
		case UNICAN_MACHINE_SWITCH_TO_STATE_IDLE:
		{
			diag_printf("Switch to state IDLE\n");
			SM_machine_request_state(SM_STATE_IDLE);
		} break;
		case UNICAN_MACHINE_SWITCH_TO_STATE_1:
		{
			diag_printf("Switch to state 1\n");
			SM_machine_request_state(SM_STATE_1);
		} break;
		case UNICAN_MACHINE_SWITCH_TO_STATE_2:
		{
			diag_printf("Switch to state 2\n");
			SM_machine_request_state(SM_STATE_2);
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

