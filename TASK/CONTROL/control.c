#include "main.h"
#include "Remote.h"

 uint32_t time_tick_1ms = 5000;//原本是static变量
s16 pitch_move = 3320;
s16 yaw_move;//俯仰电机的位置输入量，水平电机的位置输入量
s16 CM1Speed,CM2Speed,CM3Speed,CM4Speed=0;//定义四个电机的速度值，左上1号，右上2号，左下3号，右下4号
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
};  //功率限制函数
//定时器中断触发的控制任务
uint16_t my;
void Control_Task(void)
{
	time_tick_1ms++;
	if(time_tick_1ms>8000&&time_tick_1ms<9000)
	{
		superDR_open();  		  //等待一段时间给超级电容信号                                                                                                                                                                                                                                                  
	}
	if(time_tick_1ms>10000&&time_tick_1ms<10500)
	{
		MPUEX_Yaw_Init = MPUEX.angle.yaw;
	}
	if(time_tick_1ms > 10500) //陀螺仪有10s的上电稳定
	{
		if(time_tick_1ms%2 == 0)//定时器触发2次，进这个if，也就是2ms刷新一次
		{
			//Remote_Task(); //遥控任务
			
			//GMControlLoop(); //云台控制
			
			//SMControlLoop(); //拨轮控制(电流的发送和PID写在云台控制里)
		}
		if(time_tick_1ms%4 == 0)//定时器触发4次，进这个if，也就是4ms刷新一次
		{	
			 
			//CMControlLoop();//底盘控制
		}
		if(time_tick_1ms%10 == 0)//定时器触发20次，进这个if，也就是20ms刷新一次
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
			Dog.RC_Receive=0; //停止遥控器喂狗，如果遥控器有数据，遥控器会喂狗
			Green=!Green;	
		}
		if(time_tick_1ms%3000 == 0)
		{	
			speed = -speed;
		}
	}
}

/**************底盘电机控制任务************/
//底盘的控制任务，包括电机外环控制与内环控制
//外环：功率环闭环控制
//内环：速度环闭环控制
//采用串行PID控制，即遥控器的拨杆值是功率环的输入量，功率环的输出量是速度环的输入量
//其实电调本身还有一个电流环控制，这个是电调内部完成的，这个不用我们处理
//s16 CM1Speed,CM2Speed,CM3Speed,CM4Speed=0;//定义四个电机的速度值，左上1号，右上2号，左下3号，右下4号
double vx;//定义x向的速度值，x向与1、3电机平行
double vy;//定义y向的速度值，y向与2、1电机平行
double zz;
double w0;//定义旋转的速度值
double vz;//俯仰云台的方向
double K_Speed=16;//定义拨杆给定速度放大比例值

s16 gyro_speed=0;//陀螺仪返回的速度补偿值
u8  denfense_time_flag;//底盘扭腰计数标志位
s16 denfense_time_count;//底盘开始扭腰就计数
s16 denfense_time_period=40;//底盘扭腰的周期=40*4*4=640ms，通过控制周期来控制速度。
s16 yaw_bias_position=0;//PC控制的时候，让车身不动移动云台1
float sumOfCurrent = 0;
void CMControlLoop(void)
{
	//chassis_electric();//底盘电流检测,等有裁判系统启用
	//*****电机速度值计算*****//

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
		denfense_time_count=0;//当扭腰关闭的时候，将输入值置0，即为云台跟随模式
	}	
	
	PID_Calc(&Gyro_normal_pid,Yaw_position_Feedback.real[0],1024*denfense_time_count/2/denfense_time_period+5078+yaw_bias_position,Incremental);
	//1024是云台的移动角度值，用的yaw电机的	反馈，1024/22.75是角度值，大概是45°左右，两个方向就是90°，5078是云台在正前方的时候电机的绝对位置值 
	
	Power_Limited();
	CM1Speed=(vx+vy+Gyro_normal_pid.output)*K_Speed ; 	//将3向速度值赋到4个电机
	CM2Speed=(-vx+vy+Gyro_normal_pid.output)*K_Speed;
	CM3Speed=(vx-vy+Gyro_normal_pid.output*k2/k1)*K_Speed;
	CM4Speed=(-vx-vy+Gyro_normal_pid.output*k2/k1)*K_Speed;
/***********************************************************************************************/			
	PID_Calc(&CM1_speed_pid,CM1_Feedback.real[0],CM1Speed,Incremental);//四个底盘电机的PID计算
	PID_Calc(&CM2_speed_pid,CM2_Feedback.real[0],CM2Speed,Incremental);
	PID_Calc(&CM3_speed_pid,CM3_Feedback.real[0],CM3Speed,Incremental);
	PID_Calc(&CM4_speed_pid,CM4_Feedback.real[0],CM4Speed,Incremental);
	
	//BL  -  FR  -  BR  -FL
	Set_CM_Speed(CM1_speed_pid.output, CM2_speed_pid.output, CM3_speed_pid.output, CM4_speed_pid.output);//发送电流值给电调值
}


