#include "unican.h"
#include <stdlib.h>
//TODO check all malloc results

static unican_status state;
static unican_buffer unican_rx_buffers [UNICAN_RX_BUFFERS_COUNT];

/*
Initialization of buffers itself
*/
//TODO: Implementation needed
uint16 unican_init (void)
{
  uint16 i;  
  for (i=0; i < UNICAN_RX_BUFFERS_COUNT; i++)
    unican_rx_buffers[i].umsg = NULL;
}

/*
Allocate memory for new unican_message
*/
inline static unican_message* unican_allocate_message(uint16 msg_id, uint16 from, uint16 to, uint16 data_len)
{
  if (state.free_space > data_len+sizeof(unican_message))
    return NULL;
  else
  {
  unican_message* temp_msg;
  temp_msg = malloc (sizeof(unican_message));
  if (temp_msg == NULL)
    return NULL;
  
  temp_msg->unican_msg_id = msg_id;
  temp_msg->unican_address_from = from;
  temp_msg->unican_address_to = to;
  temp_msg->unican_length = data_len;
  temp_msg->data = malloc (data_len);
  if (temp_msg->data == NULL)
  {
    free(temp_msg);
    return NULL; 
  } 
  temp_msg->next_msg = NULL;
  state.free_space -= (data_len+sizeof(unican_message));
  return temp_msg;
  }
}

        

/*
Find buffer associated with data flow from known address.
*/
static unican_buffer* unican_find_buffer (uint16 address_from)
{
  uint16 i;
  unican_buffer* free_buffer;
  free_buffer = NULL;
  
  for (i=0; i < UNICAN_RX_BUFFERS_COUNT; i++)
    if (unican_rx_buffers[i].umsg != NULL)
    {
      if (unican_rx_buffers[i].umsg->unican_address_from == address_from)
        return &(unican_rx_buffers[i]);
    }
    else
      free_buffer = &(unican_rx_buffers[i]);
      
  return free_buffer;
}

/*
Check if identifier valid or not
*/
//TODO: additional checks needed
static uint16 can_check_message (can_message* msg)
{
  if (msg->can_dlc > CAN_MAX_DLC)
    return UNICAN_CAN_MESSAGE_TOO_LONG;
  else if ((msg->can_identifier > 0x4FF) && (msg->can_extbit == 0))
    return UNICAN_CAN_IDENTIFIER_TOO_LONG ;
  else if ((msg->can_identifier > 0x1FFFFFFF) && (msg->can_extbit > 0))
    return UNICAN_CAN_IDENTIFIER_EXT_TOO_LONG ;
  else
    return UNICAN_OK;
}


/*
Saves valid unican messages for further use
*/
static void unican_save_message (unican_message* umsg)
{
//TODO:IMPLEMENT QUEUE!
  if (state.last == NULL)
      state.last = umsg;
    else
      state.last->next_msg = umsg;
  state.messages_count++;
  
}
/*

*/
static uint16 unican_check_crc(uint8* buff, uint16 length)
{
  //TODO:Implement
}

/*
Collects unican message, sends it to user and clears 
used buffer
*/
//TODO: Implementation needed
static uint16 unican_collect_buffer(unican_buffer* buff)
{
  
  uint16 crc;
  crc = unican_check_crc(buff->umsg->data, buff->umsg->unican_length);
  //TODO:Check actually
  unican_save_message(buff->umsg);
  buff->umsg = NULL;
  buff->crc = 0x0000;
  buff->position = 0;
  state.free_buffers_count++;
  return UNICAN_OK;
}

/*
Processes single received CAN message 
*/

uint16 can_receive_message (can_message* msg)
{
  unican_buffer* buff;
  //Reading CAN identifier feilds
  uint16 address_to = 0;
  uint16 address_from = 0;
  uint8 data_bit = 0;
  uint16 i;
  uint16 retval;
  retval = UNICAN_OK;
  
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
  if (errcode != UNICAN_OK)
    return errcode;
  //Done with checking
  
  //Receive Logic:
  
  //Appending pure data to buffer
  if (data_bit == 1)  
  {
    buff = unican_find_buffer(address_from);
    if (buff == NULL)
      return UNICAN_DATA_WITHOUT_START;
    else if (buff->umsg == NULL)
      return UNICAN_DATA_WITHOUT_START;
    else
    {
      for (i=0 ; i < msg->can_dlc ; i++)
      {
        if (buff->position < buff->umsg->unican_length)
        {
          buff->umsg->data[buff->position] = msg->data[i];
          buff->position++;
        }
        else
        {
          unican_collect_buffer(buff);
          return UNICAN_WARNING_UNEXPECTED_DATA;//and what to do with buffer?
        }
      }
      if (buff->umsg->unican_length == buff->position)
        unican_collect_buffer(buff);
    }
  }
  //Done with appending pure data to buffer
  //Processing some short commands
  else
  {
    uint16 msg_id;
    if (msg->can_dlc >= CAN_MIN_DLC)
      msg_id = msg->data[0] + (msg->data[1] * 256); 
    else
      return UNICAN_CAN_MESSAGE_TOO_SHORT;
    
    switch (msg_id)
    {
      //Some protocol-specific short messages
      case UNICAN_START_LONG_MESSAGE:
      {
        if (msg->can_dlc < UNICAN_HEADER_SIZE)
          return UNICAN_HEADER_TOO_SHORT;
          
        buff = unican_find_buffer(address_from);
        if (buff == NULL)
          return UNICAN_NO_FREE_BUFFER;
        else 
        {
          if (buff->umsg != NULL) 
          {         
            unican_drop_message(buff->umsg);
            retval = UNICAN_WARNING_BUFFER_OVERWRITE;
          }
          uint16 msg_subid = msg->data[2] + (msg->data[3] * 256);
          uint16 data_length = msg->data[6] + (msg->data[7] * 256);          
          buff->umsg = unican_allocate_message(msg_subid, address_from, address_to, data_length);
          if (buff->umsg == NULL)          
            return UNICAN_CANT_ALLOCATE_MESSAGE;//FIXME:Here we have a chance to ignore retval
          state.free_buffers_count--;
          buff->crc = msg->data[4] + (msg->data[5] * 256);
          buff->position = 0;
        }
        
      } break;
      //Done with protocol-specific short messages
      //Regular short messages
      default:
      {        
        unican_message* temp_msg;
        temp_msg = unican_allocate_message(msg_id, address_from, address_to, msg->can_dlc - CAN_MIN_DLC);
        if (temp_msg == NULL)
          return UNICAN_CANT_ALLOCATE_MESSAGE;
        
        for ( i = 0; i<temp_msg->unican_length; i++)
          temp_msg->data[i]=msg->data[i+CAN_MIN_DLC];
          
        unican_save_message (temp_msg);          
          
      } break;
      //Done with regular short messages
    }
  }
  //Done with short commands
  return retval;
}

inline void can_send_message (can_message* msg)
{
  can_HW_send_message (msg);
}

void unican_drop_message(unican_message* msg)
{
  if (msg != NULL)
  {
    state.free_space += msg->unican_length+sizeof(unican_message);
    free(msg->data);
    free(msg);
  }
}
