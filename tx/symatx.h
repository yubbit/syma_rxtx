#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

byte dump[10];

class SymaTX {
    byte checksum(byte*);

    byte channels[4];
    byte data[10];
    int current_channel;
    int channel_counter;
    
  public:
    SymaTX();
    void init(byte*, RF24&);
    void set_channel();
    void build_packet();
    void transmit(byte*, RF24&);
    void transmit(RF24&);

    byte addr[5];
    byte direction[4];
    bool bound;
};


SymaTX::SymaTX() {
    bound = false;

    channels[0] = 0x4b;
    channels[1] = 0x30;
    channels[2] = 0x40;
    channels[3] = 0x20;
    current_channel = 0;
    channel_counter = 0;

    addr[0] = 0xab;
    addr[1] = 0xac;
    addr[2] = 0xad;
    addr[3] = 0xae;
    addr[4] = 0xaf;

    direction[0] = 124;
    direction[1] = 0;
    direction[2] = 0;
    direction[3] = 0;

    return;
}    


void SymaTX::init(byte *tx_addr, RF24 &radio) {
    radio.stopListening();
    radio.setChannel(channels[0]);
    radio.openWritingPipe(addr);

    data[0] = tx_addr[4];
    data[1] = tx_addr[3];
    data[2] = tx_addr[2];
    data[3] = tx_addr[1];
    data[4] = tx_addr[0];
    data[5] = 0xaa;
    data[6] = 0xaa;
    data[7] = 0xaa;
    data[8] = 0x00;
    data[9] = checksum(data);

    for (int i=0; i<50; i++) {
        Serial.readBytes(dump, 11);
        transmit(radio);
    }

    memcpy(addr, tx_addr, 5);
    build_packet();
    set_channel();
    radio.openWritingPipe(addr);
    bound = true;

    return;
}


void SymaTX::set_channel(void) {
    byte tx0 = addr[0];
    byte start1[4] = {0x0a, 0x1a, 0x2a, 0x3a};
    byte start2[4] = {0x2a, 0x0a, 0x42, 0x22};
    byte start3[4] = {0x1a, 0x3a, 0x12, 0x32};
    uint32_t *pchans = (u32 *)channels;

    current_channel = 0;
    channel_counter = 0;

    tx0 &= 0x1f;
    if(tx0 < 0x10) {
        if(tx0 == 0x06) tx0 = 0x07;
        for(int i=0; i<4; i++) {
            channels[i] = start1[i] + tx0;
        }
    } else if(tx0 < 0x18) {
        for(int i=0; i<4; i++) {
            channels[i] = start2[i] + (tx0 & 0x07);
        }
        if(tx0 == 0x16) {
            channels[0] += 0x01;
            channels[1] += 0x01;
        }
    } else if(tx0 < 0x1e) {
        for(int i=0; i<4; i++) {
            channels[i] = start3[i] + (tx0 & 0x07);
        }
    } else if(tx0 == 0x1e) {
        *pchans = 0x38184121;
    } else {
        *pchans = 0x39194121;
    }

    return;
}


void SymaTX::build_packet(void) {
    data[0] = direction[0]; 
    data[1] = direction[1]; 
    data[2] = direction[2]; 
    data[3] = direction[3]; 
    data[4] = 0x00;
    data[5] = (data[1] >> 2) | 0xc0;
    data[6] = (data[2] >> 2);
    data[7] = (data[3] >> 2);
    data[8] = 0x00;
    data[9] = checksum(data);

    return;
}


void SymaTX::transmit(RF24 &radio) {
    radio.setChannel(channels[current_channel]);

//    for (int i=0; i<10; i++) {
//      if (data[i] != 0x0a) {
//        Serial.write(data[i]);
//      } else {
//        Serial.write(0x0b);
//      }
//    }
//    Serial.write("\n");

    radio.write(&data, 10, false);

    channel_counter++;
    if (channel_counter % 2 == 0) {
        current_channel++;
        current_channel = current_channel % 4;
        channel_counter = 0;
    }

    return;
}


void SymaTX::transmit(byte *packet, RF24 &radio) {
//    radio.setChannel(channels[current_channel]);

    memcpy(data, packet, 10);
    data[9] = checksum(data);

//    for (int i=0; i<10; i++) {
//      if (data[i] != 0x0a) {
//        Serial.write(data[i]);
//      } else {
//        Serial.write(0x0b);
//      }
//    }
//    Serial.write("\n");

    for (int i=0; i<4; i++) {
      radio.setChannel(channels[i]);
      radio.write(&data, 10, false);
    }
    

//    channel_counter++;
//    if (channel_counter % 2 == 0) {
//        current_channel++;
//        current_channel = current_channel % 4;
//        channel_counter = 0;
//    }

    return;
}


byte SymaTX::checksum(byte *packet) {
    byte sum;
    sum = packet[0];
    for (int i=1; i<9; i++) {
        sum ^= packet[i];
    }
    sum += 0x55;

    return sum;
}


