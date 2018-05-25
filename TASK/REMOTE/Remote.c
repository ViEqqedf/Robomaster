#include "main.h"
//ң�����ұ߲���S2��ֵ����1 ��3 ��2
//�����˴����Ϸ�ʱ��Ϊң��������
//          ��λʱ�������κο���
//          �·�ʱ��Ϊ����������

u8 friction_speed=Friction_speed2;
Keyboard_t Keyboard;
u8 denfense_mode=0;//����ģʽ�Ķ��壬0Ϊ����ģʽ��1Ϊҡͷģʽ��
u8 minipc_mode=0;//��̨ģʽ���壬0Ϊ�ֶ�ģʽ��1Ϊ���Զ�ģʽ��
u8 buff_mode=0;//0Ϊ�Ǵ��ģʽ��1Ϊ���ģʽ��
u8 shooting_allow=0;//�����䣨�����ж����η������Ƿ�̫�̣�
s16 shooting_position=0;//���ֵ��λ��ֵ
u8 control_mode=0;//�ж�ң��������(0)����PC����(1)
float PC_pitch_ref=3320;
float PC_yaw_ref=0;
float RC_yaw_ref=0;
float RC_pitch_ref=3320;
u8 flag = 1;
s32 shootSpeed = 0;
extern s16 pitch_move;
u8 Remote_run_flag = 0;
u8 shoot_num = 1;
/********ң�����������Ҳದ��ִ��**********/
void Remote_Task(void)//��������
{
	if(RC_Ctl.rc.s2==1)//��
	{
	    control_mode=RC;
		denfense_mode = CLOSE;
	}
	else if(RC_Ctl.rc.s2==2)//��
	{
		control_mode = PC;
		denfense_mode = CLOSE;
	}
	else if(RC_Ctl.rc.s2 == 3)//��
	{
		denfense_mode = OPEN;
	}
	
	if(control_mode == PC)
		PC_control();
	else if(control_mode == RC)
		RC_control();
}

