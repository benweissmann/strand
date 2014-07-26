
CFLAGS += -std=c99

include /usr/share/arduino/Arduino.mk

# sometimes this seems to work when regular upload gets stuck
alt-upload:
	avrdude -p atmega328p -c arduino -P /dev/ttyUSB0 \
	    -U flash:w:build-atmega328/lights2.hex:i
