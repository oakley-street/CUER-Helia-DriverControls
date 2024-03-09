class Button {
public:
    Button(bool locking_mode); // Constructor declaration

    bool state;
    bool prev_state;
    bool locking;

    void rise();
    void fall();
    Button() = default;
};
