#include "main.h"

void BSP_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组2
	LED_Init();						//初始化LED 
	KEY_Init(); 					//按键初始化  
	Laser_Init();       			//激光初始化
	CAN1_Init(); 					//底盘电机 云台电机 拨轮电机通信协议初始化
	DBUS_Init();        			//遥控器初始化     
	Referee_Init();
	IMU_EX_Init();   		   		//外部陀螺仪初始化
	superDR_init();					//超级电容对应GPIO初始化
	
	TIM3_Init();        			//定时器中断程序初始化
	TIM1_Init();					//摩擦轮初始化``			
	TIM_SetCompare1(TIM12,13000);	//修改比较值，修改占空比
	TIM_SetCompare2(TIM12,13000);	//修改比较值，修改占空比
	TIM_SetCompare1(TIM1,50);
	TIM_SetCompare2(TIM1,50);
	TIM_SetCompare3(TIM1,50);
	TIM_SetCompare4(TIM1,50);
	Usart2_init(115200);
}
 
void PID_SET_Init(void)
{													//												     3            2              1
	//void PID_Init(PidTypeDef * pid,double kp,double ki,double kd,double ka,double max_out,double dead_band,double i_band,double max_input)
	PID_Init(&CM1_speed_pid,10,0.7,0,0,4000,100,0,6000); 	//1号电机速度环
	PID_Init(&CM2_speed_pid,10,0.7,0,0,4000,100,0,6000);
	PID_Init(&CM3_speed_pid,10,0.7,0,0,4000,100,0,6000);
	PID_Init(&CM4_speed_pid,10,0.7,0,0,4000,100,0,6000);
	PID_Init(&Gyro_normal_pid,0.40,0,0,0,2000,10,0,6000); 	//底盘旋转环
	PID_Init(&Power_limit_pid,0.8,0,0,0, 50,0,0,8192); 	//功率限制环
//ren   pos yaw 6.5 0.08  9
	
	 //一个比较好的pid   yaw loc_pid 6 4 0(8)
	//加i的  pid speed 1.4 0 5 loc 9 0.03 8
	PID_Init(&Yaw_speed_pid, 1.5, 0,2, 0, 4500, 20, 0,15000);			//输出在1500左右
	PID_Init(&Yaw_position_pid,5, 0.01,0, 0, 15000, 0, 600, 1800);	// 26 1.4
	//yaw pos  2018.4.30  
	//5.5  p 7.5等幅震荡
	PID_Init(&Yaw_resist_pid, 2.3, 0, 80, 0, 15000, 5, 250, 1800);	// 26 1.4
  //2018.5.12  yaw loc pi 5 0.01 4 zhu
	PID_Init(&Pitch_speed_pid, 0.5, 0, 0, 0, 4000, 50, 400, 6000);		//
	PID_Init(&Pitch_position_pid, 5, 0, 30, 0, 4500, 0, 300, 6200);	 	//  单环 P5  D60

	PID_Init(&Shoot_position_pid,4, 0, 2, 0, 12000, 100, 0, 15744);	
	PID_Init(&Shoot_speed_pid,2, 6, 4,0,2000,100,0,12000);				
}