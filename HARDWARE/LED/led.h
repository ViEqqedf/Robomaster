#ifndef __LED_H
#define __LED_H
#include "sys.h"

//LED端口定义
#define Green PFout(14)	// Green
#define Red PEout(7)	// Red

void LED_Init(void);//初始化		 				    
#endif
