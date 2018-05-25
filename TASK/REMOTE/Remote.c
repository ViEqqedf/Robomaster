#include "main.h"
//遥控器右边拨杆S2的值，上1 中3 下2
//当拨杆处于上方时，为遥控器控制
//          中位时，不做任何控制
//          下方时，为键盘鼠标控制

u8 friction_speed=Friction_speed2;
Keyboard_t Keyboard;
u8 denfense_mode=0;//底盘模式的定义，0为正常模式，1为摇头模式。
u8 minipc_mode=0;//云台模式定义，0为手动模式，1为半自动模式。
u8 buff_mode=0;//0为非大符模式，1为大符模式。
u8 shooting_allow=0;//允许发射（用来判断两次发射间隔是否太短）
s16 shooting_position=0;//拨轮电机位置值
u8 control_mode=0;//判断遥控器控制(0)还是PC控制(1)
float PC_pitch_ref=3320;
float PC_yaw_ref=0;
float RC_yaw_ref=0;
float RC_pitch_ref=3320;
u8 flag = 1;
s32 shootSpeed = 0;
extern s16 pitch_move;
u8 Remote_run_flag = 0;
u8 shoot_num = 1;
/********遥控任务处理，由右侧拨杆执行**********/
void Remote_Task(void)//控制任务
{
	if(RC_Ctl.rc.s2==1)//上
	{
	    control_mode=RC;
		denfense_mode = CLOSE;
	}
	else if(RC_Ctl.rc.s2==2)//下
	{
		control_mode = PC;
		denfense_mode = CLOSE;
	}
	else if(RC_Ctl.rc.s2 == 3)//中
	{
		denfense_mode = OPEN;
	}
	
	if(control_mode == PC)
		PC_control();
	else if(control_mode == RC)
		RC_control();
}

/**************遥控器控制任务**********/
u8 Last_s1=0;//上一次拨杆值得储存
u8 friction_flag=0;//摩擦轮开启状态标志，1为开启，0为关闭
u8 shooting_flag=0;//发射任务状态标志，1为在最近的0.22S内发射过弹丸，0为未发射过
void RC_control(void)//遥控器控制
{
	//防止遥控器没接车乱跑
	if(flag == 1 && RC_Ctl.rc.ch0 == 0 && RC_Ctl.rc.ch1 == 0 && RC_Ctl.rc.ch2 == 0 && RC_Ctl.rc.ch3 == 0){
		vz = 0;
		w0 = 0;
		return;
	}
	
	flag = 2;
	vx=RC_Ctl.rc.ch1-1024;//将拨杆中位定义为速度的零值
	vy=RC_Ctl.rc.ch0-1026;//1.1024
												//2.1060
												//3.1014
												//4.1020
												//5.1030
												//6.1027
												//7.1026
												//8.1027
												//9.1026 2018.03.11 21:03 游三正

	w0=-(RC_Ctl.rc.ch2-1024)*0.0025;//因为是一个累加的过程，所以降低数
	vz=(RC_Ctl.rc.ch3-1024)*0.0025;

	/*****水平云台值处理*****/
	if(RC_Ctl.rc.ch2-1024 > 50 || RC_Ctl.rc.ch2-1024 < -50)
		RC_yaw_ref += w0;//累加移动值
	
	if(RC_Ctl.rc.ch3-1024 > 50 || RC_Ctl.rc.ch3-1024 < -50)
		RC_pitch_ref += vz;
  
	/*****左侧拨杆任务处理*****/
	switch(RC_Ctl.rc.s1)
	{
		case 1:
			if(friction_flag==0&&Last_s1==3)
			{
				Friction_ON(friction_speed);//摩擦轮开启，激光开启
				friction_flag = 1;
			}
			
			else if(friction_flag==1&&Last_s1==3)
			{
				Friction_OFF();
				friction_flag = 0;
			}
			
			Last_s1=1;
			break;
			
		case 3:
			Last_s1=3;
			shootSpeed = 0;
			break;
		case 2:		
			if(Last_s1==3 && friction_flag==1)	
			{
				shooting_flag=1;
				shootSpeed = 4000;
				Last_s1=2;
			}
			break;
		default:
			break;
	}
	

}
/*******电脑控制任务********/
double K_stall=1;//shift和ctrl的档位数
void PC_control(void)//电脑控制
{
	if(denfense_mode==CLOSE)//判断底盘模式
	{
		chassis_PC_control();
	}//电脑控制底盘
	else
	{
		K_stall=1;
	}
	mouse_PC_control();//云台控制、自动瞄准切换、发射任务
	//reset_PC_control();//复位、激光、摩擦轮电脑控制
	mode_PC_control();//模式控制
}

