#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "rtc.h"

static const char* rtcnames[] = {
    "/dev/rtc",
    "/dev/rtc0",
    "/dev/rtc1",
    "/dev/rtc2",
};

static int rtc_fd = -1;

static void ioctl_assert(int fd, unsigned long request, void* rtc) {
    int rval = ioctl(fd, request, rtc);
    if (rval < 0) {
        fprintf(stderr, "Assertion Fail (%d)\n", rval);
        exit(-1);
    }
}

static void rtc_open_flag(int oflag);

void rtc_open() {
    rtc_open_flag(O_RDWR);
}

void rtc_open_rw() {
    rtc_open_flag(O_RDWR);
}

void rtc_open_ro() {
    rtc_open_flag(O_RDONLY);
}

void rtc_open_flag(int oflag) {
    size_t length = sizeof(rtcnames)/sizeof(rtcnames[0]);

    rtc_fd = -1;
    for (int i = 0; i < length; i++) {
        const char *fname = rtcnames[i];
        
        if (access(fname, F_OK) == 0) {
            rtc_fd = open(fname, oflag);

            if (rtc_fd >= 0) {
                break;
            }
        }
    }

    if (rtc_fd < 0) {
        fprintf(stderr, "Could not open rtc\n");
        exit(-1);
    }
}

void rtc_close() {
    if (rtc_fd != -1) {
        close(rtc_fd);
        rtc_fd = -1;
    }
}

void rtc_readtime(struct tm *time) {
    ioctl_assert(rtc_fd, RTC_RD_TIME, time);
}
void rtc_writetime(struct tm *time) {
    ioctl_assert(rtc_fd, RTC_SET_TIME, time);    
}
unsigned long rtc_readepoch() {
    unsigned long* epoch = 0;
    ioctl_assert(rtc_fd, RTC_EPOCH_READ, epoch);
    return *epoch;
}
void rtc_uie_on() {
    ioctl_assert(rtc_fd, RTC_UIE_ON, 0);
}
void rtc_uie_off() {
    ioctl_assert(rtc_fd, RTC_UIE_OFF, 0);
}

void rtc_alarm_set(struct tm *time) {
    ioctl_assert(rtc_fd, RTC_ALM_SET, time);
}
void rtc_alarm_read(struct tm *time) {
    ioctl_assert(rtc_fd, RTC_ALM_READ, time);
}
void rtc_alarm_on() {
    ioctl_assert(rtc_fd, RTC_AIE_ON, 0);
}
void rtc_alarm_off() {
    ioctl_assert(rtc_fd, RTC_AIE_OFF, 0);
}
void rtc_wkalarm_set(struct rtc_wkalrm *wk) {
    ioctl_assert(rtc_fd, RTC_WKALM_SET, wk);
}
void rtc_wkalarm_read(struct rtc_wkalrm *wk) {
    ioctl_assert(rtc_fd, RTC_WKALM_RD, wk);
}



