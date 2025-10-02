#include "mpu6050.h"
#include "delay.h"

//sda引脚输出模式控制
void SDA_GPIOInit(GPIOMode_TypeDef GPIO_Mode)
{
		//打开时钟
	RCC_APB2PeriphClockCmd(RCC_IIC_PORT,ENABLE);
	//定义gpio结构体
	GPIO_InitTypeDef GPIO_InitStruct;
	//定义tx口
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode;
	GPIO_InitStruct.GPIO_Pin=IIC_SDA_PIN;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_PORT_IIC,&GPIO_InitStruct);
}
void SCL_GPIOInit()
{
		RCC_APB2PeriphClockCmd(RCC_IIC_PORT,ENABLE);
	//定义gpio结构体
	GPIO_InitTypeDef GPIO_InitStruct;
	//定义tx口
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin=IIC_SCL_PIN;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_PORT_IIC,&GPIO_InitStruct);
}
void IIC_Init()
{					     
	SDA_GPIOInit(GPIO_Mode_Out_OD);
	SCL_GPIOInit();
	IIC_SDA(1);
	IIC_SCL(1);
	delay_us(5);

}
//**************************************
//I2C起始信号
//**************************************
void IIC_Start()
{
	//确保SDA是输出模式
	SDA_GPIOInit(GPIO_Mode_Out_OD);
	//确保SDA和SCL是高电平
	IIC_SDA(1);
	IIC_SCL(1);
	delay_us(5);
	//让SDA在scl为高电平的时候拉低电平，然后再拉低SCL的电平
	IIC_SDA(0);
	delay_us(5);
	IIC_SCL(0);
	delay_us(5);
}
void IIC_STOP()
{
	SDA_GPIOInit(GPIO_Mode_Out_OD);
	IIC_SCL(0);
	IIC_SDA(0);
	delay_us(5);
	
	IIC_SCL(1);
	delay_us(5);
	IIC_SDA(1);
	delay_us(5);
}
void IIC_Master_IsACK(uint8_t ack)
{
  SDA_GPIOInit(GPIO_Mode_Out_OD);
	IIC_SCL(0);
	delay_us(5);
	
	if(ack)
	{
	IIC_SDA(1);//没应答
	}
	else
	{
	IIC_SDA(0);
	}
		
	delay_us(5);
	
	
	IIC_SCL(1);
	delay_us(5);
	
	IIC_SCL(0);
	delay_us(5);	
}
//**************************************
//I2C接收应答信号
//**************************************
	bool IIC_Slave_IsACK()
	{
		bool temp;
		SDA_GPIOInit(GPIO_Mode_IN_FLOATING);
		IIC_SCL(0);
		delay_us(5);
		
		IIC_SCL(1);
		delay_us(5);
		
		if(IIC_Read)
		{
			temp=false;
		}
		else
			temp=true;
		
		IIC_SCL(0);
		delay_us(5);
		return temp;
	}

void IIC_SentByte(uint8_t byte)
{
	//确保SDA是输出模式
	SDA_GPIOInit(GPIO_Mode_Out_OD);
		IIC_SCL(0);
		delay_us(5);
	for(int i=0;i<8;i++)
	{

		if(byte&0x80)
		{
			IIC_SDA(1);
		}
		else
		{
		IIC_SDA(0);
		}			
		byte<<=1;
		delay_us(5);
	
		IIC_SCL(1);
		delay_us(5);
			IIC_SCL(0);
		delay_us(5);
	}

}
uint8_t IIC_ReadByte()
{
	uint8_t data=0;
	
	SDA_GPIOInit(GPIO_Mode_IN_FLOATING);
	IIC_SCL(0);
	delay_us(5);
	
	for(int i =0;i<8;i++)
	{
		IIC_SCL(1);
		delay_us(5);
	
		data<<=1;
		data|=IIC_Read;
		//delay_us(5);
		
		IIC_SCL(0);
		delay_us(5);
	}
	return data;
}
//**************************************
//向I2C设备写入一个字节数据
//**************************************
void Single_WriteI2C(unsigned char REG_Address,unsigned char REG_data)
{
    IIC_Start();                  //起始信号
    IIC_SentByte(MPU6050_ADDRESS);   //发送设备地址+写信号
		if(IIC_Slave_IsACK()==false)
	{
			IIC_STOP();
			
		  
	}
    IIC_SentByte(REG_Address);    //内部寄存器地址，
		if(IIC_Slave_IsACK()==false)
	{
			IIC_STOP();
			
		  
	}
    IIC_SentByte(REG_data);       //内部寄存器数据，
		if(IIC_Slave_IsACK()==false)
	{
			IIC_STOP();
			
		  
	}
    IIC_STOP();                   //发送停止信号
}
//**************************************
//从I2C设备读取一个字节数据
//**************************************
unsigned char Single_ReadI2C(unsigned char REG_Address)
{
	unsigned char REG_data;
	IIC_Start();                   //起始信号
	IIC_SentByte(MPU6050_ADDRESS);    //发送设备地址+写信号
		if(IIC_Slave_IsACK()==false)
	{
			IIC_STOP();
			
		  return false;
	}
	IIC_SentByte(REG_Address);     //发送存储单元地址，从0开始	
		if(IIC_Slave_IsACK()==false)
	{
			IIC_STOP();
			
		  return false;
	}
	IIC_Start();                   //起始信号
	IIC_SentByte(MPU6050_ADDRESS+1);  //发送设备地址+读信号
		if(IIC_Slave_IsACK()==false)
	{
			IIC_STOP();
			
		  return false;
	}
	REG_data=IIC_ReadByte();       //读出寄存器数据
	IIC_Master_IsACK(1);                //接收应答信号
	IIC_STOP();                    //停止信号
	return REG_data;
}
//**************************************
//初始化MPU6050
//**************************************
void InitMPU6050()
{	
	IIC_Init();
	/*MPU6050寄存器初始化，需要对照MPU6050手册的寄存器描述配置，此处仅配置了部分重要的寄存器*/
	Single_WriteI2C(MPU6050_PWR_MGMT_1, 0x01);		//电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
	Single_WriteI2C(MPU6050_PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机
	Single_WriteI2C(MPU6050_SMPLRT_DIV, 0x09);		//采样率分频寄存器，配置采样率
	Single_WriteI2C(MPU6050_CONFIG, 0x06);			//配置寄存器，配置DLPF
	Single_WriteI2C(MPU6050_GYRO_CONFIG, 0x18);	//陀螺仪配置寄存器，选择满量程为±2000°/s
	Single_WriteI2C(MPU6050_ACCEL_CONFIG, 0x18);	//加速度计配置寄存器，选择满量程为±16g
}
//**************************************
//合成数据
//**************************************
 short GetData(unsigned char H_REG_Address,unsigned char L_REG_Address)
{
	char H,L;
	H=Single_ReadI2C(H_REG_Address);
	L=Single_ReadI2C(L_REG_Address);
	return (short)((H<<8)|L);   //合成数据
}

