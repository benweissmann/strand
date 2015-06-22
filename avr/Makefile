
HEADERS = \
    common.h
SOURCES = \
    common.cpp \
    pattern.cpp

TARGETS = strand.so
OBJECTS = $(SOURCES:.cpp=.o)
CFLAGS = -Wall
LDFLAGS = -lm

all: $(TARGETS)

clean:
	rm -f $(OBJECTS) $(TARGETS)

strand.so: $(HEADERS) $(OBJECTS)
	$(CC) -shared $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

