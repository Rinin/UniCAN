#ifndef UNICAN_ERRCODES_H_
#define UNICAN_ERRCODES_H_

#define UNICAN_NO_FREE_BUFFER 4
#define UNICAN_DATA_WITHOUT_START 5
#define UNICAN_UNEXPECTED_DATA 6

/*
0x0001 - таймаут сообщения
0x0002 - следующая команда начала передачи пришла до получения всех данных посылки.
0x0003 - ошибка CRC.
*/
#define CAN_MESSAGE_TOO_SHORT 11
#define CAN_MESSAGE_TOO_LONG 12 
#define CAN_IDENTIFIER_TOO_LONG 13
#define CAN_IDENTIFIER_EXT_TOO_LONG 14



#endif /* UNICAN_ERRCODES_H_ */
