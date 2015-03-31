#include "unican.h"
//TODO: Check if unican online or not
//TODO: Add define for CRC

static unican_status state;
static unican_buffer unican_rx_buffers [UNICAN_RX_BUFFERS_COUNT];


/*
X-Modem CRC16 x^16+x^12+x^5+1
*/
static const uint16 crc16tab[] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0, 
};

uint16 crc16(uint8 *buf, uint16 len)
{
  uint16 i;
  uint16 retval;
  retval = 0;  
  for (i = 0;  i < len;  i++)
    retval = (retval<<8) ^ crc16tab[((retval>>8) ^ *buf++)&0x00FF];
  return retval;
}


/*
Frees memory allocated for message;
*/
void unican_drop_message(unican_message* msg)
{
  if (msg != NULL)
  {
    state.free_space += msg->unican_length+sizeof(unican_message);
    free(msg->data);
    free(msg);
  }
}

/*

*/
static void unican_flush_queue(void)
{
unican_message* current;
  while (state.first != NULL)
  {
    current = state.first;
    state.first = state.first->next_msg;
    unican_drop_message(current);
  }
  state.messages_count = 0;
  state.first = NULL;
  state.last = NULL;           
}

/*
Initialization of buffers itself
*/
void unican_init (void)
{
  uint16 i;  
  for (i=0; i < UNICAN_RX_BUFFERS_COUNT; i++)
  {
    unican_rx_buffers[i].umsg = NULL;
    unican_rx_buffers[i].position = 0;
    unican_rx_buffers[i].crc = 0;
  }
  state.is_online = 1;
  unican_flush_queue();
  state.free_space = UNICAN_ALLOWED_SPACE;  
  state.free_buffers_count = UNICAN_RX_BUFFERS_COUNT;
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
      if (free_buffer == NULL)
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
  unican_RX_event (umsg->unican_msg_id, umsg->unican_length);
}

/*
Collects unican message, sends it to user and clears 
used buffer
*/
static uint16 unican_collect_buffer(unican_buffer* buff)
{
  uint16 retval = UNICAN_OK;
  uint16 crc;
  crc = crc16(buff->umsg->data, buff->umsg->unican_length);
  if (buff->crc == crc)
    unican_save_message(buff->umsg);
  else
    retval = UNICAN_WRONG_CRC;
  buff->umsg = NULL;
  buff->crc = 0x0000;
  buff->position = 0;
  state.free_buffers_count++;
  return retval;
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


