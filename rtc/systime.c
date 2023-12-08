#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

void sys_gettime(struct tm* tinfo) {
    struct timeval now;
    struct tm *nowtm;

    gettimeofday(&now, NULL);
    nowtm = localtime(&now.tv_sec);
    *tinfo = *nowtm;
}

void sys_settime(struct tm* tinfo) {
    struct timeval settv = {};
    time_t sec;
    
    sec = mktime(tinfo);
    settv.tv_sec = sec;

    if (settimeofday(&settv ,NULL) < 0) {
        fprintf(stderr, "Permission Denied\n");
        exit(-1);
    }
}