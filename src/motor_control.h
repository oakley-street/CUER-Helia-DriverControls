/*****************************************************************************************************\
 CUER Driver Controls v1.0 Code by Poline S (March 2024)

 This is an initial verision with the only aim of being a template, and none of the calculations have been checked so far.
\*****************************************************************************************************/

class MotorControl {
public:
    MotorControl();  // Constructor
    ~MotorControl(); // Destructor

    void initialize(); // Method to initialize the motor control
    void start();     // Method to start the motor control
    void stop();      // Method to stop the motor control
    void setSpeed(int speed); // Method to set the speed of the motor
    void increaseSpeed(int increment); // Method to increase the speed of the motor
    void decreaseSpeed(int decrement); // Method to decrease the speed of the motor
    void reverseDirection(); // Method to reverse the direction of the motor
    void brake(); // Method to apply brake to the motor

private:
    // Private member variables
    int speed;
    bool isRunning;
};