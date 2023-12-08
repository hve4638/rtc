#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "rtc.h"
#include "systime.h"
#include "printbuffer.h"
#define FLAG_ARGERROR   0x1
#define FLAG_HELP       0x2
#define FLAG_LOAD       0x4
#define FLAG_STORE      0x8
#define FLAG_TEST      0x10

int is_sudo() {
    return geteuid() == 0 ? 1 : 0;
}

int catch_args(int argc, char *const* argv);
void show_clock();
void printtime(struct tm* time);
void show_help();
void test();

void load_systime_from_rtc();
void store_systime_to_rtc();

int main(int argc, char *const* argv) {
    int flags = catch_args(argc, argv);

    if (flags & FLAG_ARGERROR) {
        exit(-1);
    }
    else if (flags & FLAG_HELP) {
        show_help();
    }
    else if (flags & FLAG_LOAD) {
        load_systime_from_rtc();
    }
    else if (flags & FLAG_STORE) {
        store_systime_to_rtc();
    }
    else {
        show_clock();
    }

    return 0;
}

void test() {
    struct tm time = {};

    sys_gettime(&time);
    time.tm_hour += 1;
    sys_settime(&time);
}

int catch_args(int argc, char *const* argv) {
    int flags = 0x0;
    
    int c;
    while( (c = getopt(argc, argv, "hls")) != -1) {
        switch(c) {
            case 't':
                flags |= FLAG_TEST;
                break;
            case 'h':
                flags |= FLAG_HELP;
                break;
            case 's':
                flags |= FLAG_STORE;
                break;
            case 'l':
                flags |= FLAG_LOAD;
                break;
            case '?':
                flags |= FLAG_ARGERROR;
                fprintf(stderr, "Unknown flag : %c\n", optopt);
                break;
        }
    }
    return flags;
}

void show_help() {
    printf("Usage: rtc [arguments]\n");
    printf("\n");
    printf("Arguments:\n");
    printf("  -h\t\tPrint help\n");
    printf("  -l\t\tLoad System Time from RTC\n");
    printf("  -s\t\tStore System Time to RTC\n");
}

void show_clock() {
    struct tm systime = {};
    struct tm rtctime = {};

    sys_gettime(&systime);
    printf("System Time\t");
    printtime(&systime);

    if (!is_sudo()) {
        fprintf(stderr, "rtc: Cannot access the Hardware Clock\n");
    }
    else {
        rtc_open();
        rtc_readtime(&rtctime);
        printf("RTC Time\t");
        printtime(&rtctime);
        rtc_close();
    }
}

void printtime(struct tm* time) {
    printf("%04d-%02d-%02d %02d:%02d:%02d\n",
        1900 + time->tm_year,
        1 + time->tm_mon,
        time->tm_mday,
        time->tm_hour,
        time->tm_min,
        time->tm_sec
    );
}

void load_systime_from_rtc() {
    struct tm time = {};

    if (!is_sudo()) {
        fprintf(stderr, "rtc: Cannot access the Hardware Clock\n");
        exit(1);
    }

    rtc_open();
    rtc_readtime(&time);
    rtc_close();

    sys_settime(&time);
}

void store_systime_to_rtc() {
    struct tm time = {};

    if (!is_sudo()) {
        fprintf(stderr, "rtc: Cannot access the Hardware Clock\n");
        exit(1);
    }

    sys_gettime(&time);

    rtc_open();
    rtc_writetime(&time);
    rtc_close();
}