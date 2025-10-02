#include "lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
#include "lcd.h"
#include "lcd_init.h"
#include "bsp_i2c_gpio.h"
#include "CST816.h"
#include "max30102.h"
#include "Timer.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "RTC.h"
#include "usart.h"
#include "max30102.h"
#include <stdlib.h>
#include "step.h"
#include "mpu6050.h"
#include "esp8266.h"

//#include "max30102.h"

  extern unsigned char revbuf[512];
  extern int revint;
	
void clocksetting_screen(void);
void clock_screen(void);
void menu_screen(void);
void WiFi_screen(void);
void Getweather_screen(void);

lv_obj_t * label_HeartRate_Spo2;
lv_obj_t * label_Stepnum;
lv_obj_t * weather;

extern u8 dis_hr;
extern u8	dis_spo2;

extern uint16_t step_count;
extern uint16_t stepnum;

uint16_t now_day=0;
uint16_t record_day=0;

TaskHandle_t ESP8266initTask_Handler;         /* 任务句柄 */
void ESP8266initTask(void *pvParameters);    /* 任务函数 */
BaseType_t ESP8266initTaskReturned;					/* 创建任务的返回值 */

TaskHandle_t ConnectToWeatherTask_Handler;         /* 任务句柄 */
void ConnectToWeatherTask(void *pvParameters);    /* 任务函数 */
BaseType_t ConnectToWeatherTaskReturned;					/* 创建任务的返回值 */

TaskHandle_t ConnectToTimeTask_Handler;         /* 任务句柄 */
void ConnectToTimeTask(void *pvParameters);    /* 任务函数 */
BaseType_t ConnectToTimeTaskReturned;					/* 创建任务的返回值 */

static void event_handler1(lv_event_t * e)
{			
		 lv_obj_t * btn = lv_event_get_target(e);
		 lv_obj_t * screen=lv_obj_get_parent(btn);
		 lv_obj_del(screen);
		 menu_screen();
}
static void event_handler2(lv_event_t * e)
{			
		 lv_obj_t * btn = lv_event_get_target(e);
		 lv_obj_t * screen=lv_obj_get_parent(btn);			
		 lv_obj_del(screen);
		 menu_screen();
}

 void time_refresh(lv_event_t* event)
{
	RTC_GET();
	now_day=MyRTC_Time[2];
	lv_obj_t * label_time=lv_event_get_target(event);	
	lv_label_set_text_fmt(label_time, "%2d:%2d:%2d\r\n    %d", MyRTC_Time[3],MyRTC_Time[4],MyRTC_Time[5],MyRTC_Time[0]);
	lv_label_set_text_fmt(label_HeartRate_Spo2, "HeartRate:%d\nSpo2:%d",dis_hr,dis_spo2);
	lv_label_set_text_fmt(label_Stepnum, "StepNum:%d",stepnum);
}
static void event_send(lv_timer_t * timer)
{
	lv_obj_t * label_time=(lv_obj_t *)timer->user_data;
	lv_event_send(label_time, LV_EVENT_REFRESH,NULL);
	//lv_event_send(label_HeartRate_Spo2, LV_EVENT_REFRESH,NULL);
}
static void event_handler_button_Clockshow(lv_event_t* event)
{
		 lv_obj_t * btn = lv_event_get_target(event);
		 lv_obj_t * screen=lv_obj_get_parent(btn);
		 lv_obj_del(screen);
		 clock_screen();
}
static void event_handler_btn_clockset(lv_event_t* event)
{
		 lv_obj_t * btn = lv_event_get_target(event);
		 lv_obj_t * screen=lv_obj_get_parent(btn);
		 lv_obj_del(screen);
		 clocksetting_screen();
}

static void event_handler_button_WiFiSet(lv_event_t* event)
{
		 lv_obj_t * btn = lv_event_get_target(event);
		 lv_obj_t * screen=lv_obj_get_parent(btn);
		 lv_obj_del(screen);
		 WiFi_screen();
}

