#define DRIVE_MESSAGE_ADDR 0x500

// Left Wavesculptor base address
#define DRIVER_CONTROLS_BASE_ADDR_L 0x500

// Right Wavesculptor base address
#define DRIVER_CONTROLS_BASE_ADDR_R 0x550

#define LIGHTS_MESSAGE_ADDR 0x730

#define BMS_MESSAGE_ADDR 0x750

enum CanMessageId {
    // Charger and Driver Controls
    Msg_DriverControlsToCharger = 0x1806E5F4,
    Msg_ChargerToDriverControls = 0x18FF50E5,

    // IVT addresses
    Msg_FrontIVTStart = 0x520, // Note: Actual range and messages will depend on implementation
    Msg_RearIVTStart = 0x530,

    // MPPT messages for simplicity, actual implementation may require functions to compute these dynamically
    Msg_MPPT1_Base = 0x180,
    Msg_MPPT2_Base = 0x280,
    Msg_MPPT3_Base = 0x480,

    // Additional messages
    Msg_Lights = 0x730,
    Msg_MotorControllerL_Base = 0x200,
    Msg_MotorControllerR_Base = 0x220,
    // Add remaining addresses as needed
};

// Define similar structures for other messages
struct IVTCommand {
    uint16_t voltage;
    uint16_t current;
    uint8_t ivtControl;
};

struct IVTStatus {
    uint16_t voltage;
    uint16_t current;
    uint8_t faultStates;
};

struct MotorControllerStatus {
    uint16_t speed;
    uint16_t torque;
    uint8_t faultStates;
};

struct BMSCommand {
    uint16_t voltage;
    uint16_t current;
    uint8_t bmsControl;
};

struct MotorControllerCommand {
    uint16_t speed;
    uint16_t torque;
    uint8_t control;
};

struct MPPTCommand {
    uint16_t voltage;
    uint16_t current;
    uint8_t mpptControl;
};

struct MPPTStatus {
    uint16_t voltage;
    uint16_t current;
    uint8_t faultStates;
};

struct LightsCommand {
    uint8_t lights;
};

struct DriverControls {
    uint16_t throttle;
    uint16_t regen;
    uint8_t driveMode;
    uint8_t brake;
    uint8_t lights;
};

struct ChargingCommand {
    uint16_t chargingVoltage;
    uint16_t chargingCurrent;
    uint8_t chargingControl;
};

struct ChargingStatus {
    uint16_t actualVoltage;
    uint16_t actualCurrent;
    uint8_t faultStates;
};

struct CanFrame {
    uint32_t id;      // CAN message ID
    uint8_t length;   // Length of the data field
    uint8_t data[8];  // Data field, maximum of 8 bytes for standard CAN
};

void serializeChargingCommand(const ChargingCommand& cmd, CanFrame& frame) {
    // Convert structure to CAN frame
    frame.id = Msg_ChargerToDriverControls;
    frame.length = 8;
    frame.data[0] = cmd.chargingVoltage & 0xFF;
    frame.data[1] = cmd.chargingVoltage >> 8;
    frame.data[2] = cmd.chargingCurrent & 0xFF;
    frame.data[3] = cmd.chargingCurrent >> 8;
    frame.data[4] = cmd.chargingControl;
    // Add remaining data as needed
}

void deserializeChargingCommand(const CanFrame& frame, ChargingCommand& cmd) {
    // Convert CAN frame back to structure
    cmd.chargingVoltage = frame.data[0] | (frame.data[1] << 8);
    cmd.chargingCurrent = frame.data[2] | (frame.data[3] << 8);
    cmd.chargingControl = frame.data[4];
    // Extract remaining data as needed
}

// Main function to demonstrate usage
int main() {
    // Initialize CAN library and hardware
    // CAN.begin();

    ChargingCommand cmdToSend = {120, 15, 1}; // Example command with dummy data
    CanFrame outgoingFrame;

    serializeChargingCommand(cmdToSend, outgoingFrame);
    // CAN.sendFrame(outgoingFrame); // Send frame using CAN library

    // Example for receiving a frame
    CanFrame incomingFrame;
    // Assume incomingFrame is filled by the CAN library upon receiving a message
    ChargingCommand cmdReceived;
    deserializeChargingCommand(incomingFrame, cmdReceived);

    // Use cmdReceived as needed...

    return 0;
}