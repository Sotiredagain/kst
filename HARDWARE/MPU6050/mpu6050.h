#ifndef __MPU6050__H
#define __MPU6050__H

#include "stm32f10x.h"
#include <stdbool.h>
/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define GPIO_PORT_IIC GPIOC				  /* GPIO端口 */
#define RCC_IIC_PORT RCC_APB2Periph_GPIOC /* GPIO端口时钟 */
#define IIC_SCL_PIN GPIO_Pin_7			  /* 连接到SCL时钟线的GPIO */
#define IIC_SDA_PIN GPIO_Pin_8			  /* 连接到SDA数据线的GPIO */

/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */														   
#define IIC_SDA(N) (N)?GPIO_SetBits(GPIO_PORT_IIC,IIC_SDA_PIN):GPIO_ResetBits(GPIO_PORT_IIC,IIC_SDA_PIN);
#define IIC_SCL(N) (N)?GPIO_SetBits(GPIO_PORT_IIC,IIC_SCL_PIN):GPIO_ResetBits(GPIO_PORT_IIC,IIC_SCL_PIN);
#define IIC_Read 	 GPIO_ReadInputDataBit(GPIO_PORT_IIC,IIC_SDA_PIN)

//****************************************
// 定义MPU6050内部地址
//****************************************
#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C
#define MPU6050_ADDRESS		0xD0		//MPU6050的I2C从机地址
#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75

void InitMPU6050(void);
short GetData(unsigned char H_REG_Address,unsigned char L_REG_Address);

#endif
