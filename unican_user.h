#ifndef UNICAN_USER_H_
#define UNICAN_USER_H_
#include <stdlib.h> //We are using malloc() and free() so we need them (HW/compiler depended)

void unican_RX_event (uint16 msg_id, uint16 length);
void unican_error (uint16 errcode);

#endif /* UNICAN_USER_H_ */
