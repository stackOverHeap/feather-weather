#ifdef ISTA

#include <Arduino.h>
#include <avr/interrupt.h>
#include <Adafruit_AHTX0.h>
#include <ArduinoJson.h>
#include <RTClib.h>

#include "Inputs-manager.h"
#include "Radio-manager.h"
#include "Screen-manager.h"
#include "Time-manager.h"

InputsManager im;
RadioManager rm;
TimeManager tm;
ScreenManager sm; // screen manager with integrated menu interface
Adafruit_AHTX0 sensor; // adafruit ATH20 sensor (temp + hum)

const float temp_offset = -0.7f;
const float hum_offset = 0.0f;

DateTime last_reception((uint32_t)0);

void setup() {
    sqw_pin_intrr_setup();

    Serial.begin(115200);

    /* SET BUTTON PRESSES CALLBACKS*/
    for (int pin = A0; pin < A4; pin++) 
    {
        im.add_input_cb(pin, [](uint8_t input_name, uint8_t input_state) {
            sm.post_input(input_name, input_state);
        });
    }

    /*SET TIME UPDATE CALLBACK*/
    tm.add_update_cb([](){
        sm.set_time(tm.get_time().year(), tm.get_time().month(), tm.get_time().day() ,tm.get_time().hour(), tm.get_time().minute(), tm.get_time().second()); // update the time on the screen
    
        sensors_event_t temp, hum;
        sensor.getEvent(&hum, &temp);
    
        sm.set_hum(hum.relative_humidity + hum_offset);
        sm.set_temp(temp.temperature + temp_offset); // get the temperature and humidity from the sensor

        if ((tm.get_time() - last_reception).minutes() >= 1)
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
        Serial.println((char*)buf);
        JsonDocument doc;
        deserializeJson(doc, buf, len);
        sm.set_ext_temp(float(doc["temp"]) + temp_offset);
        sm.set_ext_hum(float(doc["hum"]) + hum_offset); 
        sm.set_bat_level_warning(bool(doc["lowbat"]));
        sm.set_signal_strength_warning(rssi < -90 ? true : false);
    });

    /*INIT PART*/
    tm.init(DS3231_DEFAULT_ADDRESS);
    sm.init(SCREEN_DEFAULT_ADDRESS);
    im.init();
    rm.init();
    sensor.begin();
}

void loop() {
    // infinite non blocking loop to simulate pseudo-RTOS system
    tm.run(); // run the time manager to update the time and call the callbacks
    im.run(); // run the inputs manager to check for button presses
    sm.run(); // run the screen manager to refresh the display if needed
    rm.run();
}

ISR(PCINT0_vect){ // 1hz clock interrrupt from the rtc's SQW pin
    tm.request_update();
}

#endif