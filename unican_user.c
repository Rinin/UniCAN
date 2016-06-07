#include "unican.h"
#include "mtime.h"

/*
This event occures when valid unican message received
it's calls from within can_receive_message () so if
can_receive_message () called in interrupt handler
you will not want to do a lot of work here
 */
void unican_RX_event (uint16_t msg_id, uint16_t length)
{
	/* User code */
	unican_take_message();
	/*end of user code*/
}

void send_reg_tel(float angular_rate, float period)
{
	static system_timer_typedef prev_timer;
	static system_timer_typedef cur_timer;

	static uint8_t prev_timer_initialized = 0;
	if(!prev_timer_initialized){
		prev_timer_initialized = 1;
		system_timer_update(&prev_timer);
	}

	float delta_time;
	system_timer_update(&cur_timer);
	system_timer_get_diff_float(&cur_timer, &prev_timer, &delta_time);
	if(delta_time > period){
		prev_timer = cur_timer;

		unican_message can_msg;
		uint8_t* p;
		uint8_t i, can_data[6];

		can_msg.unican_address_from=UNICAN_ID;
		can_msg.unican_address_to=0x05;
		can_msg.unican_length=6;
		can_msg.unican_msg_id=0x0AD1;

		p=(uint8_t *)&angular_rate;
		for (i = 0; i < 4; ++i) {
			can_data[i]=*(p+i);
		}

		can_data[4] = 0;
		can_data[5] = 0;

		can_msg.data=can_data;
		unican_send_message(&can_msg);
	}
}

/*
This function should perform required software
reaction to unican message received from another
device.
 */
void unican_RX_message (unican_message* msg)
{
	//	Velocity=0;
	//	/* User code */
	//	uint8_t i;
	//
	//	if(msg->unican_msg_id==0x0A01)
	//	{
	//		for (i = 0; i < 4; ++i) {
	//			*(uint8_t *)((uint8_t*)(&Velocity)+i)=*(msg->data+i);
	//		}
	//	}

	printf("message received: 0x%X!\n", msg->unican_address_from);

	/*end of user code*/
}

/*
This function should perform required software
reaction to errors in unican
 */
void unican_error (uint16_t errcode)
{
};
