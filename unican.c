#include "unican.h"

static unican_buffer unican_rx_buffers [UNICAN_RX_BUFFERS_COUNT];
/*
Initialization of buffers itself
*/
//TODO: Implementation needed
uint16 unican_init (void)
{
  uint16 i;
  
  for (i=0; i < UNICAN_RX_BUFFERS_COUNT; i++)
    unican_rx_buffers[i].umsg.unican_address_from = 0;
    
}
/*
Find buffer associated with data flow from known address.
*/
static unican_buffer* unican_find_buffer (uint16 address_from)
{
  uint16 i;
  for (i=0; i < UNICAN_RX_BUFFERS_COUNT; i++)
    if (unican_rx_buffers[i].umsg.unican_address_from == address_from)
      return &(unican_rx_buffers[i]);
      
  return NULL;
}

/*
Check if identifier valid or not
*/
//TODO: additional checks needed
static uint16 can_check_message (can_message* msg)
{
  if (msg->can_dlc > CAN_MAX_DLC)
    return CAN_MESSAGE_TOO_LONG;
  else if ((msg->can_identifier > 0x4FF) && (msg->can_extbit == 0))
    return CAN_IDENTIFIER_TOO_LONG ;
  else if ((msg->can_identifier > 0x1FFFFFFF) && (msg->can_extbit > 0))
    return CAN_IDENTIFIER_EXT_TOO_LONG ;
  else
    return 0;
}

/*
Collects unican message, sends it to user and clears 
used buffer
*/
//TODO: Implementation needed
static uint16 unican_collect_buffer(unican_buffer* buff)
{
  
}

/*
Processes single received CAN message 
*/
//TODO:Replace return with some other errcode handler. We shouldn't return to user
//TODO:Add buffer beteen irq and receive;

uint16 can_receive_message (can_message* msg)
{
  unican_buffer* buff;
  //Reading CAN identifier feilds
  uint16 address_to = 0;
  uint16 address_from = 0;
  uint8 data_bit = 0;
  uint16 i;
  
  if (msg->can_extbit == 0) 
  {
    address_to = (msg->can_identifier & 0x1F);
    address_from = (msg->can_identifier & 0x3E0)>>5;
    data_bit = (msg->can_identifier & 0x400)>>10;
  }
  else
  {
    address_to = (msg->can_identifier & 0x3FFF);
    address_from = (msg->can_identifier & 0xFFFC000)>>14;
    data_bit = (msg->can_identifier & 0x10000000)>>28;
  }
  //Done with CAN identifier feilds
  //Checking message
  uint16 errcode;
  errcode = can_check_message(msg);
  if (errcode != 0)
    return errcode;
  //Done with checking
  
  //Receive Logic:
  
  //Appending pure data to buffer
  if (data_bit == 1)  
  {
    buff = unican_find_buffer(address_from);
    if (buff == NULL)
      return UNICAN_DATA_WITHOUT_START;
    else
    {
      for (i=0 ; i < msg->can_dlc ; i++)
      {
        if (buff->position < buff->umsg.unican_length)
        {
          buff->umsg.data[buff->position] = msg->data[i];
          buff->position++;
        }
        else
          return UNICAN_UNEXPECTED_DATA;
      }
      if (buff->umsg.unican_length == buff->position)
        unican_collect_buffer(buff);
    }
  }
  //Done with appending pure data to buffer
  //Processing some short commands
  else
  {
    uint16 msg_id;
    if (msg->can_dlc >= CAN_MIN_DLC)
      msg_id = ((msg->data[0]&0x00FF) | (msg->data[1]<<8)); 
    else
      return CAN_MESSAGE_TOO_SHORT;
    
    switch (msg_id)
    {
      //Some protocol-specific short messages
      case UNICAN_START_LONG_MESSAGE:
      {
        //TODO: Implementation needed
      } break;
      //Done with protocol-specific short messages
      //Regular short messages
      default:
      {
        //TODO: Implementation needed
      } break;
      //Done with regular short messages
    }    
  }
  //Done with short commands
}

inline void can_send_message (can_message* msg)
{
  can_HW_send_message (msg);
}

//TODO: Implementation needed
void unican_drop_msg(unican_message* msg)
{

}