static void event_handler_WiFi_Back(lv_event_t * e)
{			
		 lv_obj_t * btn = lv_event_get_target(e);
		 lv_obj_t * screen=lv_obj_get_parent(btn);			
		 lv_obj_del(screen);
		 menu_screen();
}

static void event_handler_WiFi_Connect(lv_event_t * e)
{			
		 lv_obj_t * btn = lv_event_get_target(e);
		 vTaskResume(ESP8266initTask_Handler);
}
static void event_handler_aoto_gettime(lv_event_t * e)
{			
		 lv_obj_t * btn = lv_event_get_target(e);
		 vTaskResume(ConnectToTimeTask_Handler);
}
static void event_handler_button_Getweather(lv_event_t* event)
{
		 vTaskResume(ConnectToWeatherTask_Handler);
		 lv_obj_t * btn = lv_event_get_target(event);
		 lv_obj_t * screen=lv_obj_get_parent(btn);
		 lv_obj_del(screen);
		 Getweather_screen();
}

static void roller1_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        int hour = atoi(buf); 
			RTC_GET();
			MyRTC_Time[3]=hour;
			RTC_SET();
    }
}
static void roller2_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        int minute = atoi(buf); 
			RTC_GET();
			MyRTC_Time[4]=minute;
			RTC_SET();
    }
}
static void roller3_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        int second = atoi(buf); 
			RTC_GET();
			MyRTC_Time[5]=second;
			RTC_SET();
    }
}
void clocksetting_screen()
{
		lv_obj_t * screen = lv_obj_create(NULL);
		lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);
	
		lv_obj_t * btn = lv_btn_create(screen);
		lv_obj_t * roller1 = lv_roller_create(screen);
		lv_obj_t * roller2 = lv_roller_create(screen);
		lv_obj_t * roller3 = lv_roller_create(screen);
	 const char * hour = 
        "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n"
        "11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n"
        "21\n22\n23\n24";
	 const char * secend = 
        "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n"
        "11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n"
        "21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n"
        "31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n"
        "41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n"
        "51\n52\n53\n54\n55\n56\n57\n58\n59\n60";
    lv_roller_set_options(roller1,hour ,LV_ROLLER_MODE_NORMAL);
		lv_roller_set_options(roller2,secend ,LV_ROLLER_MODE_NORMAL);
		lv_roller_set_options(roller3,secend ,LV_ROLLER_MODE_NORMAL);
		//lv_obj_set_size(btn,50,50);
		lv_roller_set_visible_row_count(roller1, 4);
	 	lv_obj_t * label1 = lv_label_create(btn);
		lv_label_set_text(label1, "BACK");
	 
		lv_obj_align(roller1, LV_ALIGN_LEFT_MID, 0, 0);
	 
	 	lv_roller_set_visible_row_count(roller2, 4);
		lv_obj_align(roller2, LV_ALIGN_CENTER, 0, 0);
	 
	 	 	lv_roller_set_visible_row_count(roller3, 4);
		lv_obj_align(roller3, LV_ALIGN_RIGHT_MID, 0, 0);
	 
	 
	 
		lv_obj_add_event_cb(btn, event_handler1,LV_EVENT_CLICKED, NULL);
		lv_obj_add_event_cb(roller1, roller1_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
		lv_obj_add_event_cb(roller2, roller2_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
		lv_obj_add_event_cb(roller3, roller3_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
		lv_scr_load(screen);
}
void clock_screen()
{
		lv_obj_t * screen = lv_obj_create(NULL);
		lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);
	
		lv_obj_t * label_time=lv_label_create(screen);
		RTC_GET();
		lv_label_set_text_fmt(label_time, "%2d:%2d:%2d\r\n    %d", MyRTC_Time[3],MyRTC_Time[4],MyRTC_Time[5],MyRTC_Time[0]);
		lv_obj_align(label_time, LV_ALIGN_CENTER, 0, 0);
		lv_obj_set_size(label_time,150,100);
		lv_obj_set_style_text_font(label_time, &lv_font_montserrat_32, 0); // 32px 字体
		lv_obj_align(label_time,LV_ALIGN_CENTER , 0, 0);
		lv_obj_t * btn = lv_btn_create(screen);
		lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
	
		label_HeartRate_Spo2=lv_label_create(screen);
	
		lv_label_set_text_fmt(label_HeartRate_Spo2, "HeartRate:%d\nSpo2:%d",dis_hr,dis_spo2);
		lv_obj_align(label_HeartRate_Spo2,LV_ALIGN_BOTTOM_MID , 0, 0);	
	
		lv_obj_t * label1 = lv_label_create(btn);
		lv_label_set_text(label1, "BACK");
		
		label_Stepnum=lv_label_create(screen);
		lv_label_set_text_fmt(label_Stepnum, "StepNum:%d",stepnum);
		lv_obj_align(label_Stepnum,LV_ALIGN_TOP_RIGHT , 0, 0);
		
		lv_obj_add_event_cb(btn, event_handler2,LV_EVENT_CLICKED, NULL);
		lv_obj_add_event_cb(label_time,time_refresh,LV_EVENT_REFRESH ,NULL);
		//lv_obj_add_event_cb(label_HeartRate_Spo2,HeartRateSpo2_refresh,LV_EVENT_REFRESH ,NULL);
		lv_timer_create(event_send, 1000, label_time);
		lv_scr_load(screen);
}
void menu_screen()
{
		lv_obj_t * screen = lv_obj_create(NULL);
		lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);
	
		lv_obj_t * btn_clockshow = lv_btn_create(screen);
		lv_obj_t * btn_clockset =  lv_btn_create(screen);
		lv_obj_t * btn_wifiget =   lv_btn_create(screen);
		lv_obj_t * btn_getweather =  lv_btn_create(screen);
		lv_obj_t * btn_autoget_time =   lv_btn_create(screen);	
	
		lv_obj_align(btn_clockshow, LV_ALIGN_CENTER, 0, 0);
		lv_obj_align(btn_clockset,LV_ALIGN_LEFT_MID , 0, 0);	
		lv_obj_align(btn_wifiget,LV_ALIGN_RIGHT_MID , 0, 0);
		lv_obj_align(btn_getweather, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
		lv_obj_align(btn_autoget_time, LV_ALIGN_BOTTOM_LEFT, 0, 0);
	
		lv_obj_t * label1 = lv_label_create(btn_clockshow);
		lv_label_set_text(label1, "Clock");
	
		lv_obj_t * label2 = lv_label_create(btn_clockset);
		lv_label_set_text(label2, "TimeSet");
	
		lv_obj_t * label3 = lv_label_create(btn_wifiget);
		lv_label_set_text(label3, "WiFiSet");
		
		lv_obj_t * label4 = lv_label_create(btn_getweather);
		lv_label_set_text(label4, "getweather");
		
		lv_obj_t * label5 = lv_label_create(btn_autoget_time);
		lv_label_set_text(label5, "autoget_time");		
		
		lv_obj_add_event_cb(btn_getweather,event_handler_button_Getweather , LV_EVENT_CLICKED, NULL);
		lv_obj_add_event_cb(btn_autoget_time, event_handler_aoto_gettime, LV_EVENT_CLICKED, NULL);
		lv_obj_add_event_cb(btn_wifiget, event_handler_button_WiFiSet, LV_EVENT_CLICKED, NULL);
		lv_obj_add_event_cb(btn_clockshow, event_handler_button_Clockshow, LV_EVENT_CLICKED, NULL);
		lv_obj_add_event_cb(btn_clockset, event_handler_btn_clockset, LV_EVENT_CLICKED, NULL);
	
		lv_scr_load(screen);
}
static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);
    if(code == LV_EVENT_FOCUSED) {
        lv_keyboard_set_textarea(kb, ta);
    }

    if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);

    }
}
char passwd[32]={0};
char userna[32]={0};
static void getusertext_event_cb(lv_event_t * e)
{
		lv_obj_t * btn = lv_event_get_target(e);
		lv_obj_t * usernametext = lv_event_get_user_data(e);
		const char * text=lv_textarea_get_text(usernametext);
		strcpy(userna,text);
		Usart1_SentString(userna);

}
static void getpawdtext_event_cb(lv_event_t * e)
{
		lv_obj_t * btn = lv_event_get_target(e);
		lv_obj_t * passwardtext = lv_event_get_user_data(e);

		const char * text=lv_textarea_get_text(passwardtext);
		strcpy(passwd,text);
		Usart1_SentString(passwd);

}
void WiFi_screen()
{
		lv_obj_t * screen = lv_obj_create(NULL);
		lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);
	
		lv_obj_t * kb = lv_keyboard_create(screen);
	
		lv_obj_t * usernametext = lv_textarea_create(screen);
		lv_obj_t * passwardtext = lv_textarea_create(screen);
	
		lv_obj_t * btn_back = lv_btn_create(screen);
		lv_obj_t * btn_connectwifi = lv_btn_create(screen);
		lv_obj_t * btn_writedown_username = lv_btn_create(screen);
		lv_obj_t * btn_writedown_password = lv_btn_create(screen);
	
		lv_obj_set_size(btn_back, 60, 25);     //或者使用一个函数  
		lv_obj_set_size(btn_connectwifi, 80, 25);      
		
		lv_obj_t * label1 = lv_label_create(btn_back);	
		lv_obj_t * label2 = lv_label_create(screen);
		lv_obj_t * label3 = lv_label_create(screen);
		lv_obj_t * label4 = lv_label_create(btn_writedown_username);
		lv_obj_t * label5 = lv_label_create(btn_writedown_password);
		lv_obj_t * label6 = lv_label_create(btn_connectwifi);
	
		lv_label_set_text(label1, "Back");
		lv_label_set_text(label2, "username:");
		lv_label_set_text(label3, "passward:");
		lv_label_set_text(label4, "upload");
		lv_label_set_text(label5, "upload");
		lv_label_set_text(label6, "connect");		
		
		lv_obj_align(btn_connectwifi, LV_ALIGN_TOP_RIGHT, -70, 0);
		lv_obj_align(btn_back, LV_ALIGN_TOP_RIGHT, 0, 0);
		lv_obj_align(btn_writedown_username, LV_ALIGN_TOP_RIGHT, 0, 35);
		lv_obj_align(btn_writedown_password, LV_ALIGN_TOP_RIGHT, 0, 85);
	
		lv_textarea_set_one_line(usernametext, true);
		lv_obj_set_width(usernametext, 150);
		lv_textarea_set_one_line(passwardtext, true);
		lv_obj_set_width(passwardtext, 150);
		
	
		lv_obj_align(label2, LV_ALIGN_TOP_LEFT, 0, 19);
		lv_obj_align(label3, LV_ALIGN_TOP_LEFT, 0, 70);
		
		lv_obj_align(usernametext, LV_ALIGN_TOP_LEFT, 0, 35);
		lv_obj_align(passwardtext, LV_ALIGN_TOP_LEFT, 0, 85);
	
		lv_obj_add_event_cb(btn_back, event_handler_WiFi_Back, LV_EVENT_CLICKED, NULL);
		lv_obj_add_event_cb(btn_connectwifi,event_handler_WiFi_Connect , LV_EVENT_CLICKED, NULL);
		lv_obj_add_event_cb(btn_writedown_username,getusertext_event_cb , LV_EVENT_CLICKED, usernametext);
		lv_obj_add_event_cb(btn_writedown_password,getpawdtext_event_cb , LV_EVENT_CLICKED, passwardtext);		
		lv_obj_add_event_cb(usernametext, ta_event_cb, LV_EVENT_ALL, kb);
		lv_obj_add_event_cb(passwardtext, ta_event_cb, LV_EVENT_ALL, kb);
		lv_scr_load(screen);

}
static void event_handler_Getweather_Back(lv_event_t * e)
{			
		 lv_obj_t * btn = lv_event_get_target(e);
		 lv_obj_t * screen=lv_obj_get_parent(btn);			
		 lv_obj_del(screen);
		 menu_screen();
}
static void event_handler_Getweather(lv_event_t * e)
{			
		 
		 lv_label_set_text_fmt(weather, "%s",revbuf);
		 
}

