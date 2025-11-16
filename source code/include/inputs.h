#pragma once
#include <stdint.h>

#define INPUT_PRESSED 0
#define INPUT_RELEASED 1

#define pin_to_index(pin) (pin - A0) // convert pin number to index in the input state array

typedef void (*InputCb_t)(uint8_t input_name, uint8_t input_state);

class InputsManager
{
private:
    uint8_t m_input_state[4]; // 4 inputs
    void (*m_callback[4])(uint8_t input_name, uint8_t input_state);

    void check_inputs(); // check the state of the inputs and call the callback if needed
public:
    InputsManager(/* args */);
    ~InputsManager();

    void init();
    void run();

    void add_input_cb(uint8_t input_name, InputCb_t cb);
};

