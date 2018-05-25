#include "main.h"
#include "Remote.h"

 uint32_t time_tick_1ms = 5000;//ԭ����static����
s16 pitch_move = 3320;
s16 yaw_move;//���������λ����������ˮƽ�����λ��������
s16 CM1Speed,CM2Speed,CM3Speed,CM4Speed=0;//�����ĸ�������ٶ�ֵ������1�ţ�����2�ţ�����3�ţ�����4��
s16 MPUEX_Yaw_Init=0;
int cur_shooter_heat16=0;
const float a1=185,b1=165,l1=45;
const float k1=a1+b1-l1,k2=a1+b1+l1;
extern s32 shootSpeed;
u8 data[30];
s16 speed = 0;
double max_Power = 75;
const float power_data[61]=
{
	   0  ,  0.0030  ,  0.0060  ,  0.0090  ,  0.0120  ,  0.0150  ,  0.0180  ,  0.0210  ,  0.0240  ,  0.0270  ,  0.0300   ,  0.0390  ,  
0.0480  ,  0.0570  ,  0.0660  ,  0.0750  ,  0.0840  ,  0.0930  ,  0.1020  ,  0.1110  ,  0.1200  ,  0.1460  ,  0.1720   ,  0.1980  ,
0.2240  ,  0.2500  ,  0.3000  ,  0.3500  ,  0.4000  ,  0.4500  ,  0.5000  ,  0.5500  ,  0.6000  ,  0.6500  ,  0.7000   ,  0.7500  ,
0.7760  ,  0.8020  ,  0.8280  ,  0.8540  ,  0.8800  ,  0.8890  ,  0.8980  ,  0.9070  ,  0.9160  ,  0.9250  ,  0.9340   ,  0.9430  ,
0.9520  ,  0.9610  ,  0.9700  ,  0.9730  ,  0.9760  ,  0.9790  ,  0.9820  ,  0.9850  ,  0.9880  ,  0.9910  ,  0.9940   ,  0.9970  ,  
1.0000 
};  //�������ƺ���
//��ʱ���жϴ����Ŀ�������
uint16_t my;
void Control_Task(void)
{
	time_tick_1ms++;
	if(time_tick_1ms>8000&&time_tick_1ms<9000)
	{
		superDR_open();  		  //�ȴ�һ��ʱ������������ź�                                                                                                                                                                                                                                                  
	}
	if(time_tick_1ms>10000&&time_tick_1ms<10500)
	{
		MPUEX_Yaw_Init = MPUEX.angle.yaw;
	}
	if(time_tick_1ms > 10500) //��������10s���ϵ��ȶ�
	{
		if(time_tick_1ms%2 == 0)//��ʱ������2�Σ������if��Ҳ����2msˢ��һ��
		{
			//Remote_Task(); //ң������
			
			//GMControlLoop(); //��̨����
			
			//SMControlLoop(); //���ֿ���(�����ķ��ͺ�PIDд����̨������)
		}
		if(time_tick_1ms%4 == 0)//��ʱ������4�Σ������if��Ҳ����4msˢ��һ��
		{	
			 
			//CMControlLoop();//���̿���
		}
		if(time_tick_1ms%10 == 0)//��ʱ������20�Σ������if��Ҳ����20msˢ��һ��
		{	
//			data[0]=(u8)(((s16)MPUEX.angle.yaw)>>8);
//			data[1]=(u8)((s16)MPUEX.angle.yaw);
//			data[2]=(u8)(((s16) yaw_move) >> 8);
//			data[3]=(u8)((s16) yaw_move);
//			data[4]=(u8)(((s16)(Yaw_position_pid.output))>>8);
//			data[5]=(u8)((s16)(Yaw_position_pid.output));
//			data[6]=(u8)(((s16)(Yaw_position_pid.Kp * (Yaw_position_pid.e[2]))) >> 8);
//			data[7]=(u8)((s16)Yaw_position_pid.Kp * (Yaw_position_pid.e[2]));
//			data[8]=(u8)(((s16)Yaw_position_pid.intergral) >> 8);
//			data[9]=(u8)((s16)Yaw_position_pid.intergral);
//			
//			 my= Yaw_speed_pid.d_last;
//			data[10]=(u8)(  (my )    >> 8);
//			data[11]=(u8)( my );
//			data[12]=(u8)(((s16)Yaw_speed_pid.d_last) >> 8);
//			data[13]=(u8)((s16)Yaw_speed_pid.d_last);
//			data[14]=(u8)(((s16)InfantryJudge.Power) >> 8);
//			data[15]=(u8)((s16)InfantryJudge.Power);
//			Send_data8(data,16,1);
		}
		if(time_tick_1ms%500 == 0)
		{	
			Dog.RC_Receive=0; //ֹͣң����ι�������ң���������ݣ�ң������ι��
			Green=!Green;	
		}
		if(time_tick_1ms%3000 == 0)
		{	
			speed = -speed;
		}
	}
}

