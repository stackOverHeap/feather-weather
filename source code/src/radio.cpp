#include "radio.h"

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
    digitalWrite(RFM69_RST, LOW); // ensure the reset pin is not floating
    delay(10);

    m_radio->init();
    m_radio->setFrequency(RF69_FREQ);
    m_radio->setTxPower(20, true);

    m_radio->recv(m_buf, &m_buf_len); // seems to get bad readings on first reception. Trying to flush the reception buffer to get clean msg afterward.
}

void RadioManager::run()
{
    if (m_radio->available())
        fetch_msg();
}

void RadioManager::add_rcv_cb(RadioRcvCb_t cb)
{
    m_rcv_cb = cb;
}

void RadioManager::fetch_msg()
{
    if (m_radio->recv(m_buf, &m_buf_len))
        m_rcv_cb(m_buf, m_buf_len, m_radio->lastRssi());
}
