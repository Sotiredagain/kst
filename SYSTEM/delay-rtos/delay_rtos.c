#include "stm32f10x.h"

void delay_us(u32 nus)
{
		int  sum  = 0;             //��Ϊ���������Եݼ����������ۼ�
		int  load = SysTick->LOAD; //��Systick�����ؼĴ�����ֵ���� 
		int  told = 0;						 //���ڴ洢��ȡ��Systick��VAL�Ĵ����ĵ�1�ε�ֵ
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