u16 K_stall_closeDelayCount=0;
void chassis_PC_control(void)//电脑控制底盘电机
{
	

		if((RC_Ctl.key.v&KEY_Q) == KEY_Q)//Q  低射速
			friction_speed=Friction_speed1;
			
		else if((RC_Ctl.key.v&KEY_E) == KEY_E)//e 高射速
			;//friction_speed=  ;
		else if((RC_Ctl.key.v&KEY_W) == KEY_W)//W前进
		{
			vx+=0.8;
			vy*=0.995;
		}
		//case 65://WQ
		//	break;
		//case 129://WE
		//	break;
		else if((RC_Ctl.key.v&KEY_S) == KEY_S)//S
		{
			vx-=0.8;
			vy*=0.995;
		}
		//case 66://SQ
		//	break;
		//case 130://SE
		//	break;
		else if((RC_Ctl.key.v&KEY_A) == KEY_A)//A
		{
			vx*=0.995;
			vy-=0.8;
		}
		else if((RC_Ctl.key.v&KEY_W) == KEY_W && ((RC_Ctl.key.v&KEY_A) == KEY_A) )//WA
		{
			vx+=0.8;
			vy-=0.8;
		}
	
		else if((RC_Ctl.key.v&KEY_S) == KEY_S && ((RC_Ctl.key.v&KEY_A) == KEY_A) )//SA
		{
			vx-=0.8;
			vy-=0.8;
	
		}
	else if((RC_Ctl.key.v&KEY_D) == KEY_D  )//D
		{
			vx*=0.995;
			vy+=0.8;
		}
		else if((RC_Ctl.key.v&KEY_W) == KEY_W && ((RC_Ctl.key.v&KEY_A) == KEY_A) )
		{//WD
			vx+=0.8;
			vy+=0.8;
		}
		else if((RC_Ctl.key.v&KEY_S) == KEY_S && ((RC_Ctl.key.v&KEY_D) == KEY_D) )//SD
		{	vx-=0.8;
			vy+=0.8;
		}
		else
		{
			vx*=0.995;
			vy*=0.995;
		}
	
	VAL_LIMIT(vx, -1000*K_stall, 1000*K_stall); 
	VAL_LIMIT(vy, -1000*K_stall, 1000*K_stall); 	
	
//	Keyboard.stall=RC_Ctl.key.v_l&0x30;
//	if(Keyboard.stall==16)//升档
//	{
//		K_stall_closeDelayCount++;
//		if(K_stall_closeDelayCount==25)
//		{
//		K_stall+=0.1;
//		K_stall_closeDelayCount=0;
//		}
//	}
//	else if(Keyboard.stall==32)//降档
//	{
//		K_stall_closeDelayCount++;
//		if(K_stall_closeDelayCount==25)
//		{
//		K_stall-=0.1;
//		K_stall_closeDelayCount=0;
//		}
//	}
//	else
//	{K_stall_closeDelayCount=0;}
//	VAL_LIMIT(K_stall,1,7.5);
}
/**********复位任务************/
u16 friction_closeDelayCount=0;
u16 PC_Dog_closeDelayCount=0;
//void reset_PC_control(void)//手动复位 激光 摩擦轮
//{
//	Keyboard.reset=RC_Ctl.key.v_h&0x38;
//	switch(Keyboard.reset)
//	{
//		case 8://键盘Z，暂时没用
//			break;
//		case 16://摩擦轮+激光开
//			Friction_ON(friction_speed);
//			break;
//		case 32://摩擦轮+激光关
//			friction_closeDelayCount++;
//			if(friction_closeDelayCount>500)
//			{
//				Friction_OFF();
//			}
//			break;
//		case 56://手动复位
//			PC_Dog_closeDelayCount++;
//			if(PC_Dog_closeDelayCount>500)
//			{
//				Dog.KeyBoard_Reset=0;//停止喂狗
//			}
//			break;
//		default:
//			friction_closeDelayCount=0;
//			PC_Dog_closeDelayCount=0;
//			break;
//	}
//}



u16 mini_closeDelayCount=0;
uint32_t fangdou_time=100;
uint32_t fangdou_cur_time=0;

