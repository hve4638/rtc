CC=gcc
CFLAGS=-g -Wall
OBJS=main.o rtc.o printbuffer.o systime.o rtc-sysfs.o
TARGET=rtc

all: $(TARGET)
$(TARGET): $(OBJS)

main.o: main.c rtc.h printbuffer.h
rtc.o: rtc.c rtc.h
printbuffer.o: printbuffer.c printbuffer.h
systime.o: systime.c systime.h
rtc-sysfs.o: rtc-sysfs.c rtc-sysfs.h

clean:
	rm -rf *.o
	rm rtc