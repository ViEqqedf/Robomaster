#include "iwdg.h"
#include "dbus.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//�������Ź� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//��ʼ���������Ź�
//prer:��Ƶ��:0~7(ֻ�е�3λ��Ч!)
//rlr:�Զ���װ��ֵ,0~0XFFF.
//��Ƶ����=4*2^prer.�����ֵֻ����256!
//rlr:��װ�ؼĴ���ֵ:��11λ��Ч.
//ʱ�����(���):Tout=((4*2^prer)*rlr)/32 (ms).

DogTypeDef Dog={0,1,1,1};


void IWDG_Init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //ʹ�ܶ�IWDG->PR IWDG->RLR��д
	
	IWDG_SetPrescaler(4); //����IWDG��Ƶϵ��

	IWDG_SetReload(500);   //����IWDGװ��ֵ

	IWDG_ReloadCounter(); //reload
	
	IWDG_Enable();       //ʹ�ܿ��Ź�
}

//ι�������Ź�
void IWDG_Feed(void)
{
		//ң��������Сֵ364�м�ֵ1024���ֵ1684
	if((RC_Ctl.rc.ch0>363||RC_Ctl.rc.ch1>363||RC_Ctl.rc.ch2>363||RC_Ctl.rc.ch3>363)&&(RC_Ctl.rc.ch0<1685||RC_Ctl.rc.ch1<1685||RC_Ctl.rc.ch2<1685||RC_Ctl.rc.ch3<1685))
	{
		Dog.Flag=(Dog.RC_Receive|Dog.RC_Data<<1|Dog.IMU<<2|Dog.KeyBoard_Reset<<3);
		if((Dog.Flag&0x0f)==0x0f)
		{
			IWDG_ReloadCounter();//reload
		}
	}
}