void Getweather_screen()
{
		lv_obj_t * screen = lv_obj_create(NULL);
		lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);
	
		lv_obj_t * btn_getweather = lv_btn_create(screen);
		lv_obj_t * btn_back = lv_btn_create(screen);
		lv_obj_set_size(btn_getweather, 50, 50);
		lv_obj_set_size(btn_back, 50, 50);
	
		lv_obj_t *label1 =lv_label_create(btn_back);
		lv_label_set_text_fmt(label1, "back");
	
		lv_obj_t *label2 =lv_label_create(btn_getweather);
		lv_label_set_text_fmt(label2, "get");
	
		 weather = lv_label_create(screen);
		
		lv_label_set_text_fmt(weather, "NoInternet");
	
		lv_obj_align(btn_getweather, LV_ALIGN_TOP_MID, 0, 0);
		lv_obj_align(btn_back, LV_ALIGN_TOP_RIGHT, 0, 0);
		lv_obj_align(weather, LV_ALIGN_CENTER, 0, 0);
	
		lv_obj_add_event_cb(btn_back,event_handler_Getweather_Back , LV_EVENT_CLICKED, NULL);
		lv_obj_add_event_cb(btn_getweather,event_handler_Getweather , LV_EVENT_CLICKED, NULL);
	
		lv_scr_load(screen);	
}


