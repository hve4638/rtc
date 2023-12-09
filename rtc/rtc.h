#include <linux/rtc.h>
#include <time.h>

void rtc_open();
void rtc_open_rw();
void rtc_open_ro();
void rtc_close();
void rtc_readtime(struct tm*);
void rtc_writetime(struct tm*);
unsigned long rtc_readepoch();
void rtc_uie_on();
void rtc_uie_off();
void rtc_alarm_set(struct tm *time);
void rtc_alarm_read(struct tm *time);
void rtc_alarm_on();
void rtc_alarm_off();
void rtc_wkalarm_set(struct rtc_wkalrm *);
void rtc_wkalarm_read(struct rtc_wkalrm *);

