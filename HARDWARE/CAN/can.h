#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    
typedef struct
{
	s16 real[8];
	u8  count;
	s16 round_cnt;
	s16 ecd_value;
	s16 sum;
	s16 bias;
	s16 calc;
} ReceiveTypeDef;

					    						    
										 							 				    
void CAN1_Init(void);//CAN初始化
void Set_CM_Speed(s16 cm1_iq,s16 cm2_iq,s16 cm3_iq,s16 cm4_iq);		//发送数据
void Set_Gimbal_Current(s16 gimbal_yaw_iq, s16 gimbal_pitch_iq,s16 shoot_iq);
void Motor_Data_Receive(CanRxMsg *msg);
void Speed_Data_deal(ReceiveTypeDef *Receive,CanRxMsg * msg);
void Shooting_Speed_Data_deal(ReceiveTypeDef *Receive,CanRxMsg * msg);
void Position_Data_deal(ReceiveTypeDef *Receive,CanRxMsg * msg);
void Position_Round_Data_deal(ReceiveTypeDef *Receive,CanRxMsg * msg);
void Get_Position_Bias(ReceiveTypeDef *Receive,CanRxMsg * msg);
#endif

extern ReceiveTypeDef CM1_Feedback;
extern ReceiveTypeDef CM2_Feedback;
extern ReceiveTypeDef CM3_Feedback;
extern ReceiveTypeDef CM4_Feedback;
extern ReceiveTypeDef Yaw_speed_Feedback;
extern ReceiveTypeDef Yaw_position_Feedback;
extern ReceiveTypeDef Pitch_speed_Feedback;
extern ReceiveTypeDef Pitch_position_Feedback;
extern ReceiveTypeDef Shoot_speed_Feedback;
extern ReceiveTypeDef Shoot_position_Feedback;