/**************ң������������**********/
u8 Last_s1=0;//��һ�β���ֵ�ô���
u8 friction_flag=0;//Ħ���ֿ���״̬��־��1Ϊ������0Ϊ�ر�
u8 shooting_flag=0;//��������״̬��־��1Ϊ�������0.22S�ڷ�������裬0Ϊδ�����
void RC_control(void)//ң��������
{
	//��ֹң����û�ӳ�����
	if(flag == 1 && RC_Ctl.rc.ch0 == 0 && RC_Ctl.rc.ch1 == 0 && RC_Ctl.rc.ch2 == 0 && RC_Ctl.rc.ch3 == 0){
		vz = 0;
		w0 = 0;
		return;
	}
	
	flag = 2;
	vx=RC_Ctl.rc.ch1-1024;//��������λ����Ϊ�ٶȵ���ֵ
	vy=RC_Ctl.rc.ch0-1026;//1.1024
												//2.1060
												//3.1014
												//4.1020
												//5.1030
												//6.1027
												//7.1026
												//8.1027
												//9.1026 2018.03.11 21:03 ������

	w0=-(RC_Ctl.rc.ch2-1024)*0.0025;//��Ϊ��һ���ۼӵĹ��̣����Խ�����
	vz=(RC_Ctl.rc.ch3-1024)*0.0025;

	/*****ˮƽ��ֵ̨����*****/
	if(RC_Ctl.rc.ch2-1024 > 50 || RC_Ctl.rc.ch2-1024 < -50)
		RC_yaw_ref += w0;//�ۼ��ƶ�ֵ
	
	if(RC_Ctl.rc.ch3-1024 > 50 || RC_Ctl.rc.ch3-1024 < -50)
		RC_pitch_ref += vz;
  
	/*****��ದ��������*****/
	switch(RC_Ctl.rc.s1)
	{
		case 1:
			if(friction_flag==0&&Last_s1==3)
			{
				Friction_ON(friction_speed);//Ħ���ֿ��������⿪��
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
/*******���Կ�������********/
double K_stall=1;//shift��ctrl�ĵ�λ��
void PC_control(void)//���Կ���
{
	if(denfense_mode==CLOSE)//�жϵ���ģʽ
	{
		chassis_PC_control();
	}//���Կ��Ƶ���
	else
	{
		K_stall=1;
	}
	mouse_PC_control();//��̨���ơ��Զ���׼�л�����������
	//reset_PC_control();//��λ�����⡢Ħ���ֵ��Կ���
	mode_PC_control();//ģʽ����
}

u16 K_stall_closeDelayCount=0;
void chassis_PC_control(void)//���Կ��Ƶ��̵��
{
	

		if((RC_Ctl.key.v&KEY_Q) == KEY_Q)//Q  ������
			friction_speed=Friction_speed1;
			
		else if((RC_Ctl.key.v&KEY_E) == KEY_E)//e ������
			;//friction_speed=  ;
		else if((RC_Ctl.key.v&KEY_W) == KEY_W)//Wǰ��
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
//	if(Keyboard.stall==16)//����
//	{
//		K_stall_closeDelayCount++;
//		if(K_stall_closeDelayCount==25)
//		{
//		K_stall+=0.1;
//		K_stall_closeDelayCount=0;
//		}
//	}
//	else if(Keyboard.stall==32)//����
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
/**********��λ����************/
u16 friction_closeDelayCount=0;
u16 PC_Dog_closeDelayCount=0;
//void reset_PC_control(void)//�ֶ���λ ���� Ħ����
//{
//	Keyboard.reset=RC_Ctl.key.v_h&0x38;
//	switch(Keyboard.reset)
//	{
//		case 8://����Z����ʱû��
//			break;
//		case 16://Ħ����+���⿪
//			Friction_ON(friction_speed);
//			break;
//		case 32://Ħ����+�����
//			friction_closeDelayCount++;
//			if(friction_closeDelayCount>500)
//			{
//				Friction_OFF();
//			}
//			break;
//		case 56://�ֶ���λ
//			PC_Dog_closeDelayCount++;
//			if(PC_Dog_closeDelayCount>500)
//			{
//				Dog.KeyBoard_Reset=0;//ֹͣι��
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

//	/*****�Ҽ��������Զ���׼ģʽ*****/
//	if(RC_Ctl.mouse.press_r== 1)
//		{minipc_mode=OPEN;}
//	if(RC_Ctl.mouse.press_r== 0)
//		{mini_closeDelayCount++;}
//	else
//		{mini_closeDelayCount = 0;}	
//	if(mini_closeDelayCount>50)   //�ر�Ħ����
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
			Friction_ON(friction_speed);//Ħ���ֿ��������⿪��
			friction_flag = 1;
			fangdou_time=time_tick_1ms;
		}
	  else if( (friction_flag==1) &&(RC_Ctl.mouse.press_r==1)&&(fangdou_cur_time-fangdou_time>1000))
		{
			   Friction_OFF();//Ħ���ֹرգ�����ر�
				 friction_flag = 0;
			   fangdou_time=time_tick_1ms;
		}
		fangdou_cur_time=time_tick_1ms;
	/*****�������*****/
	if(RC_Ctl.mouse.press_l== 1&&friction_flag==1)
	{
		//��������д������
		shooting_flag=1;
	}
	
	/*****�����ƶ�������ˮƽ����������ƶ�������Ƹ������*****/
	
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
		/*****ˮƽ��ֵ̨����*****/
	  w0=-(RC_Ctl.mouse.x)*0.02;
	  PC_yaw_ref+=w0;//�ۼ��ƶ�ֵ
	}
 
 else //if((RC_Ctl.mouse.x>10)||(RC_Ctl.mouse.x<-10))	
 {
	  //PC_yaw_ref += (-RC_Ctl.mouse.x)*0.025;//0.25
		/*****ˮƽ��ֵ̨����*****/
	  w0=-(RC_Ctl.mouse.x)*0.07;
	  PC_yaw_ref+=w0;//�ۼ��ƶ�ֵ
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
				
				//Ħ�����µ�
			}
		}
		
}
