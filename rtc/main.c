#include <stdio.h>
#include "rtc.h"
#include "printbuffer.h"

extern void printbuffer(void* buffer, int length, int interval);

void resettime(struct rtc_time* time);
void printrtc(struct rtc_time time);

int main() {
    struct rtc_time time = {};

    resettime(&time);

    rtc_open();
    // rtc_readtime(&time);
    rtc_writetime(&time);

    printrtc(time);

    rtc_close();

    return 0;
}

void resettime(struct rtc_time* time) {
    time->tm_sec = 0;
    time->tm_min = 15;
    time->tm_hour = 13;
    time->tm_mday = 15;
    time->tm_mon = 5;
    time->tm_year = 2020;
	time->tm_wday = 0;
	time->tm_yday = 0;
	time->tm_isdst = 0;
}

void printrtc(struct rtc_time rtc) {
    printf("RTC time: %04d.%02d.%02d %02d:%02d:%02d\n",
        1900 + rtc.tm_year, 1 + rtc.tm_mon, rtc.tm_mday,
        rtc.tm_hour, rtc.tm_min, rtc.tm_sec);
}
