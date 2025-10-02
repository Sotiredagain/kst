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
//     触        SDA     PB8  触摸IIC 数据线
//     摸        SCL     PB9  触摸IIC 时钟线
//     接        RST     PB4  触摸复位脚
//     口        INT     PB5  触摸中断
// -----------------------------------------------------------------------------
//特别说明:  我们是屏幕生产厂家 不是做嵌入式开发的,写代码不是我们的专业, 虽然例程在我们手上已验证OK, 
//           但因为我们写程序的不专业, 也没有做严格校对.难免会有格式 语法错误 以及一些编译警告.
//           请老师高手不吝指正，谢谢。
//           有时变更了代码没有修改注释，请以代码定义为准，不要盯着注释。
//*******************************************************************************/
#include "delay.h"
#include "CST816.h"
#include "bsp_i2c_gpio.h"
#include "usart.h"

static void i2c_Delay(void)
{
	// uint8_t i;

	/*　
		下面的时间是通过逻辑分析仪测试得到的。
	工作条件：CPU主频72MHz ，MDK编译环境，1级优化

		循环次数为10时，SCL频率 = 205KHz
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us
		循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us
	*/
	// for (i = 0; i < 10; i++);
}

/****************************************************
 * 函数名称 ：
 * 功    能 ：单片机发送起始信号
 * 入口参数 ：无
 * 返回参数 ：无
 * 注意事项 ：
 *****************************************************/
void CST816_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	I2C_SDA_1();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_0();
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
}

/****************************************************
 * 函数名称 ：
 * 功    能 ：单片机发送停止信号
 * 入口参数 ：无
 * 返回参数 ：无
 * 注意事项 ：
 *****************************************************/
void CST816_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	I2C_SDA_0();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_1();
}

/****************************************************
 * 函数名称 ：
 * 功    能 ：单片机发送应答信号
 * 入口参数 ：无
 * 返回参数 ：无
 * 注意事项 ：单片机读1B数据后发送一个应答信号
 *****************************************************/
void CST816_McuACK(void)
{
	I2C_SCL_0();
	delay_us(1);
	I2C_SDA_0();
	delay_us(1);
	I2C_SCL_1(); // SCL最小高电平脉宽:0.6us
	delay_us(1);
	I2C_SCL_0(); // SCL最小低电平脉宽:1.2us
}

/****************************************************
 * 函数名称 ：
 * 功    能 ：单片机发送非应答信号
 * 入口参数 ：无
 * 返回参数 ：无
 * 注意事项 ：单片机读数据停止前发送一个非应答信号
 *****************************************************/
void CST816_McuNACK(void)
{
	I2C_SCL_0();
	delay_us(1);
	I2C_SDA_1();
	delay_us(1);
	I2C_SCL_1(); // SCL最小高电平脉宽:0.6us
	delay_us(1);
	I2C_SCL_0(); // SCL最小低电平脉宽:1.2us
}

/****************************************************
* 函数名称 ：
* 功    能 ：单片机检查CST816送来的应答信号
* 入口参数 ：无
* 返回参数 ：1，接收应答失败
			 0，接收应答成功
* 注意事项 ：单片机写1个地址/数据后检查
			 全局变量RevAckF:收到CST816应答信号的标志位,为0表示收到
*****************************************************/
u8 CST816_CheckAck(void)
{
	u8 ucErrTime = 0;
	// SDA_IN();      				//SDA设置为输入
	I2C_SDA_1();
	I2C_SCL_1(); //使SDA上数据有效;SCL最小高电平脉宽:0.6us
	delay_us(2);
	while (I2C_SDA_READ())
	{
		ucErrTime++;
		if (ucErrTime > 250) //无应答
		{
			CST816_Stop();
			return 1;
		}
		delay_us(2);
	}
	I2C_SCL_0();
	return 0;
}

/****************************************************
 * 函数名称 ：
 * 功    能 ：单片机向IIC总线发送1B的地址/数据
 * 入口参数 ：待发送的1B地址/数据
 * 返回参数 ：无
 * 注意事项 ：不是一个完整的数据发送过程;送数的顺序是从高到低
 *****************************************************/
void CST816_WrOneByte(u8 dat)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{
		if (dat & 0x80)
		{
			I2C_SDA_1();
		}
		else
		{
			I2C_SDA_0();
		}
		i2c_Delay();
		I2C_SCL_1();
		i2c_Delay();
		I2C_SCL_0();
		if (i == 7)
		{
			I2C_SDA_1(); // 释放总线
		}
		dat <<= 1; /* 左移一个bit */
		i2c_Delay();
	}
}

/****************************************************
 * 函数名称 ：
 * 功    能 ：单片机从IIC总线接收1B的数据
 * 入口参数 ：无
 * 返回参数 ：收到的1B数据
 * 注意事项 ：不是一个完整的数据接收过程;从高到低的顺序接收数据
 *****************************************************/
u8 CST816_RdOneByte(void)
{
	u8 i, dat = 0; //接收数据位数和内容暂存单元

	I2C_SDA_1(); //使能上拉,准备读取数据
	for (i = 8; i > 0; i--)
	{
		I2C_SCL_0();
		i2c_Delay();
		i2c_Delay();
		i2c_Delay();
		I2C_SCL_1();
		dat <<= 1;
		if (CST816_SDA_Read)
			dat++;
		i2c_Delay();
		i2c_Delay();
		i2c_Delay(); // SCL最小低电平脉宽:1.2us
	}
	I2C_SDA_1();
	return (dat); //返回1B的数据
}

