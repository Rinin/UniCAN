#include "unican.h"
/*
This function could be implemented in any way, but as
result user programm should call receive_can_message()
with valid can_message structures provided.
Function name and parameters are not mandatory, but 
shold be changed in unican.h accordingly
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
This function should perform required software
reaction to unican message received from another
device  
*/
void unican_proceed_message (unican_message* msg)
{
  /* User code */
  
  /*end of user code*/
  unican_drop_msg(msg);
}
