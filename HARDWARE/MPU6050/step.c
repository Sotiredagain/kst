#include <stdio.h>
#include "stm32f10x.h"
#include "mpu6050.h"

#define ABS(a) (0 - (a)) > 0 ? (-(a)) : (a)   //取a的绝对值
#define SAMPLE_NUM                10          //采样10次取平均值
#define MIN_RELIABLE_VARIATION    500         //最小可信赖变化量
#define MAX_RELIABLE_VARIATION    5000        //最大可信赖变化量
#define MAX(a,b) (a > b) ? a : b; 
#define MIN(a,b) (a < b) ? a : b; 
//三轴数据
typedef struct
{
	int16_t X;
	int16_t Y;
	int16_t Z;
}axis_value_t;              
axis_value_t old_ave_GyroValue, ave_GyroValue;
//极值数据
typedef struct
{
	axis_value_t max;
	axis_value_t min;
}peak_value_t;
peak_value_t peak_value;


#define ACTIVE_NUM          30            //最活跃轴更新周期
#define ACTIVE_NULL         0             //最活跃轴未知
#define ACTIVE_X            1             //最活跃轴是X
#define ACTIVE_Y            2             //最活跃轴是Y
#define ACTIVE_Z            3             //最活跃轴是Z
uint8_t most_active_axis = ACTIVE_NULL;   //记录最活跃轴
uint16_t step_count;
uint16_t stepnum;

void Gyro_sample_update(void)
{
	axis_value_t GyroValue;
	axis_value_t change;
	int sum[3] = {0};
	uint8_t success_num = 0;
 
	//保存上一次测量的原始数据
	old_ave_GyroValue.X = ave_GyroValue.X;
	old_ave_GyroValue.Y = ave_GyroValue.Y;
	old_ave_GyroValue.Z = ave_GyroValue.Z;
 
	//多次测量取平均值
	for(uint8_t i = 0; i < SAMPLE_NUM; i++)
	{
			GyroValue.X=GetData(MPU6050_GYRO_XOUT_H,MPU6050_GYRO_XOUT_L);
			GyroValue.Y=GetData(MPU6050_GYRO_YOUT_H,MPU6050_GYRO_YOUT_L);
			GyroValue.Z=GetData(MPU6050_GYRO_ZOUT_H,MPU6050_GYRO_ZOUT_L);

			sum[0] += GyroValue.X;
			sum[1] += GyroValue.Y;
			sum[2] += GyroValue.Z;
			success_num ++;
		
	}
	ave_GyroValue.X = sum[0] / success_num;
	ave_GyroValue.Y = sum[1] / success_num;
	ave_GyroValue.Z = sum[2] / success_num;
 
	//原始数据变化量
	change.X = ABS(ave_GyroValue.X - old_ave_GyroValue.X);
	change.Y = ABS(ave_GyroValue.Y - old_ave_GyroValue.Y);
	change.Z = ABS(ave_GyroValue.Z - old_ave_GyroValue.Z);
	
	//如果变化量超出可接受的变化值，则将原始数据退回到上一次的大小
	if(change.X < MIN_RELIABLE_VARIATION || change.X > MAX_RELIABLE_VARIATION)
	{
		ave_GyroValue.X = old_ave_GyroValue.X;
	}
	if(change.Y < MIN_RELIABLE_VARIATION || change.Y > MAX_RELIABLE_VARIATION)
	{
		ave_GyroValue.Y = old_ave_GyroValue.Y;
	}	
	if(change.Z < MIN_RELIABLE_VARIATION || change.Z > MAX_RELIABLE_VARIATION)
	{
		ave_GyroValue.Z = old_ave_GyroValue.Z;
	}
 
	//分别保存三轴角速度原始数据的最大值和最小值
	peak_value.max.X = MAX(peak_value.max.X , ave_GyroValue.X);
	peak_value.min.X = MIN(peak_value.min.X , ave_GyroValue.X);
	peak_value.max.Y = MAX(peak_value.max.Y , ave_GyroValue.Y);
	peak_value.min.Y = MIN(peak_value.min.Y , ave_GyroValue.Y);
	peak_value.max.Z = MAX(peak_value.max.Z , ave_GyroValue.Z);
	peak_value.min.Z = MIN(peak_value.min.Z , ave_GyroValue.Z);
}

void which_is_active(void)
{
	axis_value_t change;
	static axis_value_t active;        //三个轴的活跃度权重
	static uint8_t active_sample_num; 
 
	Gyro_sample_update();
	active_sample_num ++;
 
	//每隔一段时间，比较一次权重大小，判断最活跃轴
	if(active_sample_num >= ACTIVE_NUM)
	{
		if(active.X > active.Y && active.X > active.Z)
		{
			most_active_axis = ACTIVE_X;
		}
		else if(active.Y > active.X && active.Y > active.Z)
		{
			most_active_axis = ACTIVE_Y;
		}
		else if(active.Z > active.X && active.Z > active.Y)
		{
			most_active_axis = ACTIVE_Z;
		}
		else
		{
			most_active_axis = ACTIVE_NULL;
		}
		active_sample_num = 0;
		active.X = 0;
		active.Y = 0;
		active.Z = 0;
	}
 
	//原始数据变化量
	change.X = ABS(ave_GyroValue.X - old_ave_GyroValue.X);
	change.Y = ABS(ave_GyroValue.Y - old_ave_GyroValue.Y);
	change.Z = ABS(ave_GyroValue.Z - old_ave_GyroValue.Z);
 
	//增加三轴活跃度权重
	if(change.X > change.Y && change.X > change.Z)
	{
		active.X ++;
	}
	else if(change.Y > change.X && change.Y > change.Z)
	{
		active.Y ++;
	}
	else if(change.Z > change.X && change.Z > change.Y)
	{
		active.Z ++;
	}
}

void detect_step(void)
{
	int16_t mid;
	which_is_active();
	switch(most_active_axis)
	{
		case ACTIVE_NULL:
			break;
		//捕捉原始数据骤增和骤减现象
		case ACTIVE_X:
			mid = (peak_value.max.X + peak_value.min.X) / 2;
			if(old_ave_GyroValue.X < mid && ave_GyroValue.X > mid)
			{
				step_count ++;
			}
			else if(old_ave_GyroValue.X > mid && ave_GyroValue.X < mid)
			{
				step_count ++;
			}
			break;
		case ACTIVE_Y:
			mid = (peak_value.max.Y + peak_value.min.Y) / 2;
			if(old_ave_GyroValue.Y < mid && ave_GyroValue.Y > mid)
			{
				step_count ++;
			}
			else if(old_ave_GyroValue.Y > mid && ave_GyroValue.Y < mid)
			{
				step_count ++;
			}
			break;	
		case ACTIVE_Z:
			mid = (peak_value.max.Z + peak_value.min.Z) / 2;
			if(old_ave_GyroValue.Z < mid && ave_GyroValue.Z > mid)
			{
				step_count ++;
			}
			else if(old_ave_GyroValue.Z > mid && ave_GyroValue.Z < mid)
			{
				step_count ++;
			}
			break;
		default:
			break;
	}
}

 
//void timer3_handler(nrf_timer_event_t event_type, void* p_context)
//{
//	static uint8_t step_time_count = 0;
//	switch(event_type)
//	{
//		case NRF_TIMER_EVENT_COMPARE0:
//			
//			detect_step();
//			step_time_count ++;
//			if(step_time_count == 6)   //300ms
//			{
//				step_time_count = 0;
//				if(step_count != 0)
//				{
//					step_count = 0;
//					stepnum ++;
//				}
//			}
//			break;
//		default:
//			break;
//	}
//}
