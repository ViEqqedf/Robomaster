#ifndef _MPU6050_H_
#define _MPU6050_H_
#include "sys.h"

typedef struct
	{
		uint32_t check;//�����ǵ�У�飬ȷ����������ȷ�ġ�ֵΪ5AA50700
		struct
		{
		int16_t pitch;//������
		int16_t roll;//�����
		int16_t yaw;//�����
		}angle;
		struct
		{
		int16_t pitch;//������
		int16_t roll;//�����
		int16_t yaw;//�����
		}v;
	}MPU6050_t;

extern volatile unsigned char gyro_rx_buffer[25];
extern MPU6050_t MPUEX;
void IMU_EX_Init(void);

/* ----------------------- Internal Data ----------------------------------- */
#endif

