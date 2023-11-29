#pragma once
#include <linux/rtc.h>

struct rtcdatetime {
    int year;
    int month;
    int day;
    int hours;
    int minutes;
    int seconds;
};

void rtcinit();
void rtcfree();

struct rtcdatetime currentdate();
