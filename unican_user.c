#include "unican.h"
#include "stdio.h"

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

void unican_RX_message (unican_message* msg)
{
    printf("message received: 0x%X!\n", msg->unican_address_from);
}

/*
This function should perform required software
reaction to errors in unican
 */
void unican_error (uint16_t errcode)
{
}
