
HEADERS = \
    common.h
SOURCES = \
    common.cpp \
    pattern.cpp

TARGETS = strand.so
OBJECTS = $(SOURCES:.cpp=.o)
CFLAGS = -Wall
LDFLAGS = -lm

all: avr $(TARGETS)

clean:
	$(MAKE) -f Makefile.avr clean
	rm -f $(OBJECTS) $(TARGETS)

avr:
	$(MAKE) -f Makefile.avr

upload:
	$(MAKE) -f Makefile.avr upload

strand.so: $(HEADERS) $(OBJECTS)
	$(CC) -shared $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

.PHONY: avr upload
