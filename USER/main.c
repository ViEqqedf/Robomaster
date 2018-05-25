#include "main.h"


//燕山大学
//2018年RoboMasters
//YSU_RoboMaster实验室
//步兵程序
//作者：控制组
//在原有的基础上修改率YAW双环参数以及PID函数中判断进入死区之前的IF语句
//底盘控制修改了当S2=1时对输出电流的修改

int main(void)
{ 
	delay_init(180);    //初始化延时函数
	PID_SET_Init();//PID初始化
	BSP_Init();//硬件初始化
	delay_ms(500);
	Red=1;	
	shooting_position = 23;
	while(1)
	{	
		//IWDG_Feed();//喂狗
		if(RC_Ctl.rc.s2==3)
		{
			TIM_SetCompare1(TIM12,13000);	//修改比较值，修改占空比
			TIM_SetCompare2(TIM12,13000);	//修改比较值，修改占空比
		}
	
	}
}
