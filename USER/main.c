#include "main.h"


//��ɽ��ѧ
//2018��RoboMasters
//YSU_RoboMasterʵ����
//��������
//���ߣ�������
//��ԭ�еĻ������޸���YAW˫�������Լ�PID�������жϽ�������֮ǰ��IF���
//���̿����޸��˵�S2=1ʱ������������޸�

int main(void)
{ 
	delay_init(180);    //��ʼ����ʱ����
	PID_SET_Init();//PID��ʼ��
	BSP_Init();//Ӳ����ʼ��
	delay_ms(500);
	Red=1;	
	shooting_position = 23;
	while(1)
	{	
		//IWDG_Feed();//ι��
		if(RC_Ctl.rc.s2==3)
		{
			TIM_SetCompare1(TIM12,13000);	//�޸ıȽ�ֵ���޸�ռ�ձ�
			TIM_SetCompare2(TIM12,13000);	//�޸ıȽ�ֵ���޸�ռ�ձ�
		}
	
	}
}
