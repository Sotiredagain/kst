//////////////////////////////////////////////////////////////////////////////////	 
//  �� �� ��   : main.c
//  �� �� ��   : v2.0
//  ��    ��   : 
//  ��������   : 2018-10-31
//  ����޸�   : 
//  ��������   :��ʾ����(STM32F103ϵ��)
//----------------------------------------------------
// �޸���ʷ   :
// ��    ��   : 
// ��    ��   : 
// �޸�����   : �����ļ�
//All rights reserved
//******************************************************************************/
#include "stm32f10x.h"                  // Device header
#include "lvgl_app.h"
#include "FreeRTOS.h"



int main(void)
{
	//MAX30102_Init();//MAX30102��ʼ��
	//GetHeartRateSpo2_init();
//	LCD_Init();//LCD��ʼ��
//	i2c_CfgGpio();
//	CST816_Init();
//	
//	RTC_Init();
//	RTC_SET();

//	Timer_Init();
//	lv_init();
//	lv_port_disp_init();
//	lv_port_indev_init();

	demo();
//	lvgl_screen2();

	while(1)
	{	

//		lv_timer_handler();
//		delay_ms(5);
	}
}

void TIM2_IRQHandler()
{
//	static uint8_t step_time_count = 0;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
//			detect_step();
//			step_time_count ++;
//			if(step_time_count == 6)   //300ms
//			{
//				step_time_count = 0;
//				if(step_count != 0)
//				{
//					step_count = 0;
//					stepnum ++;
//				}
//			}
				
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
