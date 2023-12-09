#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "rtc-sysfs.h"

static const char* rtcnames[] = {
    "/sys/class/rtc/rtc0",
    "/sys/class/rtc/rtc1",
    "/sys/class/rtc/rtc2",
};

static int rtc_index = -1;
static int rtc_length = 0;

static char filename[256] = { 0, };
static char* suffix_ptr = NULL;

static char buffer[512] = { 0, };

static FILE* openforce(const char* name, const char* option) {
    int i = 0;
    while (name[i] != '\0') {
        suffix_ptr[i] = name[i];
        i++;
    }
    suffix_ptr[i] = '\0';

    FILE* file = fopen(filename, option);
    if (file == NULL) {
        fprintf(stderr, "couldn't open rtc (%s)\n", name);
        exit(-1);
    }

    return file;
}

static int readint(char* buf, int length) {
    int result;
    char tmp = buf[length];
    buf[length] = '\0';

    result = atoi(buf);

    buf[length] = tmp;
    return result;
}

void rtcinit() {
    DIR *dir;

    rtc_index = -1;
    size_t length = sizeof(rtcnames)/sizeof(rtcnames[0]);
    for (int i = 0; i < length; i++) {
        dir = opendir(rtcnames[i]);

        if (dir != NULL) {
            rtc_index = i;
            closedir(dir);
            break;
        }
    }
    
    if (rtc_index == -1) {
        fprintf(stderr, "error: couldn't find rtc");
        exit(-1);
    } else {
        rtc_length = 0;
        const char* p = rtcnames[rtc_index];
        while (*p != '\0') {
            p++;
            rtc_length++;
        }

        memcpy(filename, rtcnames[rtc_index], rtc_length);
        suffix_ptr = filename + rtc_length;
    }
}

struct rtcdatetime currentdate() {
    struct rtcdatetime dt = { 0, };

    // "/sys/class/rtc/rtc0/date" 경로의 파일을 읽는다
    FILE* file = openforce("/date", "r");

    // YYYY-MM-DD 형식으로 날짜를 가져온다
    fscanf(file, "%s", buffer);
    
    dt.year = readint(buffer, 4);
    dt.month = readint(buffer+5, 2);
    dt.day = readint(buffer+8, 2);

    fclose(file);
    
    // HH:mm:SS 형식으로 시간을 가져온다
    file = openforce("/time", "r");
    fscanf(file, "%s", buffer);

    dt.hours = readint(buffer, 2);
    dt.minutes = readint(buffer+3, 2);
    dt.seconds = readint(buffer+6, 2);

    fclose(file);

    return dt;
}