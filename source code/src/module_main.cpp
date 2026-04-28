#ifdef MODULE

#include <Arduino.h>
#include <RH_RF69.h>
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

#define SLEEP_TIME_S 64
#define SLEEP_CYCLE_COUNT (SLEEP_TIME_S / 8)

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

ISR(WDT_vect) {}

void setup() {
  Serial.begin(115200);

  // Init radio
  radio.init();
  radio.setFrequency(RF69_FREQ);
  radio.setTxPower(15, true);  // Adjust based on range / power needs
  radio.setIdleMode(RH_RF69_OPMODE_MODE_SLEEP);

  // Init AHT20 sensor
  aht.begin();

#ifdef NE555
  pinMode(WKPIN, INPUT_PULLUP); 
  // Configure INT2 (D0) to trigger on falling edge
  attachInterrupt(digitalPinToInterrupt(WKPIN), wakeup_isr, FALLING);

  pinMode(TRGPIN, OUTPUT);
  digitalWrite(TRGPIN, HIGH); // prepare for ne555 wakeup pulse

  wdt_disable();
#endif

  power_usart0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_timer3_disable();
  power_usb_disable();
}

void loop() {  
#ifdef NE555
  EIMSK &= ~(1 << INT2);    // Disable INT2 interrupt
#endif
  // Wake up sensor, get data
  analogRead(VBATPIN); // dummy read
  update_vbat();

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  paquet_data_structure payload 
  {
    .temperature = temp.temperature,
    .humidity = humidity.relative_humidity,
    .battery_voltage = vbat
  };

  // Send data
  radio.send(reinterpret_cast<uint8_t*>(&payload), sizeof(paquet_data_structure));
  radio.waitPacketSent();

  // Put the radio into sleep mode

  // Disable ADC and other peripherals to save power
  power_adc_disable();
  power_twi_disable();
  power_spi_disable();
  power_timer0_disable();

#ifdef NE555
  // Now enter deep sleep
  digitalWrite(TRGPIN, LOW);
  delayMicroseconds(10);
  digitalWrite(TRGPIN, HIGH);
  EIFR = bit(INTF2); // clear pending INT2
  EIMSK |= (1 << INT2);    // Enable INT2 interrupt
#endif

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

#ifdef NE555
  sleep_mode();
#else
  int cycle_count = 0;

  sleep_enable();

  cli();
  wdt_reset();
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0); // interrupt only
  sei();

  while (cycle_count < SLEEP_CYCLE_COUNT)
  {
    sleep_cpu();
    cycle_count++;
  }

  cli();
  wdt_reset();
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = 0x00; // WDT fully off
  sei();

  sleep_disable();
#endif

  //  Re-enable ADC
  power_adc_enable();
  power_twi_enable();
  power_spi_enable();
  power_timer0_enable();
}

#endif
