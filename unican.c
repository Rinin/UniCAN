#include "unican.h"

static uint16 can_check_dlc (can_message* msg)
{
  if (msg->can_dlc > CAN_MAX_DLC)
    return CAN_MESSAGE_TOO_SHORT;
  else if (msg->can_dlc < CAN_MIN_DLC)
    return CAN_MESSAGE_TOO_LONG;
  else  
    return 0;  
}

static uint16 can_check_identifier (can_message* msg)
{
  if ((msg->can_identifier >= 0x800) && (msg->can_extbit = 0))
    return CAN_IDENTIFIER_TOO_LONG ;
  else if ((msg->can_identifier >= 0x20000000) && (msg->can_extbit > 0))
    return CAN_IDENTIFIER_EXT_TOO_LONG ;
  else
    return 0;
}

uint16 can_receive_message (can_message* msg)
{
  uint16 errcode;
  errcode = can_check_dlc();
  if (errcode != 0)
    return errcode
  errcode = can_check_identifier();
  if (errcode != 0)
    return errcode
  switch (msg->can_extbit)
  case 0:
  {
    
  } break;
  default:
  {
  
  } break;
  
}

void can_send_message (can_message* msg)
{
  can_HW_send_message (msg);
}

void unican_drop_msg(unican_message* msg)
{

}
