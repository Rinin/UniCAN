#pragma once


#include <stdlib.h> //malloc

void unican_RX_event (uint16_t msg_id, uint16_t length);
void unican_error (uint16_t errcode);

void send_reg_tel(float angular_rate, float period);

