/*****************************************************************************************************\
 CUER Driver Controls v5.0 Code by Rahul Swaminathan (August 2019)
 
 Currently set up for use in nitrous (torque demand) mode only.
\*****************************************************************************************************/

#include <mbed.h>
#include "CUER_CAN.h"

#define MAX_VELOCITY_MPH 10
#define WHEEL_RADIUS_M 0.275
//Drive message containing ignition and reverse bits
//(this message has a DLC of 1, with bit 0 being the ignition flag and bit 1 being the reverse flag)
#define DRIVE_MESSAGE_ADDR 0x500
//Left Wavesculptor base address
#define DRIVER_CONTROLS_BASE_ADDR_L 0x500
//Right Wavesculptor base address
#define DRIVER_CONTROLS_BASE_ADDR_R 0x550

#define THROTTLE_POT_MIN 0.11
#define THROTTLE_POT_MAX 0.22


DigitalIn solar_state(p6);
DigitalIn regen_en(p7);
DigitalIn reverse_state(p8);
DigitalIn handbrake(p9);
DigitalIn headlights(p11);
DigitalIn front_fog(p12);
DigitalIn rear_fog(p13);
DigitalIn left_ind(p14);
DigitalIn right_ind(p15);
DigitalIn hazards(p16);
DigitalIn horn(p17);
DigitalIn spare_dig(p21);
DigitalIn cruise_inc(p23);
DigitalIn cruise_dec(p24);

InterruptIn ignition(p5);
InterruptIn nitrous(p10);

AnalogIn throttle(p19);
AnalogIn brake(p20);
AnalogIn spare_an(p18);

// Serial pc(USBTX, USBRX);
// USBSerial
CAN can(p30, p29);

char motor_drive_array_l[8] = { 0x00 };
char motor_drive_array_r[8] = { 0x00 };

uint16_t throttle_val;
int throttle_val_32bit;
float motor_current_demand;

int max_rpm = (MAX_VELOCITY_MPH * 0.44704)/WHEEL_RADIUS_M * 9.5493; // 1 mph = 0.44704 m/s and 1 rad/s = 9.5493 rpm
float motor_rpm_demand;
float motor_rpm_percentage;
float throttle_percentage;
float acceleration;
float dt = 0.1;

bool ignition_state = false;
bool prev_ignition_state = false;
bool nitrous_state = false;
bool prev_nitrous_state = false;
bool cruise_state = false;
bool prev_cruise_state = false;
bool prev_reverse_state = false;
bool prev_solar_state = false;
bool CAN_data_sent;
bool heartbeat_flag = false;

char drive_message_array[1] = {0x00};

void integrate(void);
void read_pedals(void);
void read_switches(void);
void ignition_pressed(void);
void nitrous_pressed(void);

uint32_t get_float_bits(float);
void send_motor_drive_command(int, float);
void send_drive_message(void);
void beat(void);
void CANReceiveRoutine(void);

//Ticker integrator;
Ticker driver_controls_heartbeat;


int main(void) {
	printf("Hello!\n");
	can.frequency(500000);
	can.attach(&CANReceiveRoutine, CAN::RxIrq);
    can.attach(&CANDataSentCallback, CAN::TxIrq);

    driver_controls_heartbeat.attach(&beat, 0.125);

	//integrator.attach(&integrate, dt);

	ignition.rise(&ignition_pressed);
	nitrous.rise(&nitrous_pressed);

	ignition.mode(PullNone);
	nitrous.mode(PullNone);

	drive_message_array[0] = 0x00;
	send_drive_message();

	while(1) {
		//Check if ignition is toggled and update bit 0 of drive_message_array before sending a drive message
		if(ignition_state != prev_ignition_state) {
			prev_ignition_state = ignition_state;
			if(ignition_state) {
				drive_message_array[0] |= 1<<0;
				send_drive_message();
				read_pedals();
				while((throttle_percentage > 0 || nitrous_state == false) && ignition_state){
					read_pedals();
				}
			}

			else{
				drive_message_array[0] &= ~(1<<0);
				send_drive_message();
			}
			
		}

		//Check if the reverse switch is toggled and update bit 1 of drive_message_array before sending a drive message
		if(reverse_state != prev_reverse_state){
			prev_reverse_state = reverse_state;
			if(reverse_state){
				drive_message_array[0] |= 1<<1;
				send_drive_message();
			}

			else{
				drive_message_array[0] &= ~(1<<1);
				send_drive_message();
			}
		}

		//Check if the solar switch is toggled and update bit 2 of drive_message_array before sending a drive message
		if(solar_state != prev_solar_state){
			prev_solar_state = solar_state;
			if(solar_state){
				drive_message_array[0] |= 1<<2;
				send_drive_message();
			}

			else{
				drive_message_array[0] &= ~(1<<2);
				send_drive_message();
			}
		}

		read_pedals();
		if(ignition_state /*ADD: AND BRAKE ISN'T PRESSED*/){
			if(heartbeat_flag){
				heartbeat_flag = false;
				send_motor_drive_command(motor_rpm_demand, motor_current_demand);
			}
		}

	}
}

