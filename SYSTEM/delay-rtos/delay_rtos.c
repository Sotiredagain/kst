#include "stm32f10x.h"

void delay_us(u32 nus)
{
		int  sum  = 0;             //作为计数器，对递减次数进行累加
		int  load = SysTick->LOAD; //把Systick的重载寄存器的值备份 
		int  told = 0;						 //用于存储读取的Systick的VAL寄存器的第1次的值
		int  tnew = 0;  
			
		told=SysTick->VAL;
		while(1)
		{
		tnew=SysTick->VAL;	
			if(told!=tnew)
			{
				if(told>tnew)
				{
					sum +=told-tnew;
				}
				else
				{
					sum += load-tnew+told;
				}
				told=tnew;
				if(sum>=nus*72)
				{
					break;
				}					
			}
			
		}

			
}
void delay_ms(u32 nms)
{
	while(nms--)
	{
		delay_us(1000);
	}
	  }	  
