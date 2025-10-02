//////////////////////////////////////////////////////////////////////////////////////	 
//*********************************************************************************/
// ---------------------------------------------------------------------
//                            STM32F103
//           VCC -------------  3.3V  电源输入
//           GND -------------  GND   电源地
//           SCK  --------------PA5   SPI时钟脚   也有标做SCL  SCLK 
//           MOSI --------------PA7   SPI数据脚    也有标做SDA  SDI  DATA
//           DC --------------  PB14   数据指令选择脚  高电平写入数据 低电平写入指令    也有标做RS / A0
//           CS -------------   PA4   片选信号 低电平有效   也有标做CE  
//           RES -------------  PB11   复位引脚初始化前需要有一个低电平  也有标做RST/RESET  
//						BLC -------------- PB10   背光控制脚 高电平亮 低电平灭 输入PWM通过占空比调节亮度
// -----------------------------------------------------------------------------

//*******************************************************************************/
#include "lcd_init.h"
//#include "lcdfont.h"
#include "delay.h"
#include "stm32f10x.h"                  // Device header



_lcd_dev lcddev;

void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 
	GPIO_InitStructure.GPIO_Pin = TFT_RES|TFT_BLC;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 
	GPIO_InitStructure.GPIO_Pin = TFT_CS|TFT_DC;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(TFT_PORT, &GPIO_InitStructure);	  
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 
	GPIO_InitStructure.GPIO_Pin = TFT_SCL|TFT_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(TFT_PORT, &GPIO_InitStructure);
	GPIO_SetBits(TFT_PORT,TFT_RES|TFT_DC|TFT_CS);	
}

void Hardware_SPI_Init(void)
{
///////?????SPI???////////////////
	SPI_InitTypeDef SPI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);	 
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	//SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	
	SPI_Init(SPI1,&SPI_InitStructure);
	//SPI_Init(SPI2,&amp;SPI_InitStructure);
	
	SPI_Cmd(SPI1,ENABLE);
}




uint8_t LCD_SPIWrit_Bus(u8 dat) 
{	
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1,dat);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
	return SPI1->DR;
}


void LCD_WR_DATA8(u8 dat)
{
	LCD_CS_Clr();
	//LCD_Writ_Bus(dat);
	LCD_SPIWrit_Bus(dat);
	LCD_CS_Set();	
}

void LCD_WR_DATA16(u16 dat)
{   
	  LCD_CS_Clr();	
//	LCD_Writ_Bus(dat>>8);
//	LCD_Writ_Bus(dat);
		LCD_SPIWrit_Bus(dat>>8);
	  LCD_SPIWrit_Bus(dat);
	  LCD_CS_Set();	
}

void LCD_WR_DATA24(u32 dat)
{	
	LCD_SPIWrit_Bus(dat>>16);
	LCD_SPIWrit_Bus(dat>>8);
	LCD_SPIWrit_Bus(dat);
}

void LCD_WR_REG(u8 dat)
{
	LCD_CS_Clr();
	LCD_DC_Clr();//д????
	//LCD_Writ_Bus(dat);
	LCD_SPIWrit_Bus(dat);
	LCD_DC_Set();//д????
	LCD_CS_Set();	
}

void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{	
	LCD_WR_REG(lcddev.setxcmd);	
	LCD_WR_DATA8((xStar+lcddev.xoffset)>>8);
	LCD_WR_DATA8(xStar+lcddev.xoffset);		
	LCD_WR_DATA8((xEnd+lcddev.xoffset)>>8);
	LCD_WR_DATA8(xEnd+lcddev.xoffset);

	LCD_WR_REG(lcddev.setycmd);	
	LCD_WR_DATA8((yStar+lcddev.yoffset)>>8);
	LCD_WR_DATA8(yStar+lcddev.yoffset);		
	LCD_WR_DATA8((yEnd+lcddev.yoffset)>>8);
	LCD_WR_DATA8(yEnd+lcddev.yoffset);

	LCD_WR_REG(lcddev.wramcmd);;	//开始写入GRAM			
} 


