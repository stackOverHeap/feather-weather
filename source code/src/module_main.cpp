#ifdef ESTA

#include <Arduino.h>
#include <RH_RF69.h>
#include <ArduinoJson.h>
#include <Adafruit_AHTX0.h>

#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

#include "radio.h"
#include "data_structure.hpp"

#define VBATPIN A9
#define WKPIN 0 // ne555 out
#define TRGPIN 1 // ne555 trig

#define MIN_BATTERY_VOLTAGE 3.5f

RH_RF69 radio(RFM69_CS, RFM69_INT);
Adafruit_AHTX0 aht;

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
  radio.setTxPower(15, true);  // Adjust based on range / power needs
  radio.setIdleMode(RH_RF69_OPMODE_MODE_SLEEP);

  // Init AHT20 sensor
  aht.begin();
  pinMode(WKPIN, INPUT_PULLUP); 
  // Configure INT2 (D0) to trigger on falling edge
  attachInterrupt(digitalPinToInterrupt(WKPIN), wakeup_isr, FALLING);

  pinMode(TRGPIN, OUTPUT);
  digitalWrite(TRGPIN, HIGH); // prepare for ne555 wakeup pulse

  power_usart0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_usb_disable();
  wdt_disable();
  
}

void loop() {  
  EIMSK &= ~(1 << INT2);    // Disable INT2 interrupt

  // Wake up sensor, get data
  update_vbat();

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  paquet_data_strucutre payload 
  {
    .temperature = temp.temperature,
    .humidity = humidity.relative_humidity,
    .battery_voltage = vbat
  };

  // Send data
  radio.send(reinterpret_cast<uint8_t*>(&payload), sizeof(paquet_data_strucutre));
  radio.waitPacketSent();

  // Put the radio into sleep mode

  // Disable ADC and other peripherals to save power
  power_adc_disable();
  power_twi_disable();
  power_spi_disable();
  power_timer0_disable();

  // Now enter deep sleep
  digitalWrite(TRGPIN, LOW);
  delayMicroseconds(10);
  digitalWrite(TRGPIN, HIGH);

  EIFR = bit(INTF2); // clear pending INT2
  EIMSK |= (1 << INT2);    // Enable INT2 interrupt
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  sleep_mode();

  // Re-enable ADC
  power_adc_enable();
  power_twi_enable();
  power_spi_enable();
  power_timer0_enable();
}

#endif
