//////////////////////////////////////////////////////////////////////////////////////	 
//  深圳市沃乐康科技有限公司  WWW.WLKLCD.COM  0755-32882855   专业液晶显示供应商   //
// （SPI4W 16BIT-65K色 ST77916+1.8IPS_360*360 @STM32F103C8T6 MDK5.14）             //
//  https://wlklcd.1688.com/     https://shop341012592.taobao.com/                 //
//  例程仅供学习参考，不做量产使用，不得用于商业用途                              //
//*********************************************************************************/
// --------------接口连线-------------------------------------------------------
//              屏幕               STM32F103
//          1、 VCC -------------  3.3V   电源输入
//          2、 GND -------------  GND   电源地
//          3、 SCK  --------------PB13   SPI时钟脚   也有标做SCL  SCLK 
//          4、 MOSI --------------PB15   SPI数据脚    也有标做SDA  SDI  DATA
//          5、 DC --------------  PB14   数据指令选择脚  高电平写入数据 低电平写入指令    也有标做RS / A0
//          6、 CS -------------   PB12   片选信号 低电平有效   也有标做CE 
//          7、 RES -------------  PB11   复位引脚初始化前需要有一个低电平  也有标做RST/RESET 
//					8、	BLC -------------- PB10   背光控制脚 高电平亮 低电平灭 输入PWM通过占空比调节亮度
// -----------------------------------------------------------------------------
//特别说明:  我们是屏幕生产厂家 不是做嵌入式开发的,写代码不是我们的专业, 虽然例程在我们手上已验证OK, 
//           但因为我们写程序的不专业, 也没有做严格校对.难免会有格式 语法错误 以及一些编译警告.
//           请老师高手不吝指正，谢谢。
//           有时变更了代码没有修改注释，请以代码定义为准，不要盯着注释。
//*******************************************************************************/
#ifndef __LCD_INIT_H
#define __LCD_INIT_H	
#include "stm32f10x.h" 

#define USE_HORIZONTAL  	0//定义液晶屏顺时针旋转方向 	0-0度旋转，1-90度旋转，2-180度旋转，3-270度旋转

//定义LCD的尺寸
#define LCD_W 360
#define LCD_H 360

//定义接口
#define TFT_PORT GPIOA
#define TFT_SCL  GPIO_Pin_5
#define TFT_SDA  GPIO_Pin_7
#define TFT_RES  GPIO_Pin_11
#define TFT_DC   GPIO_Pin_11
#define TFT_CS   GPIO_Pin_4
#define TFT_BLC  GPIO_Pin_10

//LCD重要参数集
typedef struct  
{										    
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;				  //LCD ID
	u8  dir;			  //横屏还是竖屏控制：0，竖屏；1，横屏。	
	u16	 wramcmd;		//开始写gram指令
	u16  rramcmd;   //开始读gram指令
	u16  setxcmd;		//设置x坐标指令
	u16  setycmd;		//设置y坐标指令	
  u8   xoffset;   //x坐标偏移量  
  u8	 yoffset;	  //y坐标偏移量	
}_lcd_dev; 	

//LCD参数
extern _lcd_dev lcddev;	//管理LCD重要参数
//-----------------LCD端口定义---------------- 
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


void LCD_GPIO_Init(void);//初始化GPIO
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA16(u16 dat);//写入二个字节
void LCD_WR_DATA24(u32 dat);//写入三个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);//设置坐标函数
void LCD_direction(u8 direction);//设置显示方向
void LCD_Init(void);//LCD初始化
uint8_t LCD_SPIWrit_Bus(u8 dat); 

#endif
