CC = cc
CFLAGS = -O -D_MINIX -D_POSIX_C_SOURCE=1
LDFLAGS =

SOURCES = eliza.c
HEADERS = script.h
OBJECTS = eliza.o
TARGET = eliza

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all install clean

