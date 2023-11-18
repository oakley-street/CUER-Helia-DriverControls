#ifndef CUER_CAN_H
#define CUER_CAN_H

#include "mbed.h"
#define CAN_TIMEOUT_MS 100

extern CAN can;
extern bool CAN_data_sent;

bool can_send(CANMessage msg);
void CANDataSentCallback(void);
#endif
