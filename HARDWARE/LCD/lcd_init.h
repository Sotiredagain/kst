//////////////////////////////////////////////////////////////////////////////////////	 
//  ���������ֿ��Ƽ����޹�˾  WWW.WLKLCD.COM  0755-32882855   רҵҺ����ʾ��Ӧ��   //
// ��SPI4W 16BIT-65Kɫ ST77916+1.8IPS_360*360 @STM32F103C8T6 MDK5.14��             //
//  https://wlklcd.1688.com/     https://shop341012592.taobao.com/                 //
//  ���̽���ѧϰ�ο�����������ʹ�ã�����������ҵ��;                              //
//*********************************************************************************/
// --------------�ӿ�����-------------------------------------------------------
//              ��Ļ               STM32F103
//          1�� VCC -------------  3.3V   ��Դ����
//          2�� GND -------------  GND   ��Դ��
//          3�� SCK  --------------PB13   SPIʱ�ӽ�   Ҳ�б���SCL  SCLK 
//          4�� MOSI --------------PB15   SPI���ݽ�    Ҳ�б���SDA  SDI  DATA
//          5�� DC --------------  PB14   ����ָ��ѡ���  �ߵ�ƽд������ �͵�ƽд��ָ��    Ҳ�б���RS / A0
//          6�� CS -------------   PB12   Ƭѡ�ź� �͵�ƽ��Ч   Ҳ�б���CE 
//          7�� RES -------------  PB11   ��λ���ų�ʼ��ǰ��Ҫ��һ���͵�ƽ  Ҳ�б���RST/RESET 
//					8��	BLC -------------- PB10   ������ƽ� �ߵ�ƽ�� �͵�ƽ�� ����PWMͨ��ռ�ձȵ�������
// -----------------------------------------------------------------------------
//�ر�˵��:  ��������Ļ�������� ������Ƕ��ʽ������,д���벻�����ǵ�רҵ, ��Ȼ������������������֤OK, 
//           ����Ϊ����д����Ĳ�רҵ, Ҳû�����ϸ�У��.������и�ʽ �﷨���� �Լ�һЩ���뾯��.
//           ����ʦ���ֲ���ָ����лл��
//           ��ʱ����˴���û���޸�ע�ͣ����Դ��붨��Ϊ׼����Ҫ����ע�͡�
//*******************************************************************************/
#ifndef __LCD_INIT_H
#define __LCD_INIT_H	
#include "stm32f10x.h" 

#define USE_HORIZONTAL  	0//����Һ����˳ʱ����ת���� 	0-0����ת��1-90����ת��2-180����ת��3-270����ת

//����LCD�ĳߴ�
#define LCD_W 360
#define LCD_H 360

//����ӿ�
#define TFT_PORT GPIOA
#define TFT_SCL  GPIO_Pin_5
#define TFT_SDA  GPIO_Pin_7
#define TFT_RES  GPIO_Pin_11
#define TFT_DC   GPIO_Pin_11
#define TFT_CS   GPIO_Pin_4
#define TFT_BLC  GPIO_Pin_10

//LCD��Ҫ������
typedef struct  
{										    
	u16 width;			//LCD ���
	u16 height;			//LCD �߶�
	u16 id;				  //LCD ID
	u8  dir;			  //���������������ƣ�0��������1��������	
	u16	 wramcmd;		//��ʼдgramָ��
	u16  rramcmd;   //��ʼ��gramָ��
	u16  setxcmd;		//����x����ָ��
	u16  setycmd;		//����y����ָ��	
  u8   xoffset;   //x����ƫ����  
  u8	 yoffset;	  //y����ƫ����	
}_lcd_dev; 	

//LCD����
extern _lcd_dev lcddev;	//����LCD��Ҫ����
//-----------------LCD�˿ڶ���---------------- 
#define LCD_SCLK_Clr() GPIO_ResetBits(TFT_PORT,TFT_SCL)//SCL=SCLK
#define LCD_SCLK_Set() GPIO_SetBits(TFT_PORT,TFT_SCL)

#define LCD_MOSI_Clr() GPIO_ResetBits(TFT_PORT,TFT_SDA)//SDA=MOSI
#define LCD_MOSI_Set() GPIO_SetBits(TFT_PORT,TFT_SDA)

#define LCD_RES_Clr()  GPIO_ResetBits(GPIOB,TFT_RES)//RES
#define LCD_RES_Set()  GPIO_SetBits(GPIOB,TFT_RES)

#define LCD_DC_Clr()   GPIO_ResetBits(TFT_PORT,TFT_DC)//DC
#define LCD_DC_Set()   GPIO_SetBits(TFT_PORT,TFT_DC)

#define LCD_CS_Clr()   GPIO_ResetBits(TFT_PORT,TFT_CS)//CS1
#define LCD_CS_Set()   GPIO_SetBits(TFT_PORT,TFT_CS)

#define LCD_BLK_Clr()  GPIO_ResetBits(GPIOB,TFT_BLC)//BLK
#define LCD_BLK_Set()  GPIO_SetBits(GPIOB,TFT_BLC)


void LCD_GPIO_Init(void);//��ʼ��GPIO
void LCD_WR_DATA8(u8 dat);//д��һ���ֽ�
void LCD_WR_DATA16(u16 dat);//д������ֽ�
void LCD_WR_DATA24(u32 dat);//д�������ֽ�
void LCD_WR_REG(u8 dat);//д��һ��ָ��
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);//�������꺯��
void LCD_direction(u8 direction);//������ʾ����
void LCD_Init(void);//LCD��ʼ��
uint8_t LCD_SPIWrit_Bus(u8 dat); 

#endif
