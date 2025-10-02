#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f10x.h" 

void usart1_init(uint32_t Baud);
void Usart1_SentString(const char* String);
void usart2_init(uint32_t Baud);
void Usart2_SentString(const char* String);

#endif


