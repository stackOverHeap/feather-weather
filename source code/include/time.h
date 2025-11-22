#pragma once
#include <Arduino.h>
#include <avr/io.h>
#include <RTClib.h>


#define DS3231_DEFAULT_ADDRESS 0x68

// Time manager that keeps time and sends refresh request to the screen every seconds
class TimeManager
{
private:
    void (*m_update_callback)(DateTime now) = nullptr; //for screen refresh callback on interrupt
    bool m_pending_time_update;
    RTC_DS3231 * m_rtc;
    DateTime m_current_time;

public:
    TimeManager();
    ~TimeManager();

    void init(uint8_t addr = DS3231_DEFAULT_ADDRESS);

    void run(); // idle task (must be fast and non-blocking)

    void update();
    void request_update(); // update the time to sync with the rtc
    void add_update_cb(void (*cb)(DateTime now)); // add a function callback, called when an update was triggered

    DateTime get_time();
    void set_time(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);
};

void sqw_pin_intrr_setup();