/**************���̵����������************/
//���̵Ŀ������񣬰�������⻷�������ڻ�����
//�⻷�����ʻ��ջ�����
//�ڻ����ٶȻ��ջ�����
//���ô���PID���ƣ���ң�����Ĳ���ֵ�ǹ��ʻ��������������ʻ�����������ٶȻ���������
//��ʵ���������һ�����������ƣ�����ǵ���ڲ���ɵģ�����������Ǵ���
//s16 CM1Speed,CM2Speed,CM3Speed,CM4Speed=0;//�����ĸ�������ٶ�ֵ������1�ţ�����2�ţ�����3�ţ�����4��
double vx;//����x����ٶ�ֵ��x����1��3���ƽ��
double vy;//����y����ٶ�ֵ��y����2��1���ƽ��
double zz;
double w0;//������ת���ٶ�ֵ
double vz;//������̨�ķ���
double K_Speed=16;//���岦�˸����ٶȷŴ����ֵ

s16 gyro_speed=0;//�����Ƿ��ص��ٶȲ���ֵ
u8  denfense_time_flag;//����Ť��������־λ
s16 denfense_time_count;//���̿�ʼŤ���ͼ���
s16 denfense_time_period=40;//����Ť��������=40*4*4=640ms��ͨ�����������������ٶȡ�
s16 yaw_bias_position=0;//PC���Ƶ�ʱ���ó������ƶ���̨1
float sumOfCurrent = 0;
void CMControlLoop(void)
{
	//chassis_electric();//���̵������,���в���ϵͳ����
	//*****����ٶ�ֵ����*****//

	if(denfense_mode==OPEN)
	{		
		if(denfense_time_count<=-2 * denfense_time_period)
			denfense_time_flag=1;
		if(denfense_time_count>=2 * denfense_time_period)
			denfense_time_flag=0;
		if(denfense_time_flag==1)
			denfense_time_count++;
		else
			denfense_time_count--;
	}
	else if(denfense_mode==CLOSE)
	{
		denfense_time_count=0;//��Ť���رյ�ʱ�򣬽�����ֵ��0����Ϊ��̨����ģʽ
	}	
	
	PID_Calc(&Gyro_normal_pid,Yaw_position_Feedback.real[0],1024*denfense_time_count/2/denfense_time_period+5078+yaw_bias_position,Incremental);
	//1024����̨���ƶ��Ƕ�ֵ���õ�yaw�����	������1024/22.75�ǽǶ�ֵ�������45�����ң������������90�㣬5078����̨����ǰ����ʱ�����ľ���λ��ֵ 
	
	Power_Limited();
	CM1Speed=(vx+vy+Gyro_normal_pid.output)*K_Speed ; 	//��3���ٶ�ֵ����4�����
	CM2Speed=(-vx+vy+Gyro_normal_pid.output)*K_Speed;
	CM3Speed=(vx-vy+Gyro_normal_pid.output*k2/k1)*K_Speed;
	CM4Speed=(-vx-vy+Gyro_normal_pid.output*k2/k1)*K_Speed;
/***********************************************************************************************/			
	PID_Calc(&CM1_speed_pid,CM1_Feedback.real[0],CM1Speed,Incremental);//�ĸ����̵����PID����
	PID_Calc(&CM2_speed_pid,CM2_Feedback.real[0],CM2Speed,Incremental);
	PID_Calc(&CM3_speed_pid,CM3_Feedback.real[0],CM3Speed,Incremental);
	PID_Calc(&CM4_speed_pid,CM4_Feedback.real[0],CM4Speed,Incremental);
	
	//BL  -  FR  -  BR  -FL
	Set_CM_Speed(CM1_speed_pid.output, CM2_speed_pid.output, CM3_speed_pid.output, CM4_speed_pid.output);//���͵���ֵ�����ֵ
}


