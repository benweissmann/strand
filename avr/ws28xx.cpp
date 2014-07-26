
#include <Arduino.h>

#ifdef USE_SPI

void begin() {
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV128);
}

void end() {
  SPI.end();
}

void sendByte(char b) {
  SPI.transfer(b);
}

#else /* SPI */

#define CLK 11 /* white */
#define DAT 12 /* green */

#ifndef CLK
#error CLK not defined
#endif

#ifndef DAT
#error DAT not defined
#endif

void begin() {
  pinMode(CLK, OUTPUT);
  pinMode(DAT, OUTPUT);
  digitalWrite(CLK, LOW);
  digitalWrite(DAT, LOW);
  delayMicroseconds(1000);
  //delay(100);
}

void end() {}

void sendBit(int b) {
    digitalWrite(DAT, b ? HIGH : LOW);
    digitalWrite(CLK, HIGH);
    digitalWrite(CLK, LOW);
}

void sendByte(char b) {
  for (int i = 0; i < 8; i++) {
    sendBit(b & 0x80);
    b = b << 1;
  }
}

#endif /* SPI */

void colour(unsigned char r, unsigned char g, unsigned char b) {
  sendByte(r);
  sendByte(g);
  sendByte(b);
}

