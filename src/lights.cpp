#include "lights.h"
#include "CUER_CAN.h"
#include "address.h"

Lights::Lights(DigitalIn* p_headlights, DigitalIn* p_front_fog, DigitalIn* p_rear_fog, DigitalIn* p_left_ind, DigitalIn* p_right_ind, DigitalIn* p_hazards, DigitalIn* p_horn) {
    prev_state = 0b0000'0000;
    state = 0b0000'0000;

    headlights = p_headlights;
    front_fog = p_front_fog;
    rear_fog = p_rear_fog;
    left_ind = p_left_ind;
    right_ind = p_right_ind;
    horn = p_horn;
}

void Lights::adjust(char mask, int input) {
    if (input) {
        state |= mask;
    } else {
        state &= !mask;
    }
}

void Lights::adjust_locking(char mask, int input) {
    if (input) {
        if (!(prev_state & mask)) {
            state |= mask;
        } else {
            state &= !mask;
        }
    }
}

void Lights::poll() {
    adjust_locking(HEADLIGHTS, *headlights);
    adjust(FRONT_FOG, *front_fog);
    adjust(REAR_FOG, *rear_fog);
    adjust_locking(LEFT_IND, *left_ind);
    adjust_locking(RIGHT_IND, *right_ind);
    adjust_locking(HAZARDS, *hazards);
    adjust(HORN, *horn);

    if (state != prev_state) {
        message_array[0] = state;
        CANMessage lights_message(LIGHTS_MESSAGE_ADDR, message_array, 8);
        can_send(lights_message);
    }
}