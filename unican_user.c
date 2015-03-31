#include "unican.h"
//TODO:Check all this terrible english comments
/*
This function could be implemented in any way, but as
result user programm should call receive_can_message()
with valid can_message structures provided.
Function name and parameters are not mandatory, but 
shold be changed in unican_user.h accordingly
*/
void can_HW_receive_message (void)
{
  can_message msg;
  /* HW depended user code */
  
  /*end of user code*/
  can_receive_message (&msg);
}

/*
This function could be implemented in any way, but as
result user programm should send provided can_message
to network.
Function name and parameters are mandatory
*/
void can_HW_send_message (can_message* msg)
{
  /* HW depended user code */
  
  /*end of user code*/
}

/*
This event occures when valid unican message received
it's calls from within can_receive_message () so if
can_receive_message () called in interrupt handler
you will not want to do a lot of work here
*/
void unican_RX_event (uint16 msg_id, uint16 length)
{
  /* User code */
  
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
  
  /*end of user code*/
}

/*
This is example of user code

void main (void)
{
  unican_init();
  uint8 tempBuffer[11] = {'H','e','l','l','o',' ','W','o','r','l','d'};  
  unican_message tx_msg;
  
  tx_msg.unican_msg_id = 0xAA55;//some MSG_ID which is expected by receiver
  tx_msg.unican_address_from = 0x01;//our unican address eample
  tx_msg.unican_address_to = 0x02;//unican address of receiver example
  tx_msg.unican_length = sizeof(tempBuffer);
  tx_msg.data = tempBuffer;
  tx_msg.next_msg = NULL;  
}
*/
