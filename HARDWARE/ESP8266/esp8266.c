#include "Usart.h"
#include "delay.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

extern char passwd[32];
extern char userna[32];
	
void esp8266_exitsend()
{
    Usart2_SentString("+++");

//    char *p;
//	p=strstr((const char *)revbuf,"OK");
//	if(p!=NULL)
//	{
//		return false;
//	}
//    return true;
}

void esp8266_station()
{
    Usart2_SentString("AT+CWMODE_CUR=1\r\n");
//    vTaskDelay(500);
//    char *p;
//	p=strstr((const char *)revbuf,"OK");
//	if(p!=NULL)
//	{
//        memset((char *)revbuf,0,512);
//	    revint=0;
//		return false;
//	}
//   
//	Usart1_SentString((char *)revbuf);	
//	memset((char *)revbuf,0,512);
//	revint=0;
//    return true;
}

void esp8266_WiFi()
{		
		Usart1_SentString(userna);
		Usart1_SentString(passwd);
		char buf[64]={0};
	  sprintf(buf,"AT+CWJAP=\"%s\",\"%s\"\r\n",userna,passwd);
    Usart2_SentString(buf);//"AT+CWJAP=\"%s\",\"%s\"\r\n",user,passwd
	
//    vTaskDelay(1000*2);
//    vTaskDelay(1000*2);
//	  vTaskDelay(1000*2);
//	  vTaskDelay(1000*2);
//	  vTaskDelay(1000*2);
//	
//    char *p;
//	p=strstr((const char *)revbuf,"OK");
//	if(p!=NULL)
//	{
//        memset((char *)revbuf,0,512);
//	    revint=0;
//		return false;
//	}
//   
//	Usart1_SentString((char *)revbuf);	
//	memset((char *)revbuf,0,512);
//	revint=0;
//    return true;
}
void esp8266_tcp()
{
	Usart2_SentString("AT+CIPSTART=\"TCP\",\"49.235.116.180\",80\r\n");
//	vTaskDelay(50);
//	vTaskDelay(50);
//    char *p;
//	p=strstr((const char *)revbuf,"OK");
//	if(p!=NULL)
//	{
//        memset((char *)revbuf,0,512);
//	    revint=0;
//		return false;
//	}
//   
//	Usart1_SentString((char *)revbuf);	
//	memset((char *)revbuf,0,512);
//	revint=0;
//    return true;

}
void esp8266_intosendenable()
{
    Usart2_SentString("AT+CIPMODE=1\r\n");
//    vTaskDelay(50);
//    char *p;
//	p=strstr((const char *)revbuf,"OK");
//	if(p!=NULL)
//	{
//        memset((char *)revbuf,0,512);
//	    revint=0;
//		return false;
//	}
//   
//	Usart1_SentString((char *)revbuf);	
//	memset((char *)revbuf,0,512);
//	revint=0;
//    
//    
//    vTaskDelay(50);
//  
//	p=strstr((const char *)revbuf,"OK");
//	if(p!=NULL)
//	{
//        memset((char *)revbuf,0,512);
//	    revint=0;
//		return false;
//	}
//   
//	Usart1_SentString((char *)revbuf);	
//	memset((char *)revbuf,0,512);
//	revint=0;
//    return true;
}
void esp8266_intosend()
{
	Usart2_SentString("AT+CIPSEND\r\n");
}

void esp8266_sendtimeGET()
{
    Usart2_SentString("GET https://cn.apihz.cn/api/time/getapi.php?id=10006459&key=510e4bc2e1c4639e44194ca2fbaa06b2&type=1\r\n\r\n");
}
