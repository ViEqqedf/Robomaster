#include "sys.h"

typedef struct
{
	u8 RC_Receive;
	u8 RC_Data;
	u8 IMU;
	u8 KeyBoard_Reset;
	u8 Flag;
} DogTypeDef;

extern DogTypeDef Dog;

void IWDG_Init(void);//IWDG³õÊ¼»¯
void IWDG_Feed(void);  //Î¹¹·º¯Êý
