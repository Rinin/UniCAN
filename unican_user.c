#include "unican.h"
#include "../main.h" //TODO:Kill
void pass_RX_event(void);
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
  pass_RX_event();
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
  pass_RX_message(msg);
  //int i;
  //for (i = 0; i < msg->unican_length; i++)
  //  printf("%c", (msg->data)[i]);
  //printf ("\n");
  //ttt();
  
  /*end of user code*/
}

/*
This function should perform required software
reaction to errors in unican
*/
void unican_error (uint16 errcode)
{
  
  /* User code */
  pass_unican_error(errcode);
  /*end of user code*/
}

/*
This is example of user code

void main (void)
{
  
}
*/
