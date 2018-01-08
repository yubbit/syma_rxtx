#include "symarx.h"

SymaRX::SymaRX() {
    bound = false;

    channels[0] = 0x4b;
    channels[1] = 0x30;
    channels[2] = 0x40;
    channels[3] = 0x20;
    current_channel = 0;

    addr[0] = 0xab;
    addr[1] = 0xac;
    addr[2] = 0xad;
    addr[3] = 0xae;
    addr[4] = 0xaf;

    available = false;

    return;
}

void SymaRX::init(RF24 &radio) {
    radio.setChannel(channels[0]);
    radio.openReadingPipe(1, addr);
    radio.flush_rx();
    radio.startListening();
    prev_rx_time = millis();

    return;
}


void SymaRX::run(RF24 &radio) {
    long curr_rx_time = millis();
    radio.setChannel(channels[current_channel]);
    radio.openReadingPipe(1, addr);
    radio.startListening();

    if (!radio.available()) {
        if (curr_rx_time - prev_rx_time > 16) {
            current_channel++;
            current_channel %= 4;
            radio.setChannel(channels[current_channel]);
            prev_rx_time = curr_rx_time;
        }
    } else {
        radio.read(&data, 10);
//        Serial.print(current_channel);
//        Serial.print("\n");
        if (!bound) {
            if (checksum(data) == data[9] && 
                data[5] == 0xaa &&
                data[6] == 0xaa) {
                for (int i=0; i<5; i++) {
                    addr[i] = data[4-i];
                }
                set_channel();
                radio.setChannel(channels[current_channel]);
                radio.openReadingPipe(1, addr);
                bound = true;
            }
        }
        available = true;
    }

    return;
}


void SymaRX::read(byte *packet) {
    for (int i=0; i<10; i++) {
        packet[i] = data[i];
    }
    available = false;

    return;
}


byte SymaRX::checksum(byte *packet) {
    byte sum;
    sum = packet[0];
    for (int i=1; i<9; i++) {
        sum ^= packet[i];
    }
    sum += 0x55;

    return sum;
}


void SymaRX::set_channel(void) {
    byte tx0 = addr[0];
    byte start1[4] = {0x0a, 0x1a, 0x2a, 0x3a};
    byte start2[4] = {0x2a, 0x0a, 0x42, 0x22};
    byte start3[4] = {0x1a, 0x3a, 0x12, 0x32};
    uint32_t *pchans = (u32 *)channels;

    current_channel = 0;

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

