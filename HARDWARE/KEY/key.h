#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 


#define WK_UP 	GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_10)	
#define WKUP_PRES   1

void KEY_Init(void);	//IO初始化
u8 KEY_Scan(u8);  		//按键扫描函数	

#endif
