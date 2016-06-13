#pragma once

#define UINT16LEFT(val) \
  (((val) >> 8) & 0x00FF)
  
#define UINT16RIGHT(val) \
  ((val) & 0x00FF)
  
#define CAN_STANDART_HEADER 0
#define CAN_EXTENDED_HEADER 4

typedef struct tag_can_message
{
  uint32_t can_identifier;                     // 11 or 29bit CAN identifier
  uint8_t can_rtr;                             // Remote transmission request bit
  uint8_t can_extbit;                          // Identifier extension bit. 0x00 indicate 11 bit message ID
  uint8_t can_dlc;                             // Data length code. Number of bytes of data (0–8 bytes)
  volatile uint8_t data[8];                    // Data field
}  __attribute__ ((packed)) can_message;

/*Для инициализации необходимо имя порта, однако unican_HW_init()
не имеет нужного параметра, поэтому понадобилась эта функция.
Вторая (USBTin_unican_close()) - для единообразия интерфейса.*/
#ifdef __cplusplus
#include <QString>
void USBTin_unican_init(const QString &port_name);
void USBTin_unican_close();

/*Получить указатель на экземпляр объекта для соединения с его сигналами и слотами.
Для получения данных при помощи can_HW_receive_msg(), его нужно вызвать в слоте,
соединённом с сигналом can_message_ready(can_message msg) объекта.*/
class USBtinReader;
USBtinReader *USBTin_get_can_port();
#endif

#ifdef __cplusplus
extern "C" {
#endif
void can_send_message (can_message* msg);
void can_HW_init (void);                     //HW implementation
void can_HW_receive_message (void);          //HW implementation, may be changed.
void can_HW_send_message (can_message* msg); //HW implementation
void can_HW_close(void);                     //HW implementation
#ifdef __cplusplus
}
#endif

