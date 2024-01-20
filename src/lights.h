#include <mbed.h>
// State bit masks
const char HEADLIGHTS = 0b0000'0001;
const char FRONT_FOG = 0b0000'0010;
const char REAR_FOG = 0b0000'0100;
const char LEFT_IND = 0b0000'1000;
const char RIGHT_IND = 0b0001'0000;
const char HAZARDS = 0b0010'0000;
const char HORN = 0b0100'0000;

class Lights {
   public:
    char prev_state;
    char state;

    char message_array[8];

    DigitalIn* headlights;
    DigitalIn* front_fog;
    DigitalIn* rear_fog;
    DigitalIn* left_ind;
    DigitalIn* right_ind;
    DigitalIn* hazards;
    DigitalIn* horn;

    Lights(DigitalIn* p_headlights, DigitalIn* p_front_fog, DigitalIn* p_rear_fog, DigitalIn* p_left_ind, DigitalIn* p_right_ind, DigitalIn* p_hazards, DigitalIn* p_horn);
    void set(char mask);
    void clear(char mask);
    void adjust(char mask, int input);
    void adjust_locking(char mask, int input);
    void init();
    void poll();

   private:
};