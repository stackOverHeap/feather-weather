#include "inputs.h"
#include <Arduino.h>

void InputsManager::check_inputs()
{
    for (int pin = A0; pin < A4; pin++)
    {
        uint8_t state = digitalRead(pin);
        if (state != m_input_state[pin_to_index(pin)])
        {
            m_input_state[pin_to_index(pin)] = state;
            if (m_callback[pin_to_index(pin)])
            {
                m_callback[pin_to_index(pin)](pin, state);
            }
        }
    }
}

InputsManager::InputsManager()
{
    for (int i = 0; i < 4; i++)
    {
        m_callback[i] = nullptr;
    }
    memset(m_input_state, 0, sizeof(m_input_state));
}

InputsManager::~InputsManager()
{
}

void InputsManager::init()
{
    for (int pin = A0; pin < A4; pin++)
    {
        pinMode(pin, INPUT_PULLUP);
        m_input_state[pin_to_index(pin)] = digitalRead(pin);
    }

}

void InputsManager::run()
{
    check_inputs();
}

void InputsManager::add_input_cb(uint8_t input_name, void(* callback)(uint8_t input_name,uint8_t input_state))
{
    if (pin_to_index(input_name) < 4)
    {
        m_callback[pin_to_index(input_name)] = callback;
    }
    else
    {
        // handle error
    }
}