//向CST816写入一次数据
// reg:起始寄存器地址
// buf:数据缓缓存区
// len:写数据长度
//返回值:0,成功;1,失败.
u8 CST816_WR_Reg(u16 reg, u8 *buf, u8 len)
{
	u8 i;
	u8 ret = 0;
	CST816_Start();
	CST816_WrOneByte(FT_CMD_WR); //发送写命令
	CST816_CheckAck();
	CST816_WrOneByte(reg & 0XFF); //发送低8位地址
	CST816_CheckAck();
	for (i = 0; i < len; i++)
	{
		CST816_WrOneByte(buf[i]); //发数据
		ret = CST816_CheckAck();
		if (ret)
			break;
	}
	CST816_Stop(); //产生一个停止条件
	return ret;
}
//从CST816读出一次数据
// reg:起始寄存器地址
// buf:数据缓缓存区
// len:读数据长度
void CST816_RD_Reg(u16 reg, u8 *buf, u8 len)
{
	u8 i;
	CST816_Start();
	CST816_WrOneByte(FT_CMD_WR); //发送写命令
	CST816_CheckAck();
	CST816_WrOneByte(reg & 0XFF); //发送低8位地址
	CST816_CheckAck();
	CST816_Start();
	CST816_WrOneByte(FT_CMD_RD); //发送读命令
	CST816_CheckAck();
	for (i = 0; i < len; i++)
	{
		*buf++ = CST816_RdOneByte(); //读入1B数据到接收数据缓冲区中
		CST816_McuACK();			 //发送应答位
	}
	CST816_McuNACK(); // n个字节读完,发送非应答位
	CST816_Stop();	  //产生一个停止条件
}
/*
**函数名：CST816_Init
**传入参数：无
**返回值：无
**功能：初始化CST816引脚
*/
void CST816_Init(void)
{
	u8 temp;
	GPIO_InitTypeDef GPIO_InitStructure;										 //定义一个GPIO_InitTypeDef类型的结构体
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE); //使能PG端口时钟

	/******  PB9:时钟SCL-----PB8:数据SDA  ***********************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; // 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	   //最高输出速率50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);				   //初始化对应GPIOG
	GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);		   // PG.7.8 输出高
	/******  PB8:片选RST  ***************************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		  // 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			  //初始化对应GPIOC
	GPIO_SetBits(GPIOB, GPIO_Pin_0);				  // PC.7 输出高
	
	/**********************PB5，中断INT*********************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	  // PC6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);		  //初始化对应GPIOC
	GPIO_SetBits(GPIOB, GPIO_Pin_5);

	CST816_RST_CLR;
	delay_ms(50);
	CST816_RST_SET;
	delay_ms(100);
	CST816_SDA_SET;
	CST816_SCL_SET;
	delay_ms(10);
	temp = 0;
	CST816_WR_Reg(FT_DEVIDE_MODE, &temp, 1);  //进入正常操作模式
	temp = 21;								  //触摸有效值，22，越小越灵敏
	CST816_WR_Reg(FT_ID_G_THGROUP, &temp, 1); //设置触摸有效值
	temp = 12;								  //激活周期，不能小于12，最大14
	CST816_WR_Reg(FT_ID_G_PERIODACTIVE, &temp, 1);
	/******************************************************/
}
const u16 CST816_TPX_TBL[5] =
	{
		FT_TP1_REG,
		FT_TP2_REG,
		FT_TP3_REG,
		FT_TP4_REG,
		FT_TP5_REG};
TouchPointRefTypeDef TPR_Structure;
void CST816_Scan(void)
{
	u8 i = 0;
	u8 sta = 0;
	u8 buf[6] = {0};
	CST816_RD_Reg(0x02, &sta, 1); //读取触摸点的状态

	if (sta & 0x0f) //判断是否有触摸点按下，0x02寄存器的低4位表示有效触点个数
	{
		TPR_Structure.TouchSta = ~(0xFF << (sta & 0x0F)); //~(0xFF << (sta & 0x0F))将点的个数转换为触摸点按下有效标志
		for (i = 0; i < 1; i++)							  //分别判断触摸点1-5是否被按下
		{
			if (TPR_Structure.TouchSta & (1 << i))		  //读取触摸点坐标
			{											  //被按下则读取对应触摸点坐标数据
				CST816_RD_Reg(CST816_TPX_TBL[i], buf, 6); //读取XY坐标值
				// printf("0--%x\r\n",buf[0]);
				// printf("1--%x\r\n",buf[1]);
				// printf("2--%x\r\n",buf[2]);
				// printf("3--%x\r\n",buf[3]);
				// printf("4--%x\r\n",buf[4]);
				// printf("5--%x\r\n",buf[5]);
				TPR_Structure.x[i] = buf[2];
				TPR_Structure.y[i] = buf[4];
				//TPR_Structure.x[i]=((u16)(buf[1]&0X0F)<<8)+buf[2];
				//TPR_Structure.y[i]=((u16)(buf[3]&0X0F)<<8)+buf[4];
				if ((buf[1] & 0XC0) != 0X80)
				{
					TPR_Structure.x[i] = TPR_Structure.y[i] = 0; //必须是contact事件，才认为有效
					return;
				}
			}
		}
		TPR_Structure.TouchSta |= TP_PRES_DOWN; //触摸按下标记
	}
	else
	{
		if (TPR_Structure.TouchSta & TP_PRES_DOWN) //之前是被按下的
			TPR_Structure.TouchSta &= ~0x80;	   //触摸松开标记
		else
		{
			TPR_Structure.x[0] = 0;
			TPR_Structure.y[0] = 0;
			TPR_Structure.TouchSta &= 0xe0; //清楚触摸点有效标记
		}
	}
}
