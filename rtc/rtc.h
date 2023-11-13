#include <linux/rtc.h>
#include <time.h>

void rtc_open();
void rtc_close();
void rtc_readtime(struct tm*);
void rtc_writetime(struct tm*);