#include "unican.h"

void can_HW_init (void)
{

  /* HW depended user code */

  /*end of user code*/
  
}

void can_HW_close(void)
{

  /* HW depended user code */

  /*end of user code*/
  
}

/*
This function could be implemented in any way, but as
result user programm should call receive_can_message()
with valid can_message structures provided.
Function name and parameters are not mandatory, but
shold be changed in unican_user.h accordingly
*/
void can_HW_receive_message (void)
{
  can_message tmp_msg;
  /* HW depended user code */

  /*end of user code*/
  can_receive_message (&tmp_msg);
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
