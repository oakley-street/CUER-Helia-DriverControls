class Button {
public:
    bool state;
    bool prev_state;
    bool locking;

    void rise();
    void fall();
    Button() = default;
};