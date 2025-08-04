#ifdef SENDER

#include <Arduino.h>
#include <RH_RF69.h>
#include <ArduinoJson.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_SleepyDog.h>
#include <avr/wdt.h>
#include <avr/power.h>

#include "Radio-manager.h"

#define VBATPIN A9

RH_RF69 radio(RFM69_CS, RFM69_INT);
Adafruit_AHTX0 aht;
JsonDocument doc;

float vbat = 0;

void update_vbat()
{
  vbat = analogRead(VBATPIN);
  vbat *= 2;
  vbat *= 3.3;
  vbat /= 1024;
}

void setup() {
  Serial.begin(115200);

  // Init radio
  radio.init();
  radio.setFrequency(RF69_FREQ);
  radio.setTxPower(20, true);  // Adjust based on range / power needs
  radio.setIdleMode(RH_RF69_OPMODE_MODE_SLEEP);

  // Init AHT20 sensor
  aht.begin();
  Watchdog.enable(4000);
}

void loop() {
  // Re-enable ADC
  power_adc_enable();
  ADCSRA |= (1 << ADEN);

  // Wake up sensor, get data
  update_vbat();
  
  doc.clear();  // Clear previous JSON content
  
  if (vbat < 3.7) {
    doc["lowbat"] = true;
  } else {
    doc["lowbat"] = false;
  }

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  doc["temp"] = temp.temperature;
  doc["hum"] = humidity.relative_humidity;

  char payload[RH_RF69_MAX_MESSAGE_LEN];
  size_t len = serializeJson(doc, payload, sizeof(payload));

  // Send data
  radio.send((uint8_t*)payload, len);
  radio.waitPacketSent();

  // Put the radio into deep sleep
  radio.sleep();

  // Disable ADC and other peripherals to save power
  ADCSRA &= ~(1 << ADEN);
  power_adc_disable();
  power_spi_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();

  // Optional: disable USB
  USBCON |= (1 << FRZCLK);
  PLLCSR &= ~(1 << PLLE);
  USBCON &= ~(1 << USBE);

  // Now enter deep sleep
  Watchdog.sleep(30000);
}


#endif
