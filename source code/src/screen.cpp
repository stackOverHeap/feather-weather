#include "screen.h"
#include <Arduino.h>
#include "inputs.h"
#include "icons.h"

// CLASS DEFINITIONS

// Constructor
ScreenManager::ScreenManager()
{
    m_time_config_cb = nullptr;
    m_lcd = nullptr;

    memset(m_data_ln1, 0, sizeof(m_data_ln1));
    memset(m_data_ln2, 0, sizeof(m_data_ln2));

    m_dataChanged = false;
    m_pending_refresh = false;

    // default screen is the temperature
    m_input_handler = &handle_input_temp;
    m_renderer = &render_screen_temp;

    /*INTERFACE*/
    // settings
    //m_settings_selector_pos = 0;
    // clock config
    m_clock_config_stage = 0;
    m_clock_config_active = false;
    // calibration config
    //m_calibration_config_stage = 0;

    // Initialize sensor/time data
    m_temp = 0;
    m_ext_temp = 0;
    m_hum = 0;
    m_ext_hum = 0;
    m_bat_warning = true;
    m_signal_strength_warning = true;

    m_time.hour = 0;
    m_time.minute = 0;
    m_time.second = 0;
}

// Destructor
ScreenManager::~ScreenManager()
{
    // Uncomment if manual cleanup is needed
    // if (m_lcd) delete m_lcd;
}

// Initialize LCD screen and load custom characters
void ScreenManager::init(uint8_t addr)
{
    m_lcd = new LiquidCrystal_I2C(addr, 16, 2);
    m_lcd->init();
    m_lcd->backlight();

    //m_lcd->createChar(1, (uint8_t*)&icon_AM64);
    //m_lcd->createChar(2, (uint8_t*)&icon_PM64);
    m_lcd->createChar(3, (uint8_t*)&icon_droplet64);
    m_lcd->createChar(4, (uint8_t*)&icon_temp64);
    m_lcd->createChar(5, (uint8_t*)&icon_lowbat64);
    m_lcd->createChar(6, (uint8_t*)&icon_badrssi64);
}

// Update data only if changed
void ScreenManager::update_data()
{
    if (m_dataChanged && m_renderer != nullptr)
    {
        m_renderer(*this);     // Call current render function
        m_dataChanged = false; // Clear change flag
    }
}

// Called every loop iteration (or as needed)
void ScreenManager::run()
{
    if (m_pending_refresh)
        refresh();
}

// Request a screen update (set flag)
void ScreenManager::request_refresh()
{
    m_pending_refresh = true;
}

// Actually update the screen
void ScreenManager::refresh()
{
    m_pending_refresh = false;
    update_data();

    m_lcd->setCursor(0, 0);
    m_lcd->print(m_data_ln1);
    m_lcd->setCursor(0, 1);
    m_lcd->print(m_data_ln2);
}

// Called when input is detected
void ScreenManager::post_input(uint8_t input_name, uint8_t input_state)
{
    if (input_state == INPUT_RELEASED)
    {
        m_dataChanged = true;
        m_input_handler(*this, input_name, input_state);
    }
    m_pending_refresh = true;
}

// Setters that mark data as changed
void ScreenManager::set_time(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    if(!m_clock_config_active)
    {
        m_time.year = year;
        m_time.month = month;
        m_time.day = day;
        m_time.hour = hour;
        m_time.minute = min;
        m_time.second = sec;
        m_dataChanged = true;
    }
}

void ScreenManager::set_temp(float temp)
{
    m_temp = temp;
    m_dataChanged = true;
}

void ScreenManager::set_ext_temp(float ext_temp)
{
    m_ext_temp = ext_temp;
    m_dataChanged = true;
}

void ScreenManager::set_hum(float hum)
{
    m_hum = hum;
    m_dataChanged = true;
}

void ScreenManager::set_ext_hum(float ext_hum)
{
    m_ext_hum = ext_hum;
    m_dataChanged = true;
}

void ScreenManager::set_bat_level_warning(bool islow)
{
    m_bat_warning = islow;
}

void ScreenManager::set_signal_strength_warning(bool isbad)
{
    m_signal_strength_warning = isbad;
}

void ScreenManager::add_timeset_cb(TimeConfigCb_t cb)
{
    m_time_config_cb = cb;
}
