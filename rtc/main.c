#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "rtc.h"
#include "systime.h"
#include "printbuffer.h"

void catch_args(int argc, char *const* argv);
void printtime(struct tm* time, const char* message);

static int noflag = 1;

void handle_int(int signum) {
    printf("interrupt handle?\n");
}


void setalarm() {
    struct tm tm = { 0, };
    tm.tm_sec = 5;

    printf("alarm on\n");
    rtc_uie_on();
    // printf("alarm set\n");
    // rtc_alarm_set(&tm);
    //struct rtc_wkalrm wk = { 0, };

    //wk.enabled = 1;
    //wk.time.tm_sec = 3;
    //rtc_wkalarm_set(&wk);
}

void setinthandle() {
    struct sigaction sa;
    sa.sa_handler = &handle_int;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        return;
    }

}

void test() {
    rtc_open();
    printf("rtc open\n");

    // rtc_alarm_on();

    setalarm();
    
    setinthandle();

    printf("waiting...\n");
    while (1) {
        sleep(1);
    }

    // rtc_alarm_off();
    
    rtc_close();
    printf("rtc close\n");
}

int main(int argc, char *const* argv) {
    test();
    return 0;

    struct tm systime = {};
    struct tm rtctime = {};
    
    catch_args(argc, argv);

    printf("noflag : %d\n", noflag);

    return 0;
    sys_gettime(&systime);
    printtime(&systime, "SYS Time");

    rtc_open();

    rtc_readtime(&rtctime);
    printtime(&rtctime, "RTC Time");

    rtc_close();
    return 0;
}

void catch_args(int argc, char *const* argv) {
    int c;
    while( (c = getopt(argc, argv, "r")) != -1) {
        switch(c) {
            case 'r':
                noflag = 0;
                break;
            case '?':
                fprintf(stderr, "Unknown flag : %c", optopt);
                break;
        }
    }
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

