#include "stm32f10x.h"                  // Device header
#include <time.h>

uint16_t MyRTC_Time[]= {2025, 7, 5, 23, 59, 48};		//设置目前的时间
struct tm *local_time;
int TimeDisplay=0;

void RTC_SET()
{
	  time_t time_cnt;
    struct tm time_date;
		//time_cnt = time(NULL);
			
    // 将预设的时间信息转换为秒数
    time_date.tm_year = MyRTC_Time[0] - 1900;
    time_date.tm_mon = MyRTC_Time[1] - 1;
    time_date.tm_mday = MyRTC_Time[2];
    time_date.tm_hour = MyRTC_Time[3];
    time_date.tm_min = MyRTC_Time[4];
    time_date.tm_sec = MyRTC_Time[5];

    // mktime函数将tm结构体转换为秒数，减去8小时是为了调整时差
   time_cnt = mktime(&time_date) - 8 * 60 * 60;

    // 设置RTC的计数器
    RTC_SetCounter(time_cnt);
    RTC_WaitForLastTask();
}
void RTC_Internet_SET(time_t time_cnt)
{
		//time_cnt = time(NULL);
			
//    // 将预设的时间信息转换为秒数
//    time_date.tm_year = MyRTC_Time[0] - 1900;
//    time_date.tm_mon = MyRTC_Time[1] - 1;
//    time_date.tm_mday = MyRTC_Time[2];
//    time_date.tm_hour = MyRTC_Time[3];
//    time_date.tm_min = MyRTC_Time[4];
//    time_date.tm_sec = MyRTC_Time[5];

    // mktime函数将tm结构体转换为秒数，减去8小时是为了调整时差
   time_cnt = time_cnt;

    // 设置RTC的计数器
    RTC_SetCounter(time_cnt);
    RTC_WaitForLastTask();
}
void RTC_Configuration()
{
	//打开备份时钟和解除保护时钟
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_BKP, ENABLE);
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_PWR , ENABLE);
	//使能解除保护
	PWR_BackupAccessCmd(ENABLE);
	//复位备份域
	//BKP_DeInit();
	
	//启用低速外部时钟，不用内部是因为外部的比较精准
	RCC_LSICmd(ENABLE);
	//等待低速外部时钟准备就绪
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) !=SET);
	//使用低速外部时钟作为RTC的时钟源
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	//使能RTC外设
	RCC_RTCCLKCmd(ENABLE);
	//等待同步结束,开启后需要等待APB1时钟与RTC时钟同步，才能读写寄存器
	RTC_WaitForSynchro();
	//读写寄存器前，要确定上一个操作已经结束
	RTC_WaitForLastTask();
	//选择中断源
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	//读写寄存器前，要确定上一个操作已经结束
	RTC_WaitForLastTask();
	//设置分频值，因为低速外部时钟频率为32.768KHZ，所以除32767+1后得1HZ
	RTC_SetPrescaler(40000-1); 
	//读写寄存器前，要确定上一个操作已经结束
	RTC_WaitForLastTask();
				RTC_SET();
      //RTC_SetCounter(1672588795);
	RTC_WaitForLastTask();
}
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
void RTC_GET()
{
  time_t time_cnt;
    struct tm time_date;

    // 获取RTC的计数器值
    time_cnt = RTC_GetCounter() + 8 * 60 * 60;

    // 将秒数转换为tm结构体
    time_date = *localtime(&time_cnt);

    // 更新MyRTC_Time数组的元素
    MyRTC_Time[0] = time_date.tm_year + 1900;
    MyRTC_Time[1] = time_date.tm_mon + 1;
    MyRTC_Time[2] = time_date.tm_mday;
    MyRTC_Time[3] = time_date.tm_hour;
    MyRTC_Time[4] = time_date.tm_min;
    MyRTC_Time[5] = time_date.tm_sec;
}

 void RTC_IRQHandler(void)
 {
   if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
   {
     /* Clear the RTC Second interrupt */
     RTC_ClearITPendingBit(RTC_IT_SEC);
      
     /* Enable time update */
     TimeDisplay = 1;
 
     /* Wait until last write operation on RTC registers has finished */
     RTC_WaitForLastTask();
     
   }
 }

 void RTC_Init()
 {  
    RTC_Configuration();
    RTC_Configuration();
	NVIC_Configuration();
 }
