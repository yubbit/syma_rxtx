Utilities to control a Syma drone using an Arduino

Usage
---
Connect an nRF24L01 unit to the Arduino.

Run generate_sketch.sh in each folder to place the appropriate files in the Arduino sketch folder and
run the sketches.

The listener.ino sketch in the rx folder behaves like a Syma drone. Simply turn on the listener and
bind the controller. This transmits the data packets sent to the drone through the Arduino's serial
port.

The transmitter.ino ketch in the tx folder behaves like the Syma controller. It reads commands from
the Arduino's serial port and transmits them to the drone. To begin binding, push the throttle
forward, or equivalently send a valid transmission where the first byte of the packet is 0xFF.

Libraries
---
By default, the libraries need to be run on 2 separate Arduinos, and the serial ports should be
managed by a Python script. However, sketches can be designed to run both aspects on a single Arduino.

These sketches make use of the [RF24 library](http://tmrh20.github.io/RF24/). Details on how to
connect the nRF24L01 unit to the Arduino can be found in the library's documentation.

Summary
---
The binding and transmission process of Syma can be summarized as follows:
1. The drone awaits a standard binding signal
2. When received, it awaits a valid packet containing the transmission ID, contained within the first 5 bytes of the packet
3. It begins listening to a set of 4 channels, which are determined by the first byte of the transmission ID

Each byte in the packet corresponds to the following:
1. Throttle
2. Pitch
3. Yaw
4. Roll
5. H/L Mode and Tilt corrections (Needs further testing)
6. Checksum value (And another value, also needs further testing)
7. Checksum value and camera
8. Checksum value and flip
9. Empty packet
10. Checksum value based on previous 9 packets

