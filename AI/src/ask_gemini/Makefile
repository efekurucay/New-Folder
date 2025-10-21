CC = cc
CFLAGS = -O -D_MINIX -D_POSIX_C_SOURCE=1
LDFLAGS = -lsocket

SOURCES = ask_gemini.c net.c
HEADERS = net.h
OBJECTS = ask_gemini.o net.o
TARGET = ask_gemini

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