void LCD_direction(u8 direction)
{ 
			lcddev.setxcmd=0x2A;
			lcddev.setycmd=0x2B;
			lcddev.wramcmd=0x2C;
	switch(direction){		  
	case 0:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			lcddev.xoffset=0;
			lcddev.yoffset=0;
	    LCD_WR_REG(0x36);
	    LCD_WR_DATA8(0);
		break;
		case 1:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			lcddev.xoffset=0;
			lcddev.yoffset=0;
		  LCD_WR_REG(0x36);
	    LCD_WR_DATA8((1<<6)|(1<<5));
		break;
		case 2:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;
      lcddev.xoffset=0;
			lcddev.yoffset=0;			
		  LCD_WR_REG(0x36);
	    LCD_WR_DATA8((1<<6)|(1<<7));
		break;
		case 3:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			lcddev.xoffset=0;
			lcddev.yoffset=0;
		  LCD_WR_REG(0x36);
	    LCD_WR_DATA8((1<<7)|(1<<5));
		break;	
		default:break;
	}		
}	 

void LCD_Init(void)
{
	LCD_GPIO_Init();
	Hardware_SPI_Init();
	LCD_CS_Set();
	LCD_RES_Set();
	LCD_DC_Set();
	

	LCD_RES_Clr();
	delay_ms(10);
	LCD_RES_Set();
	delay_ms(30);

//BOE154IPS ST7789V2初始化//			
LCD_WR_REG(0x11);
delay_ms(120);//delay_ms 120ms
//--------------------------------------Display Setting------------------------------------------//
LCD_WR_REG(0x36);
LCD_WR_REG(0x36); //MX, MY, RGB mode 
LCD_WR_DATA8(0x00); 
LCD_WR_REG(0x3a);
LCD_WR_DATA8(0x05);
LCD_WR_REG(0x21);
LCD_WR_REG(0x2a);
LCD_WR_DATA8(0x00);
LCD_WR_DATA8(0x00);
LCD_WR_DATA8(0x00);
LCD_WR_DATA8(0xef);
LCD_WR_REG(0x2b);
LCD_WR_DATA8(0x00);
LCD_WR_DATA8(0x00);
LCD_WR_DATA8(0x00);
LCD_WR_DATA8(0xef);
//--------------------------------ST7789V Frame rate setting----------------------------------//
LCD_WR_REG(0xb2);
LCD_WR_DATA8(0x0c);
LCD_WR_DATA8(0x0c);
LCD_WR_DATA8(0x00);
LCD_WR_DATA8(0x33);
LCD_WR_DATA8(0x33);
LCD_WR_REG(0xb7);
LCD_WR_DATA8(0x35);
//---------------------------------ST7789V Power setting--------------------------------------//
LCD_WR_REG(0xbb);
LCD_WR_DATA8(0x1f);
LCD_WR_REG(0xc0);
LCD_WR_DATA8(0x2c);
LCD_WR_REG(0xc2);
LCD_WR_DATA8(0x01);
LCD_WR_REG(0xc3);
LCD_WR_DATA8(0x12);
LCD_WR_REG(0xc4);
LCD_WR_DATA8(0x20);
LCD_WR_REG(0xc6);
LCD_WR_DATA8(0x0f);
LCD_WR_REG(0xd0);
LCD_WR_DATA8(0xa4);
LCD_WR_DATA8(0xa1);
//--------------------------------ST7789V gamma setting--------------------------------------//
LCD_WR_REG(0xe0);
LCD_WR_DATA8(0xd0);
LCD_WR_DATA8(0x08);
LCD_WR_DATA8(0x11);
LCD_WR_DATA8(0x08);
LCD_WR_DATA8(0x0c);
LCD_WR_DATA8(0x15);
LCD_WR_DATA8(0x39);
LCD_WR_DATA8(0x33);
LCD_WR_DATA8(0x50);
LCD_WR_DATA8(0x36);
LCD_WR_DATA8(0x13);
LCD_WR_DATA8(0x14);
LCD_WR_DATA8(0x29);
LCD_WR_DATA8(0x2d);
LCD_WR_REG(0xe1);
LCD_WR_DATA8(0xd0);
LCD_WR_DATA8(0x08);
LCD_WR_DATA8(0x10);
LCD_WR_DATA8(0x08);
LCD_WR_DATA8(0x06);
LCD_WR_DATA8(0x06);
LCD_WR_DATA8(0x39);
LCD_WR_DATA8(0x44);
LCD_WR_DATA8(0x51);
LCD_WR_DATA8(0x0b);
LCD_WR_DATA8(0x16);
LCD_WR_DATA8(0x14);
LCD_WR_DATA8(0x2f);
LCD_WR_DATA8(0x31);

LCD_WR_REG(0x29);	//Display on	

LCD_direction(USE_HORIZONTAL);//设置LCD显示方向
	//LCD_BLK_Set();
LCD_BLK_Clr();    //低电平背光亮
} 
