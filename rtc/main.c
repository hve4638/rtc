#include <stdio.h>
#include "rtc.h"
#include "systime.h"
#include "printbuffer.h"

void printtime(struct tm* time, const char* message);

int main() {
    struct tm systime = {};
    struct tm rtctime = {};

    sys_gettime(&systime);
    printtime(&systime, "SYS Time");

    rtc_open();

    rtc_readtime(&rtctime);
    printtime(&rtctime, "RTC Time");

    rtc_close();
    return 0;
}

void printtime(struct tm* time, const char* message) {
    printf("%s: %04d.%02d.%02d %02d:%02d:%02d\n",
        message,
        1900 + time->tm_year,
        1 + time->tm_mon,
        time->tm_mday,
        time->tm_hour,
        time->tm_min,
        time->tm_sec
    );
}

