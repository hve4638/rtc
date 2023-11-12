#include <linux/rtc.h>

void rtc_open();
void rtc_close();
void rtc_readtime(struct rtc_time*);
void rtc_writetime(struct rtc_time*);