/*************��̨��������*************/
//��̨���ô���PID���ٶȻ�Ϊ�ڻ���λ�û�Ϊ�⻷
//���Ե�ʱ��ر�λ�û������ٶȻ�����������Ϊ0
//��̨����������ͣ��ĳһλ�ã����߻��������ƶ���Ϊ�������
//s16 pitch_move,yaw_move;//���������λ����������ˮƽ�����λ��������
u32 shooting_time=0;
void GMControlLoop(void) 
{
//	if(minipc_mode==OPEN)
//	{
//		//pitch_move=-17600;        //������Ԥ���ķ���ֵ
//		//yaw_move=MPUEX_Yaw_Init-50;
//		//YAW 1730 1422  1115//PITCH-17740  -17690 -17640
//	}

		if(control_mode==PC)
	{

		
		pitch_move=PC_pitch_ref;//ȱ�ٿ�������   
		
	 if(MPUEX_Yaw_Init + PC_yaw_ref > 1800)
		PC_yaw_ref = -1800 - MPUEX_Yaw_Init;
	
		else if(MPUEX_Yaw_Init + PC_yaw_ref < -1800)
			
		 PC_yaw_ref = 1800 - MPUEX_Yaw_Init;		
		
		yaw_move=MPUEX_Yaw_Init+PC_yaw_ref;
	}
	else if(control_mode==RC)
	{
		/******��̨�������� ������ѡ��******/
		if(RC_pitch_ref < 3000)	
			RC_pitch_ref = 3000;
		else if(RC_pitch_ref > 3900)
			RC_pitch_ref = 3900;
		pitch_move =RC_pitch_ref;	//����ͨ�����˿���

		//��ֹ����ֵ��������������  ������YAW����Ϊ ��-1800�� 1800��
		if(MPUEX_Yaw_Init + RC_yaw_ref > 1800)
			RC_yaw_ref = -1800 - MPUEX_Yaw_Init;
		else if(MPUEX_Yaw_Init + RC_yaw_ref < -1800)
			RC_yaw_ref = 1800 - MPUEX_Yaw_Init;
		yaw_move=MPUEX_Yaw_Init + RC_yaw_ref; 
  }
	/*****���������*****/
	PID_Calc(&Yaw_position_pid, MPUEX.angle.yaw, yaw_move, Positional);
	PID_Calc(&Yaw_speed_pid, MPUEX.v.yaw,-Yaw_position_pid.output , Positional);
	/*
//	int my_yaw=RC_Ctl.rc.ch2;//yaw�ٶȻ�
//	my_yaw = my_yaw-1024;
//	PID_Calc(&Yaw_speed_pid, MPUEX.v.yaw, my_yaw, Positional); 
	*/
	//PID_Calc(&Yaw_resist_pid, MPUEX.v.yaw, 0, Positional); 

	PID_Calc(&Pitch_position_pid, Pitch_position_Feedback.real[0], pitch_move, Positional);
	PID_Calc(&Pitch_speed_pid, MPUEX.v.pitch, 0, Positional);			//������������2000����������ȽϺ�
	
	PID_Calc(&Shoot_position_pid,Shoot_position_Feedback.ecd_value,shooting_position,Positional);
	PID_Calc(&Shoot_speed_pid,Shoot_speed_Feedback.real[0], -Shoot_position_pid.output,Positional);
	//Set_Gimbal_Current(-Yaw_position_pid.output + Yaw_resist_pid.output,800 - Pitch_position_pid.output + Pitch_speed_pid.output, Shoot_speed_pid.output);

	Set_Gimbal_Current(Yaw_speed_pid.output,800 - Pitch_position_pid.output + Pitch_speed_pid.output, Shoot_speed_pid.output);
}

/*******���ֵ����������*********/
/*2018.5.9 zhu
Ŀǰʹ�ù̶��ķ�����ʱ   ��Ҫ����Ϊ����������
����ǣ��Ӳ���ϵͳ�����Ѿ����䣬Ȼ���÷�����Ч���ź�
*/
void SMControlLoop(void)
{
	cur_shooter_heat16=InfantryJudge.shooterHeat16mm;//�õ���ǰ����
	if(shoot_num==3 &&(max_heat16==90))
	  cur_shooter_heat16+=15*shoot_num;//30ֵ������Ҫ��������������Ŀǰ��Ϊ���ֵ30
  else //if(shoot_num==1)
	{
		cur_shooter_heat16+=30*shoot_num;
	}

	if( (shooting_flag==1) && (shooting_allow==2) && (cur_shooter_heat16<90) )//�յ������źţ�����������
	{
		if(shoot_num == 3)
			shooting_position -=3360;	//ת��һ���ӵ�������Ϊ��1125 3360 1140ƫ��
		else
		{
			shooting_position -=1125;
		}
		if(shooting_position < 0)	//�����Ƶ�λ��ֵ����һȦ��ʱ������
		{
			shooting_position += 15744;
		}
		shooting_allow=0;//��¼��ʱ����ʱ���־λ
	}			
	if(shooting_allow==0)				
	{
		shooting_time=time_tick_1ms;
		shooting_allow=1; //�ڹ�ȥ220ms�з�����ӵ�
	}
	else if(time_tick_1ms-shooting_time>220)//220ms��շ����ź�
	{
		shooting_flag=0;
		shooting_allow=2;
	}
}

void Power_Limited(void)
{
	VAL_LIMIT(InfantryJudge.PowerBuffer,(double)0,(double)60);
	K_Speed =10*power_data[(int)InfantryJudge.PowerBuffer];
}

/************Ħ�����뼤��**********/
void Friction_ON(int pwm_speed)//Ħ���ֿ��������⿪��
{
	Laser=OPEN;
	friction_flag=1;
	TIM_SetCompare1(TIM1,pwm_speed);			//68������ 30A���  55  10m/s����
	TIM_SetCompare2(TIM1,pwm_speed);
	TIM_SetCompare3(TIM1,pwm_speed);
	TIM_SetCompare4(TIM1,pwm_speed);
}

void Friction_OFF(void)
{
	Laser=CLOSE;
	friction_flag=0;
	TIM_SetCompare1(TIM1,50);
	TIM_SetCompare2(TIM1,50);
	TIM_SetCompare3(TIM1,50);
	TIM_SetCompare4(TIM1,50);
}
