#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// screen manager with integrated menu interface
class ScreenManager; // Forward declaration of ScreenManager

#define SCREEN_DEFAULT_ADDRESS 0x27


struct Time_t
{
    uint16_t year;
    uint8_t month;
    uint8_t day;

    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

enum Button_t
{
    BTN0 = A0,
    BTN1 = A1,
    BTN2 = A2,
    BTN3 = A3
};

typedef void (*TimeConfigCb_t)(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec); // time configurator callback function

typedef void (Screen_renderer_t)(ScreenManager & context);
typedef void (Input_handler_t)(ScreenManager & context, uint8_t input, uint8_t state);

class ScreenManager
{
private:
    bool m_pending_refresh;
    bool m_dataChanged; // flag to indicate if the data has changed and needs to be updated on the screen
    char m_data_ln1 [17]; // screen data lines
    char m_data_ln2 [17];

    void update_data(); // update the data to be displayed on the screen

    LiquidCrystal_I2C * m_lcd;

private: // screen class internal data
    float m_temp; // current indoor temperature
    float m_ext_temp; // current outdoor temperature
    float m_hum; // current indoor humidity
    float m_ext_hum; // current outdoor humidity

    bool m_bat_warning;
    bool m_signal_strength_warning;
    Time_t m_time; // current time

private: // config callbacks
    TimeConfigCb_t m_time_config_cb;

private: // Screen rendering and input handling
    Screen_renderer_t *m_renderer;
    Input_handler_t *m_input_handler;

    static Screen_renderer_t render_screen_temp;
    static Input_handler_t handle_input_temp;

    static Screen_renderer_t render_screen_hum;
    static Input_handler_t handle_input_hum;

    //static Screen_renderer_t render_screen_settings;
    //static Input_handler_t handle_input_settings;
    
    static Screen_renderer_t render_screen_clock_config;
    static Input_handler_t handle_input_clock_config;

    //static Screen_renderer_t render_screen_calibration_config;
    //static Input_handler_t handle_input_calibration_config;

private: // interface
    // settings menu
    //static const char *m_settings_keys[];
    //static const Screen_renderer_t *m_settings_renderer_ptr[];
    //static const Input_handler_t *m_settings_input_handler_ptr[];
    //uint8_t m_settings_selector_pos; // should only be 0 or 1 since I'm using a LCD 1602
    // clock config
    static const char *m_clock_config_keys[];
    uint8_t m_clock_config_stage; // from 0 to 5 (year, month, day, hour, minute, seconds)
    bool m_clock_config_active; // authorise or not further automatic time update by the callback
    // calibration config
    //uint8_t m_calibration_config_stage; // from 0 to 3 (temp, ext_temp, hum, ext_hum)


public:
    ScreenManager(); // constructor with pointers to the time manager and sensor
    ~ScreenManager();

    void init(uint8_t addr);
    void run();

    // screen control functions
    void request_refresh();
    void refresh();
    void post_input(uint8_t input_name, uint8_t input_state);
    void switch_mode(uint8_t mode); // switch to a different mode (menu, settings, etc.)

    // data setters
    void set_time(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec); // set the time to be displayed on the screen
    void set_temp(float temp); // set the temperature to be displayed on the screen
    void set_ext_temp(float ext_temp);
    void set_hum(float hum); // set the humidity to be displayed on the screen
    void set_ext_hum(float ext_hum);
    void set_bat_level_warning(bool islow); // level is either good or low
    void set_signal_strength_warning(bool isbad); // rsi is either good or bad

    // callbacks
    void add_timeset_cb(TimeConfigCb_t cb);
};