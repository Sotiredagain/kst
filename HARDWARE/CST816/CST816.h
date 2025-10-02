/////////////////////////////////////////////////////////////////////////////////////
//  深圳市沃乐康科技有限公司  WWW.WLKLCD.COM  0755-32882855   专业液晶显示供应商   //
// （显示4线SPI ST7789+154IPS-240*240 SPI1  触摸模拟IIC  @STM32F103C8T6 MDK5.14）  //
//  https://wlklcd.1688.com/     https://shop341012592.taobao.com/                 //
//  例程仅供学习参考，不做量产使用，不得用于商业用途                              //
//*********************************************************************************/
// --------------接口连线-----------------------------------
//     --        GND     电源地
//     显        VCC     3.3v 电源正极
//               SCK     PA5  SPI时钟信号  也有标做SCL  SCLK  
//     示        SDA     PA7  SPI数据信号  也有标做SDA  SDI  DATA
//               RES     PA0  复位引脚初始化前需要有一个低电平  也有标做RST/RESET 
//     接        DC      PA1  数据指令选择脚  高电平写入数据 低电平写入指令    也有标做RS / A0
//     口        CS      PA2  片选信号 低电平有效   也有标做CE 
//		 --			   BLC     PA3  背光控制脚 高电平灭 低电平亮 输入PWM通过占空比调节亮度
//     ------------------------------------------------------
//     触        SDA     PB6  触摸IIC 数据线
//     摸        SCL     PB7  触摸IIC 时钟线
//     接        RST     PB4  触摸复位脚
//     口        INT     PB5  触摸中断
// -----------------------------------------------------------------------------
//特别说明:  我们是屏幕生产厂家 不是做嵌入式开发的,写代码不是我们的专业, 虽然例程在我们手上已验证OK, 
//           但因为我们写程序的不专业, 也没有做严格校对.难免会有格式 语法错误 以及一些编译警告.
//           请老师高手不吝指正，谢谢。
//           有时变更了代码没有修改注释，请以代码定义为准，不要盯着注释。
//*******************************************************************************/
#ifndef __CST816_H_
#define __CST816_H_

#include "stm32f10x.h"  

#define SDA_IN()                  \
	{                             \
		GPIOB->CRL &= 0X0FFFFFFF; \
		GPIOB->CRL |= 0X40000000; \
	} //输入模式，浮空输入模式
#define SDA_OUT()                 \
	{                             \
		GPIOB->CRL &= 0X0FFFFFFF; \
		GPIOB->CRL |= 0X10000000; \
	} //通用推挽输出，输出速度50MHZ

#define TP_PRES_DOWN 0x81 //触屏被按下
#define TP_COORD_UD 0x40  //触摸坐标更新标记

//触摸点相关数据结构体定义
typedef struct
{
	u8 TouchSta; //触摸情况，b7:按下1/松开0; b6:0没有按键按下/1有按键按下;bit5:保留；bit4-bit0触摸点按下有效标志，有效为1，分别对应触摸点5-1；
	u16 x[5];	 //支持5点触摸，需要使用5组坐标存储触摸点数据
	u16 y[5];

} TouchPointRefTypeDef;
extern TouchPointRefTypeDef TPR_Structure;

/*********************IO操作函数*********************/
//#define CST816_RST_SET GPIO_SetBits(GPIOB,GPIO_Pin_4);		//用在输出模式
//#define CST816_RST_CLR GPIO_ResetBits(GPIOB,GPIO_Pin_4);		//用在输出模式

#define CST816_RST_SET GPIO_SetBits(GPIOB,GPIO_Pin_0);		//用在输出模式
#define CST816_RST_CLR GPIO_ResetBits(GPIOB,GPIO_Pin_0);		//用在输出模式

#define CST816_SCL_SET GPIO_SetBits(GPIOB,GPIO_Pin_9);		//用在输出模式
#define CST816_SCL_CLR GPIO_ResetBits(GPIOB,GPIO_Pin_9);		//用在输出模式

#define CST816_SDA_SET GPIO_SetBits(GPIOB,GPIO_Pin_8);		//用在输出模式
#define CST816_SDA_CLR GPIO_ResetBits(GPIOB,GPIO_Pin_8);		//用在输出模式

#define CST816_SDA_Read GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) //用在输入模式
//#define CST816_INT PBin(5)		//用在输入模式


// I2C读写命令
#define FT_CMD_WR 0X2A //写命令
#define FT_CMD_RD 0X2B //读命令
// CST816 部分寄存器定义
#define FT_DEVIDE_MODE 0x00	   // CST816模式控制寄存器
#define FT_REG_NUM_FINGER 0x02 //触摸状态寄存器

#define FT_TP1_REG 0X03 //第一个触摸点数据地址
#define FT_TP2_REG 0X09 //第二个触摸点数据地址
#define FT_TP3_REG 0X0F //第三个触摸点数据地址
#define FT_TP4_REG 0X15 //第四个触摸点数据地址
#define FT_TP5_REG 0X1B //第五个触摸点数据地址

#define FT_ID_G_LIB_VERSION 0xA1  //版本
#define FT_ID_G_MODE 0xA4		  // CST816中断模式控制寄存器
#define FT_ID_G_THGROUP 0x80	  //触摸有效值设置寄存器
#define FT_ID_G_PERIODACTIVE 0x88 //激活状态周期设置寄存器
#define Chip_Vendor_ID 0xA7		  //芯片ID(0x36)
#define ID_G_CST816ID 0xA8		  // 0x11

u8 CST816_WR_Reg(u16 reg, u8 *buf, u8 len);
void CST816_RD_Reg(u16 reg, u8 *buf, u8 len);
void CST816_Init(void);
void CST816_Scan(void);

#endif
