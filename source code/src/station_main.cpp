#ifdef ISTA

#include <Arduino.h>
#include <avr/interrupt.h>
#include <Adafruit_AHTX0.h>
#include <RTClib.h>

#include "inputs.h"
#include "radio.h"
#include "screen.h"
#include "time.h"
#include "data_structure.hpp"

InputsManager im;
RadioManager rm;
TimeManager tm;
ScreenManager sm; // screen manager with integrated menu interface
Adafruit_AHTX0 sensor; // adafruit AHT20 sensor (temp + hum)

DateTime last_reception((uint32_t)0);
DateTime last_sensor_poll((uint32_t)0);
sensors_event_t temp, hum;

void setup() {
    sqw_pin_intrr_setup();

    /* SET BUTTON PRESSES CALLBACKS*/
    for (int pin = A0; pin < A4; pin++) 
    {
        im.add_input_cb(pin, [](uint8_t input_name, uint8_t input_state) {
            sm.post_input(input_name, input_state);
        });
    }

    /*SET TIME UPDATE CALLBACK*/
    tm.add_update_cb([](DateTime now){
        sm.set_time(now.year(), now.month(), now.day() ,now.hour(), now.minute(), now.second()); // update the time on the screen

        if((now - last_sensor_poll).seconds() > 30)
        {
            last_sensor_poll = now;
            sensor.getEvent(&hum, &temp);
            sm.set_hum(hum.relative_humidity);
            sm.set_temp(temp.temperature); // get the temperature and humidity from the sensor
        }

        if ((now - last_reception).minutes() >= 1)
        {
            sm.set_signal_strength_warning(true);
            sm.set_ext_temp(0);
            sm.set_ext_hum(0);
        }
        
        sm.request_refresh();
    }); // register a callback that is being executed when the time manager is updating its local time

    sm.add_timeset_cb([](uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec){
        tm.set_time(year, month, day, hour, min, sec); 
    });

    /*SET THE RADIO RECEIVE CALLBACK*/
    rm.add_rcv_cb([](void *buf, uint8_t len, int8_t rssi){
        last_reception = tm.get_time();
        if (len == 0) return;
        
        paquet_data_strucutre * payload = static_cast<paquet_data_strucutre*>(buf);
        
        sm.set_ext_temp(payload->temperature);
        sm.set_ext_hum(payload->humidity); 
        sm.set_bat_level_warning(payload->battery_voltage < 3.5 ? true : false);
        sm.set_signal_strength_warning(rssi < -90 ? true : false);
    });

    /*INIT PART*/
    tm.init(DS3231_DEFAULT_ADDRESS);
    sm.init(SCREEN_DEFAULT_ADDRESS);
    im.init();
    rm.init();
    sensor.begin();

    sensor.getEvent(&hum, &temp);
    sm.set_hum(hum.relative_humidity);
    sm.set_temp(temp.temperature); // get the first temperature and humidity from the sensor
}

void loop() {
    // infinite non blocking loop to simulate pseudo-RTOS system
    tm.run(); // run the time manager to update the time and call the callbacks
    im.run(); // run the inputs manager to check for button presses
    sm.run(); // run the screen manager to refresh the display if needed
    rm.run();
}

ISR(PCINT0_vect){ // 1Hz clock interrrupt from the rtc's SQW pin - but because of pin change (rising + falling), it is more like 2Hz
    tm.request_update();
}

#endif