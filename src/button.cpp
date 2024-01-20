#include "button.h"

void Button::rise() {
    if (locking) {
        prev_state = state;
        state = !state;
    } else {
        prev_state = false;
        state = true;
    }
}

void Button::fall() {
    if (!locking) {
        prev_state = true;
        state = false;
    }
}

// Button::Button(bool locking_mode) {
//     locking = locking_mode
// }