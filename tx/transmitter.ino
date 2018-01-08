#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "symatx.h"

RF24 radio(7, 8);
SymaTX tx;
byte packet[11];
byte addr[] = {0x80, 0x80, 0x80, 0x80, 0xa2};
int8_t temp;
byte temp2;

void setup() {
  Serial.begin(19200);
  radio.begin();
  radio.setAutoAck(false);
  radio.setAddressWidth(5);
  radio.setRetries(15, 15);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setPayloadSize(10);
  delay(12);

}

void loop() {
  if (Serial.available()) {
    Serial.readBytes(packet, 11);
    if (packet[0] == 0xff && !tx.bound) {
      tx.init(addr, radio);
    }
    if (tx.bound) {
//      for (int i=0; i<4; i++) {
//        tx.direction[i] = packet[i];        
//      }
//      tx.build_packet();
      tx.transmit(packet, radio);
    } else {
      Serial.write("\n");
    }
  }

}



