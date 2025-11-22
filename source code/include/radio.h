#include <RH_RF69.h>

#define RF69_FREQ 434.0

#define RFM69_CS    8
#define RFM69_INT   7
#define RFM69_RST   4
#define LED        13

typedef void (*RadioRcvCb_t)(void * buf, uint8_t len, int8_t rssi);

class RadioManager
{
private:
    RH_RF69 * m_radio; // singleton instance
    uint8_t m_buf[RH_RF69_MAX_MESSAGE_LEN]; // buffer for the radio message
    uint8_t m_buf_len{sizeof(m_buf)};
    RadioRcvCb_t m_rcv_cb; // callback for the radio message reception

public:
    RadioManager();
    ~RadioManager();

    void init();
    void run();

    void add_rcv_cb(RadioRcvCb_t cb);
    void fetch_msg();
};