/*************云台控制任务*************/
//云台采用串行PID，速度环为内环，位置环为外环
//调试的时候关闭位置环，即速度环的输入设置为0
//云台可以任意悬停在某一位置，或者缓慢匀速移动即为调试完成
//s16 pitch_move,yaw_move;//俯仰电机的位置输入量，水平电机的位置输入量
u32 shooting_time=0;
void GMControlLoop(void) 
{
//	if(minipc_mode==OPEN)
//	{
//		//pitch_move=-17600;        //给妙算预留的返回值
//		//yaw_move=MPUEX_Yaw_Init-50;
//		//YAW 1730 1422  1115//PITCH-17740  -17690 -17640
//	}

		if(control_mode==PC)
	{

		
		pitch_move=PC_pitch_ref;//缺少控制增量   
		
	 if(MPUEX_Yaw_Init + PC_yaw_ref > 1800)
		PC_yaw_ref = -1800 - MPUEX_Yaw_Init;
	
		else if(MPUEX_Yaw_Init + PC_yaw_ref < -1800)
			
		 PC_yaw_ref = 1800 - MPUEX_Yaw_Init;		
		
		yaw_move=MPUEX_Yaw_Init+PC_yaw_ref;
	}
	else if(control_mode==RC)
	{
		/******云台的输入量 反馈量选择******/
		if(RC_pitch_ref < 3000)	
			RC_pitch_ref = 3000;
		else if(RC_pitch_ref > 3900)
			RC_pitch_ref = 3900;
		pitch_move =RC_pitch_ref;	//俯仰通过拨杆控制

		//防止给定值超过陀螺仪量程  陀螺仪YAW量程为 （-1800， 1800）
		if(MPUEX_Yaw_Init + RC_yaw_ref > 1800)
			RC_yaw_ref = -1800 - MPUEX_Yaw_Init;
		else if(MPUEX_Yaw_Init + RC_yaw_ref < -1800)
			RC_yaw_ref = 1800 - MPUEX_Yaw_Init;
		yaw_move=MPUEX_Yaw_Init + RC_yaw_ref; 
  }
	/*****到这里结束*****/
	PID_Calc(&Yaw_position_pid, MPUEX.angle.yaw, yaw_move, Positional);
	PID_Calc(&Yaw_speed_pid, MPUEX.v.yaw,-Yaw_position_pid.output , Positional);
	/*
//	int my_yaw=RC_Ctl.rc.ch2;//yaw速度环
//	my_yaw = my_yaw-1024;
//	PID_Calc(&Yaw_speed_pid, MPUEX.v.yaw, my_yaw, Positional); 
	*/
	//PID_Calc(&Yaw_resist_pid, MPUEX.v.yaw, 0, Positional); 

	PID_Calc(&Pitch_position_pid, Pitch_position_Feedback.real[0], pitch_move, Positional);
	PID_Calc(&Pitch_speed_pid, MPUEX.v.pitch, 0, Positional);			//输入量控制在2000这个数量级比较好
	
	PID_Calc(&Shoot_position_pid,Shoot_position_Feedback.ecd_value,shooting_position,Positional);
	PID_Calc(&Shoot_speed_pid,Shoot_speed_Feedback.real[0], -Shoot_position_pid.output,Positional);
	//Set_Gimbal_Current(-Yaw_position_pid.output + Yaw_resist_pid.output,800 - Pitch_position_pid.output + Pitch_speed_pid.output, Shoot_speed_pid.output);

	Set_Gimbal_Current(Yaw_speed_pid.output,800 - Pitch_position_pid.output + Pitch_speed_pid.output, Shoot_speed_pid.output);
}

/*******拨轮电机控制任务*********/
/*2018.5.9 zhu
目前使用固定的发射延时   主要是因为鼠标连续点击
最好是，从裁判系统读出已经发射，然后置发射有效的信号
*/
void SMControlLoop(void)
{
	cur_shooter_heat16=InfantryJudge.shooterHeat16mm;//得到当前热量
	if(shoot_num==3 &&(max_heat16==90))
	  cur_shooter_heat16+=15*shoot_num;//30值可能需要根据射速来定，目前设为最大值30
  else //if(shoot_num==1)
	{
		cur_shooter_heat16+=30*shoot_num;
	}

	if( (shooting_flag==1) && (shooting_allow==2) && (cur_shooter_heat16<90) )//收到发射信号，并且允许发射
	{
		if(shoot_num == 3)
			shooting_position -=3360;	//转动一颗子弹的增量为：1125 3360 1140偏大
		else
		{
			shooting_position -=1125;
		}
		if(shooting_position < 0)	//当控制的位置值超过一圈的时候清零
		{
			shooting_position += 15744;
		}
		shooting_allow=0;//记录此时发射时间标志位
	}			
	if(shooting_allow==0)				
	{
		shooting_time=time_tick_1ms;
		shooting_allow=1; //在过去220ms中发射过子弹
	}
	else if(time_tick_1ms-shooting_time>220)//220ms清空发射信号
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

/************摩擦轮与激光**********/
void Friction_ON(int pwm_speed)//摩擦轮开启，激光开启
{
	Laser=OPEN;
	friction_flag=1;
	TIM_SetCompare1(TIM1,pwm_speed);			//68超热量 30A电调  55  10m/s左右
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