void integrate() {
	motor_rpm_percentage += acceleration * dt;

	if(motor_rpm_percentage > 1.0)
		motor_rpm_percentage = 1.0;
	else if(motor_rpm_percentage < 0)
		motor_rpm_percentage = 0;

	motor_rpm_demand = motor_rpm_percentage * max_rpm;
} 

void read_pedals() {
	throttle_val = throttle.read_u16();
	throttle_percentage = throttle_val/65535.0;
	throttle_percentage = (throttle_percentage - THROTTLE_POT_MIN)/(THROTTLE_POT_MAX - THROTTLE_POT_MIN);
	if(throttle_percentage < 0.0)
		throttle_percentage = 0.0;
	else if(throttle_percentage > 1.0)
		throttle_percentage = 1.0;


	if(nitrous_state) {
		if(ignition_state) {
			motor_current_demand = throttle_percentage;
		    motor_rpm_demand = 9999.0;
		}
		else {
			motor_current_demand = 0.0;
			acceleration = 0.0;
			motor_rpm_percentage = 0.0;
			motor_rpm_demand = 0.0;
		}
	}

	else {
		//one pedal drive code here
		if(ignition_state) {

			motor_current_demand = 0.0;
			acceleration = 0.0;
			motor_rpm_percentage = 0.0;
			motor_rpm_demand = 0.0;

		}
		else {
			motor_current_demand = 0.0;
			acceleration = 0.0;
			motor_rpm_percentage = 0.0;
			motor_rpm_demand = 0.0;
		}
	}

	if(reverse_state == 1)
		motor_rpm_demand = -motor_rpm_demand;
}

void ignition_pressed() {
	ignition_state = !ignition_state;
}

void nitrous_pressed() {
	nitrous_state = !nitrous_state;
}

void send_motor_drive_command(int rpm, float current){
	uint32_t motor_rpm_bits = get_float_bits(motor_rpm_demand);
	for(int i = 0; i<4; i++) {
		motor_drive_array_l[i] = (motor_rpm_bits >> (8*i)) & 0xff;
		motor_drive_array_r[i] = (motor_rpm_bits >> (8*i)) & 0xff;
	}

	uint32_t motor_current_bits = get_float_bits(current);
	for(int i = 4; i<8; i++){
		motor_drive_array_l[i] = (motor_current_bits >> (8*(i-4))) & 0xff;
		motor_drive_array_r[i] = (motor_current_bits >> (8*(i-4))) & 0xff;
	}

	CANMessage motor_drive_command_l(DRIVER_CONTROLS_BASE_ADDR_L + 0X01, motor_drive_array_l, 8);
	CANMessage motor_drive_command_r(DRIVER_CONTROLS_BASE_ADDR_R + 0X01, motor_drive_array_r, 8);

	can_send(motor_drive_command_l);
	can_send(motor_drive_command_r);
}

void send_drive_message(void) {
	CANMessage drive_message(DRIVE_MESSAGE_ADDR, drive_message_array, 1);
	can_send(drive_message);
}

void onepedaldrive() {
	 if(throttle_percentage > 0.666) {
		motor_current_demand = (throttle_percentage - 0.666)/0.333;
		if(motor_current_demand > 1.0){
			motor_current_demand = 1.0;
			acceleration = motor_current_demand;
		}
		else if(throttle_percentage < 0.333) {
			motor_current_demand = (0.333 - throttle_percentage) / 0.333;
			acceleration = -motor_current_demand;
		}
		else {
			motor_current_demand = 0.0;
			acceleration = 0.0;
		}

	}
}

inline uint32_t get_float_bits(float f) {
    uint32_t bits;
    memcpy(&bits, &f, sizeof f);
    //printf("%i\r\n", bits);
    return bits;
}

void beat(){
	heartbeat_flag = true;
}

void CANReceiveRoutine(void){

}