TaskHandle_t StartTask_Handler;         /* 任务句柄 */
void start_task(void *pvParameters);    /* 任务函数 */
BaseType_t Start_taskReturned;					/* 创建任务的返回值 */

TaskHandle_t lvglTask_Handler;         /* 任务句柄 */
void lvgl_task(void *pvParameters);    /* 任务函数 */
BaseType_t lvgl_taskReturned;					/* 创建任务的返回值 */

TaskHandle_t Heartspo2Task_Handler;         /* 任务句柄 */
void Heartspo2_task(void *pvParameters);    /* 任务函数 */
BaseType_t Heartspo2_taskReturned;					/* 创建任务的返回值 */

TaskHandle_t DetectStepTask_Handler;         /* 任务句柄 */
void DetectStep_task(void *pvParameters);    /* 任务函数 */
BaseType_t DetectStep_taskReturned;					/* 创建任务的返回值 */




void demo()
{	
			//创建初始化驱动的任务，此任务可以再驱动都初始完后删除。
	    Start_taskReturned = xTaskCreate(
                    start_task,       //任务的地址
                    "create task",          //任务的介绍
                    128,      //栈空间的分配 128*32
										NULL,    //传入参数
                    6,			 //设置优先级
                    &StartTask_Handler );      //任务的句柄
	
			vTaskStartScheduler();                              /* 开启任务调度 */	
}

