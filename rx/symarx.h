#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

class SymaRX {
    byte checksum(byte*);

    byte channels[4];
    byte data[10]; 
    long prev_rx_time;
    uint8_t current_channel;
    
  public:
    SymaRX();
    void init(RF24&);
    void run(RF24&);
    void read(byte*);
    void set_channel(void);

    byte addr[5];
    bool available;
    bool bound;
};

