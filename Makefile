CFLAGS ?= -c -Wall
LDFLAGS ?=

SOURCES = main.c ewmh.c
OBJECTS = $(SOURCES:.c=.o)

TARGET = ewmhgestures

CFLAGS += `pkg-config --cflags libinput`
LDFLAGS += `pkg-config --libs libinput`
CFLAGS += `pkg-config --cflags udev`
LDFLAGS += `pkg-config --libs udev` -ludev
CFLAGS += `pkg-config --cflags x11`
LDFLAGS += `pkg-config --libs x11`

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(TARGET)