void start_task(void *pvParameters)
{
	//初始化驱动设备
	MAX30102_Init();//MAX30102初始化
	LCD_Init();//LCD初始化
	i2c_CfgGpio();
	CST816_Init();	
	RTC_Init();
	RTC_SET();
	
	RTC_GET();
	record_day=MyRTC_Time[2];
	
  usart1_init(9600);
	usart2_init(115200);
	InitMPU6050();
	//Timer_Init();
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	 taskENTER_CRITICAL();           /* 进入临界区 ,防止被抢占*/
	lvgl_taskReturned = xTaskCreate(
                    lvgl_task,       //任务的地址
                    "lvgl task",          //任务的介绍
                    1024,      //栈空间的分配 1024*32
										NULL,    //传入参数
                    4,			 //设置优先级
                    &lvglTask_Handler );      //任务的句柄
	
	 Heartspo2_taskReturned = xTaskCreate(
                    Heartspo2_task,       //任务的地址
                    "create task",          //任务的介绍
                    128,      //栈空间的分配 128*32
										NULL,    //传入参数
                    1,			 //设置优先级
                    &Heartspo2Task_Handler );      //任务的句柄
										
	 DetectStep_taskReturned = xTaskCreate(
                    DetectStep_task,       //任务的地址
                    "create task",          //任务的介绍
                    128,      //栈空间的分配 128*32
										NULL,    //传入参数
                    3,			 //设置优先级
                    &DetectStepTask_Handler );      //任务的句柄										
																		
	 ESP8266initTaskReturned = xTaskCreate(
                    ESP8266initTask,       //任务的地址
                    "create task",          //任务的介绍
                    128,      //栈空间的分配 128*32
										NULL,    //传入参数
                    5,			 //设置优先级
                    &ESP8266initTask_Handler );      //任务的句柄		

	 ConnectToWeatherTaskReturned = xTaskCreate(
                    ConnectToWeatherTask,       //任务的地址
                    "create task",          //任务的介绍
                    128,      //栈空间的分配 128*32
										NULL,    //传入参数
                    5,			 //设置优先级
                    &ConnectToWeatherTask_Handler );      //任务的句柄
										
		 ConnectToTimeTaskReturned = xTaskCreate(
                    ConnectToTimeTask,       //任务的地址
                    "create task",          //任务的介绍
                    128,      //栈空间的分配 128*32
										NULL,    //传入参数
                    5,			 //设置优先级
                    &ConnectToTimeTask_Handler );      //任务的句柄
										
		 vTaskSuspend(ESP8266initTask_Handler);
		 vTaskSuspend(ConnectToWeatherTask_Handler);
		 vTaskSuspend(ConnectToTimeTask_Handler);
		 
	   taskEXIT_CRITICAL();            /* 退出临界区 */
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
	
}

