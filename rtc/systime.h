#include <time.h>
#include <linux/rtc.h>

void sys_gettime(struct tm* time);
void sys_settime(struct tm* time);