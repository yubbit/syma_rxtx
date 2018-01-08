#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "symarx.h"

RF24 radio(7, 8);
SymaRX rx;
byte packet[10];
byte addr[] = {0x80, 0x80, 0x80, 0x80, 0xa2};
int8_t temp;
byte temp2;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(false);
  radio.setAddressWidth(5);
  radio.setRetries(15, 15);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setPayloadSize(10);
  delay(12);

  rx.init(radio);
}

void loop() {
  rx.run(radio);
  if (rx.available) {
    rx.read(packet);
    for (int i=0; i<10; i++) {
      Serial.write(packet[i]);
    }
    Serial.write("\n");
  }

}