void lvgl_task(void *pvParameters)
{
	
	
	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();
	
	clock_screen();
	while(1)
	{
		  lv_timer_handler(); /* LVGL计时器 */
			//Usart_SentString("working\r\n");
			vTaskDelay(5);
	}
}

void Heartspo2_task(void *pvParameters)
{
	
	while(1)
	{
		GetHeartRateSpo2();
	}
	
}

void DetectStep_task(void *pvParameters)
{
	static uint8_t step_time_count = 0;
	
	while(1)
	{		
			
			detect_step();
			//Usart_SentString(" step working\r\n");
			step_time_count ++;
			if(step_time_count == 6)   //300ms
			{
				step_time_count = 0;
				if(step_count != 0)
				{
					if(record_day!=now_day)
					{
						record_day=now_day;
						stepnum=0;
					}
					step_count = 0;
					stepnum ++;
				}
			}
			vTaskDelay(50);
	}
}

void ESP8266initTask(void *pvParameters)
{
	char *p;
	while(1)
	{
			esp8266_exitsend();
			vTaskDelay(50);
			memset((char *)revbuf,0,512);
			revint=0;

		esp8266_station();
		vTaskDelay(50);
			p=strstr((const char *)revbuf,"OK");
		if(p==NULL)
		{
			Usart1_SentString("station set error\r\n");
		}
			Usart1_SentString((char *)revbuf);
			memset((char *)revbuf,0,512);
			revint=0;		
		
		
		esp8266_WiFi();
		vTaskDelay(5000);
		vTaskDelay(5000);
			p=strstr((const char *)revbuf,"OK");
		if(p==NULL)
		{
			Usart1_SentString("wifi connect  error\r\n");
		}
					Usart1_SentString((char *)revbuf);
			memset((char *)revbuf,0,512);
			revint=0;		
		

		vTaskSuspend(ESP8266initTask_Handler);
	}
	

}
void ConnectToWeatherTask(void *pvParameters)
{	
	char *p;
	while(1)
	{
			//退出透传模式
			esp8266_exitsend();
			vTaskDelay(50);
			memset((char *)revbuf,0,512);
			revint=0;
		
			Usart2_SentString("AT+CIPCLOSE\r\n");
		p=strstr((const char *)revbuf,"OK");
		if(p==NULL)
		{
			Usart1_SentString("tcp connect  error\r\n");
		}
					Usart1_SentString((char *)revbuf);
			memset((char *)revbuf,0,512);
			revint=0;		
		
			//连接tcp
			Usart2_SentString("AT+CIPSTART=\"TCP\",\"116.62.81.138\",80\r\n");
			vTaskDelay(100);
			p=strstr((const char *)revbuf,"OK");
		if(p==NULL)
		{
			Usart1_SentString("tcp connect  error\r\n");
		}
					Usart1_SentString((char *)revbuf);
			memset((char *)revbuf,0,512);
			revint=0;		
		//使能透传
		esp8266_intosendenable();
			vTaskDelay(50);
			p=strstr((const char *)revbuf,"OK");
		if(p==NULL)
		{
			Usart1_SentString("send mode enable  error\r\n");
		}
			Usart1_SentString((char *)revbuf);
			memset((char *)revbuf,0,512);
			revint=0;		
			//透传
			esp8266_intosend();
			vTaskDelay(50);
			p=strstr((const char *)revbuf,"OK");
		if(p==NULL)
		{
			Usart1_SentString("send  error\r\n");
			//失败了就重新按一次再来一遍连接wifi流程
		}
			Usart1_SentString((char *)revbuf);
			memset((char *)revbuf,0,512);
			revint=0;		
		
		Usart2_SentString("GET https://api.seniverse.com/v3/weather/now.json?key=SwpNQndf_JYHhmZ_0&location=beijing&language=zh-Hans&unit=c\r\n\r\n");
		vTaskDelay(300);
		
		vTaskSuspend(ConnectToWeatherTask_Handler);
	}
}


