#include "superDR.h"

void superDR_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//ʹ��ʱ��	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	//���ó�ʼ���ṹ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��

}


void superDR_open()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_5);
}