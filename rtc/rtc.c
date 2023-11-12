#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "rtc.h"

static int rtc_fd = -1;

static void ioctl_assert(int fd, unsigned long request, void* rtc) {
    int rval = ioctl(fd, request, rtc);
    if (rval < 0) {
        fprintf(stderr, "Assertion Fail (%d)\n", rval);
        exit(-1);
    }
}

void rtc_open() {
    rtc_fd = open("/dev/rtc", O_RDWR);
    if (rtc_fd < 0) {
        fprintf(stderr, "Could not open /dev/rtc\n");
        exit(-1);
    }
}

void rtc_close() {
    if (rtc_fd != -1) {
        close(rtc_fd);
        rtc_fd = -1;
    }
}

void rtc_readtime(struct rtc_time *time) {
    ioctl_assert(rtc_fd, RTC_RD_TIME, time);    
}
void rtc_writetime(struct rtc_time *time) {
    ioctl_assert(rtc_fd, RTC_SET_TIME, time);    
}