void ConnectToTimeTask(void *pvParameters)
{	
	char *p;
	while(1)
	{
			//退出透传模式
			esp8266_exitsend();
			vTaskDelay(50);
			memset((char *)revbuf,0,512);
			revint=0;
		
			Usart2_SentString("AT+CIPCLOSE\r\n");
			p=strstr((const char *)revbuf,"OK");
			if(p==NULL)
			{
				Usart1_SentString("tcp connect  error\r\n");
			}
				Usart1_SentString((char *)revbuf);
			memset((char *)revbuf,0,512);
			revint=0;		
		
			//连接tcp
			Usart2_SentString("AT+CIPSTART=\"TCP\",\"43.142.65.209\",80\r\n");
			vTaskDelay(100);
			p=strstr((const char *)revbuf,"OK");
		if(p==NULL)
		{
			Usart1_SentString("tcp connect  error\r\n");
		}
					Usart1_SentString((char *)revbuf);
			memset((char *)revbuf,0,512);
			revint=0;		
		//使能透传
		esp8266_intosendenable();
			vTaskDelay(50);
			p=strstr((const char *)revbuf,"OK");
		if(p==NULL)
		{
			Usart1_SentString("send mode enable  error\r\n");
		}
			Usart1_SentString((char *)revbuf);
			memset((char *)revbuf,0,512);
			revint=0;		
			//透传
			esp8266_intosend();
			vTaskDelay(50);
			p=strstr((const char *)revbuf,"OK");
		if(p==NULL)
		{
			Usart1_SentString("send  error\r\n");
			//失败了就重新按一次再来一遍连接wifi流程
		}
			Usart1_SentString((char *)revbuf);
			memset((char *)revbuf,0,512);
			revint=0;		
		
		Usart2_SentString("GET https://cn.apihz.cn/api/time/getapi.php?id=10006459&key=510e4bc2e1c4639e44194ca2fbaa06b2&type=1\r\n\r\n");
		vTaskDelay(300);
		Usart1_SentString((char *)revbuf);
			char *tmp=strtok((char *)revbuf,":");
	
		tmp=strtok(NULL,":");
		tmp=strtok(NULL,"\"");
		int ddd = atoi(tmp);
		memset((char *)revbuf,0,512);
		revint=0;
		
		RTC_Internet_SET(ddd);
				
		vTaskSuspend(ConnectToTimeTask_Handler);
	}
}