void mouse_PC_control(void)
{ 

//	/*****右键开启半自动瞄准模式*****/
//	if(RC_Ctl.mouse.press_r== 1)
//		{minipc_mode=OPEN;}
//	if(RC_Ctl.mouse.press_r== 0)
//		{mini_closeDelayCount++;}
//	else
//		{mini_closeDelayCount = 0;}	
//	if(mini_closeDelayCount>50)   //关闭摩擦轮
//		{minipc_mode=CLOSE;}	
	
//	if( (RC_Ctl.key.v_h&0xc7)==1)
//	{
//		minipc_mode=OPEN;
//	}
//	else 
//	{
//		minipc_mode=CLOSE;
//	}
	 PC_pitch_ref=pitch_move;
	

	if( (friction_flag==0)&&(RC_Ctl.mouse.press_r==1)&&(fangdou_cur_time-fangdou_time>1000))
		{
			Friction_ON(friction_speed);//摩擦轮开启，激光开启
			friction_flag = 1;
			fangdou_time=time_tick_1ms;
		}
	  else if( (friction_flag==1) &&(RC_Ctl.mouse.press_r==1)&&(fangdou_cur_time-fangdou_time>1000))
		{
			   Friction_OFF();//摩擦轮关闭，激光关闭
				 friction_flag = 0;
			   fangdou_time=time_tick_1ms;
		}
		fangdou_cur_time=time_tick_1ms;
	/*****左键点射*****/
	if(RC_Ctl.mouse.press_l== 1&&friction_flag==1)
	{
		//发射任务写在这里
		shooting_flag=1;
	}
	
	/*****左右移动鼠标控制水平电机，上下移动电机控制俯仰电机*****/
	
	VAL_LIMIT(RC_Ctl.mouse.x, -150, 150); 
	VAL_LIMIT(RC_Ctl.mouse.y, -150, 150); 
	if( RC_Ctl.mouse.y<=15 &&(RC_Ctl.mouse.y>=0))
	{
		PC_pitch_ref += (-RC_Ctl.mouse.y)*0.03;
	}
	else if( (RC_Ctl.mouse.y>-15) && (RC_Ctl.mouse.y<0))
	{
		PC_pitch_ref += (-RC_Ctl.mouse.y)*0.2;
	}
	else 
	{
		PC_pitch_ref += (-RC_Ctl.mouse.y)*0.13;
	}
	VAL_LIMIT(PC_pitch_ref, 3100, 3900);
	if((RC_Ctl.mouse.x<15)&&(RC_Ctl.mouse.x>-15))	
 {
	  //PC_yaw_ref += (-RC_Ctl.mouse.x)*0.025;//0.25
		/*****水平云台值处理*****/
	  w0=-(RC_Ctl.mouse.x)*0.02;
	  PC_yaw_ref+=w0;//累加移动值
	}
 
 else //if((RC_Ctl.mouse.x>10)||(RC_Ctl.mouse.x<-10))	
 {
	  //PC_yaw_ref += (-RC_Ctl.mouse.x)*0.025;//0.25
		/*****水平云台值处理*****/
	  w0=-(RC_Ctl.mouse.x)*0.07;
	  PC_yaw_ref+=w0;//累加移动值
	}
//	if(PC_yaw_ref>1800)
//	{PC_yaw_ref=1800;}
//	if(PC_yaw_ref<-1800)
//	{PC_yaw_ref=-1800;}
}

void mode_PC_control(void)
{
		if((RC_Ctl.key.v&KEY_R) == KEY_R)//R
		{
			
		}
		if((RC_Ctl.key.v&KEY_F) == KEY_F)//F
		{
			denfense_mode=OPEN;
		}
		if((RC_Ctl.key.v&KEY_G) == KEY_G)//G
		{
			
			//denfense_mode=CLOSE;
		}
		if((RC_Ctl.key.v&KEY_Z) == KEY_Z)//Z
		{
			shoot_num = 1;
			friction_speed=Friction_speed2;
		}
		if((RC_Ctl.key.v&KEY_X) == KEY_X)//X
		{
			shoot_num = 3;
			friction_speed=Friction_speed1;
		}
		if((RC_Ctl.key.v&KEY_C) == KEY_C)//C
		{
			shoot_num = 3;
			friction_speed=Friction_speed2;
		}
		if((RC_Ctl.key.v&KEY_V) == KEY_V)//V
		{
			
		}
		if((RC_Ctl.key.v&KEY_B) == KEY_B)//B
		{
			buff_mode=CLOSE;
			if(RC_Ctl.rc.s1==1)
			{
				
				//摩擦轮下调
			}
		}
		
}
