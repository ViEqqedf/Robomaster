#include "laser.h" 


void Laser_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG ,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13  ;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
		
	GPIO_SetBits(GPIOG , GPIO_Pin_13 );

}




