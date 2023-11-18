#include "CUER_CAN.h"

bool can_send(CANMessage msg){
	Timer t;
	CAN_data_sent = false;
	t.start();
	can.write(msg);
	while(!CAN_data_sent && t.read_ms() < CAN_TIMEOUT_MS);
	return t.read_ms() <= CAN_TIMEOUT_MS;
}

void CANDataSentCallback(void){
	CAN_data_sent = true;
}