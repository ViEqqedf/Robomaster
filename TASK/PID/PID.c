#include "PID.h"
#include "USART2.h"

PidTypeDef CM1_speed_pid={0};
PidTypeDef CM2_speed_pid={0};
PidTypeDef CM3_speed_pid={0};
PidTypeDef CM4_speed_pid={0};
PidTypeDef Gyro_normal_pid;
PidTypeDef Yaw_speed_pid={0};
PidTypeDef Yaw_position_pid={0};
PidTypeDef Yaw_resist_pid={0};
PidTypeDef Pitch_speed_pid={0};
PidTypeDef Pitch_position_pid={0};
PidTypeDef Shoot_speed_pid={0};
PidTypeDef Shoot_position_pid={0};
PidTypeDef Power_limit_pid={0};

//��ʼ��PID
void PID_Init(PidTypeDef * pid,double kp,double ki,double kd,double ka,double max_out,double dead_band,double i_band,double max_input)
{
	pid->Kp=kp;
	pid->Ki=ki;
	pid->Kd=kd;
	pid->Ka=ka;
	pid->max_out=max_out;
	pid->dead_band=dead_band;
	pid->intergral_band=i_band;
	pid->max_input=max_input;
	//PID���ֵ
	pid->output=0;
	//���
	pid->e[0]=0;
	pid->e[1]=0;
	pid->e[2]=0;//2���� 1��һ�� 0���ϴ�
	pid->d_last=0;
}

//PID����
void PID_Calc(PidTypeDef * pid, double rel_val, double set_val,u8 PID_mode)
{
	double p = 0,
		   i = 0,
		   d = 0;
	if(set_val > pid->max_input)
		set_val = pid->max_input;
	else if(set_val < -(pid->max_input))
		set_val = -(pid->max_input);

	pid->e[2] = set_val - rel_val; //��ǰ���
	
	if(pid==&Shoot_position_pid)
	{
		if(pid->e[2]>(pid->max_input/2))
		{
			pid->e[2]=pid->e[2]- pid->max_input;
		}
		else if((-pid->e[2])>(pid->max_input/2))
		{
			pid->e[2]=pid->max_input + pid->e[2];
		}
	}
	if(pid==&Gyro_normal_pid||pid==&Yaw_position_pid)
	{
		if(pid->e[2]>(pid->max_input))
		{
			pid->e[2]=pid->e[2]- 2 * pid->max_input;
		}
		else if((-pid->e[2])>(pid->max_input))
		{
			pid->e[2]=pid->max_input * 2 + pid->e[2];
		}
	}

	//�Ƿ��������
	if(MyAbs(pid->e[2]) >=pid->dead_band)//MyAbs(pid->e[2]) >= pid->dead_band
	{
		if(MyAbs(pid->e[2]) <= 15)
			pid->intergral = pid->intergral + (pid->Ki) * (pid->e[2]);
		else
			pid->intergral=pid->intergral*0.99;
		if(pid->intergral > pid->intergral_band) //��������
			pid->intergral= pid->intergral_band;
		else if(pid->intergral < -pid->intergral_band) //��������
			pid->intergral= -pid->intergral_band;
		
		
		if(PID_mode == Positional)
		{		
			p = pid->Kp * (pid->e[2]);
		if(pid==&Yaw_position_pid)
	  {
			if(p > 1800)
				p = 1800;
			else if(p < -1800)
				p =-1800;
		}
			i = pid->intergral;
			d = pid->Kd *( (1-pid->Ka)*(pid->e[2]-pid->e[1]) + (pid->Ka)*(pid->d_last) );
			pid->d_last = d;
			pid->output = p+i+d;
		}
		else if(PID_mode == Incremental)
		{
			p = pid->Kp * ((pid->e[2]-pid->e[1]));
			i = pid->Ki*pid->e[2];
			d = pid->Kd *(pid->e[2]-pid->e[1]*2+pid->e[0]);
			pid->output += p+i+d;
		}			
		
		//�Ƿ񳬳�������
		if(pid->output > pid->max_out)
			pid->output = pid->max_out;
		if(pid->output < -(pid->max_out))
			pid->output = -(pid->max_out);
		
	}
	else
	{
		pid->output=0;
	}
	/*�������֡*/
	pid->e[0] = pid->e[1];
	pid->e[1] = pid->e[2];
}

float MyAbs(float num)
{
	if(num>=0)
		return num;
	else 
		return -num;	
}



