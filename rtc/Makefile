CC=gcc
CFLAGS=-g -Wall
OBJS=main.o rtc.o printbuffer.o
TARGET=rtc

all: $(TARGET)
$(TARGET): $(OBJS)

main.o: main.c rtc.h printbuffer.h
rtc.o: rtc.c rtc.h
printbuffer.o: printbuffer.c printbuffer.h
