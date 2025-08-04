#include "Radio-manager.h"

RadioManager::RadioManager()
{
    m_rcv_cb = nullptr;
    m_radio = nullptr;
    memset(m_buf, 0, sizeof(m_buf)); // clear the buffer
}

RadioManager::~RadioManager()
{
}

void RadioManager::init()
{
    m_radio = new RH_RF69 (RFM69_CS, RFM69_INT);
    pinMode(RFM69_RST, OUTPUT);
    digitalWrite(RFM69_RST, LOW);
    // manual reset
    digitalWrite(RFM69_RST, HIGH);
    delay(10);
    digitalWrite(RFM69_RST, LOW);
    delay(10);

    m_radio->init();
    m_radio->setFrequency(RF69_FREQ);
    m_radio->setTxPower(20, true);
}

void RadioManager::run()
{
    uint8_t len = sizeof(m_buf);
    if (m_radio->recv(m_buf, &len))
    {
        m_rcv_cb(m_buf, len, m_radio->lastRssi());
    }
}

void RadioManager::add_rcv_cb(RadioRcvCb_t cb)
{
    m_rcv_cb = cb;
}
