#include "time.h"

TimeManager::TimeManager()
{
    m_pending_time_update = false;
    m_time_change = false;
    m_rtc = nullptr;
}

TimeManager::~TimeManager()
{
    /*if (m_rtc)
        delete m_rtc;*/
}

void TimeManager::init(uint8_t addr)
{
    m_rtc = new RTC_DS3231();

    m_rtc->begin();
    m_rtc->writeSqwPinMode(DS3231_SquareWave1Hz);

    update(); // the one and only request to the rtc. 
              // The rest is managed by the controller itself trough the interrupt signals the clock sends (see tick() method).
}

void TimeManager::run()
{
    if (m_pending_time_update) {
        update();
    }
    if (m_time_change && m_update_callback) {
        m_update_callback(m_current_time); // execute the update callbak for the screen
        m_time_change = false;
    }
}

void TimeManager::update()
{
    m_pending_time_update = false;
    m_current_time = m_rtc->now();
    m_time_change = true;
}

void TimeManager::request_update()
{
    m_pending_time_update = true;
}

void TimeManager::tick()
{
    TimeSpan second(1);
    m_current_time = m_current_time + second;
    m_time_change = true;
}

void TimeManager::add_update_cb(void (*cb)(DateTime now))
{
    m_update_callback = cb;
}

DateTime TimeManager::get_time()
{
    return m_current_time;
}

void TimeManager::set_time(uint16_t y, uint8_t mo, uint8_t d, uint8_t h, uint8_t mi, uint8_t s)
{
    DateTime dt(y, mo, d, h, mi, s);
    m_rtc->adjust(dt);
}

void sqw_pin_intrr_setup()
{
    // setup interrupt for the 1hz clock from the rtc's SQW pin
    PCMSK0 = bit (PCINT5);  // want pin 9
    PCIFR  = bit (PCIF0);   // clear any outstanding interrupts
    PCICR  = bit (PCIE0);   // enable pin change interrupts for D0 to D7
}
