#ifdef ESTA

#include <Arduino.h>
#include <RH_RF69.h>
#include <ArduinoJson.h>
#include <Adafruit_AHTX0.h>

#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

#include "Radio-manager.h"

#define VBATPIN A9
#define WKPIN 0 // ne555 out
#define TRGPIN 1 // ne555 trig

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

void wakeup_isr()
{
  digitalWrite(TRGPIN, HIGH);
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
  pinMode(WKPIN, INPUT_PULLUP); 
  // Configure INT2 (D0) to trigger on falling edge
  attachInterrupt(digitalPinToInterrupt(WKPIN), wakeup_isr, FALLING);

  pinMode(TRGPIN, OUTPUT);
  digitalWrite(TRGPIN, HIGH); // prepare for ne555 wakeup pulse

  power_timer1_disable();
  power_timer2_disable();
  
  USBCON |= (1 << FRZCLK);
  PLLCSR &= ~(1 << PLLE);
  USBCON &= ~(1 << USBE);

  wdt_disable();
  
}

void loop() {  
  EIMSK &= ~(1 << INT2);    // Disable INT2 interrupt
  // Re-enable ADC
  power_adc_enable();
  ADCSRA |= (1 << ADEN);
  power_twi_enable();
  power_spi_enable();

  // Wake up sensor, get data
  update_vbat();
  
  doc.clear();  // Clear previous JSON content
  
  if (vbat < 3.5) {
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

  // Put the radio into sleep mode
  radio.sleep();
  
  // Disable ADC and other peripherals to save power
  ADCSRA &= ~(1 << ADEN);
  power_adc_disable();
  power_twi_disable();
  power_spi_disable();

  // Now enter deep sleep
  digitalWrite(TRGPIN, LOW);
  delayMicroseconds(10);
  digitalWrite(TRGPIN, HIGH);
  EIFR = bit(INTF2); // clear pending INT2
  EIMSK |= (1 << INT2);    // Enable INT2 interrupt
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_mode();
}

#endif
