#ifndef __RTC_H
#define __RTC_H
 
#include <time.h>
extern uint16_t MyRTC_Time[];

void RTC_Init(void);
void RTC_SET(void);
void RTC_GET(void);
void RTC_Internet_SET(time_t time_cnt);

#endif
