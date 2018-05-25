#include "can.h"
#include "led.h"
#include "delay.h"
#include "usart.h"


ReceiveTypeDef CM1_Feedback={0};
ReceiveTypeDef CM2_Feedback={0};
ReceiveTypeDef CM3_Feedback={0};
ReceiveTypeDef CM4_Feedback={0};
ReceiveTypeDef Yaw_speed_Feedback;
ReceiveTypeDef Yaw_position_Feedback;
ReceiveTypeDef Pitch_speed_Feedback;
ReceiveTypeDef Pitch_position_Feedback;
ReceiveTypeDef Shoot_speed_Feedback;
ReceiveTypeDef Shoot_position_Feedback;


void CAN1_Init(void)
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
   	NVIC_InitTypeDef  NVIC_InitStructure;

	    //使能相关时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能PORTA时钟	                   											 

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	
	
    //初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1| GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化PA11,PA12
	
	//引脚复用映射配置
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_CAN1); //GPIOA11复用为CAN1
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_CAN1); //GPIOA12复用为CAN1
	
	  
  	//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;	//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;	 //模式设置 
  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=CAN_BS1_8tq; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=CAN_BS2_6tq;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=3;  //分频系数(Fdiv)为brp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 
    
		//配置过滤器
	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		  
  	
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

}   
 

void CAN1_RX0_IRQHandler(void)
{
	  CanRxMsg rx_message;	
    if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
        CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
	    CAN_ClearFlag(CAN1, CAN_FLAG_FF0); 		
	    //CAN_Receive(CAN1, CAN_FIFO0, &rx_message);//数据处理里接收了一次
		Motor_Data_Receive(&rx_message);
    }
}
/***********************发送四个电机的电流值******************************/
void Set_CM_Speed(s16 cm1_iq,s16 cm2_iq,s16 cm3_iq,s16 cm4_iq)
{	
	CanTxMsg TxMessage;
	TxMessage.StdId=0x200;	 
	TxMessage.IDE=CAN_Id_Standard;		  
	TxMessage.RTR=CAN_RTR_Data;		  
	TxMessage.DLC=0x08;							
	TxMessage.Data[0] = (uint8_t)(cm1_iq >> 8);
	TxMessage.Data[1] = (uint8_t)cm1_iq;
	TxMessage.Data[2] = (uint8_t)(cm2_iq >> 8);
	TxMessage.Data[3] = (uint8_t)cm2_iq;   
	TxMessage.Data[4] = (uint8_t)(cm3_iq >> 8);
	TxMessage.Data[5] = (uint8_t)cm3_iq;
	TxMessage.Data[6] = (uint8_t)(cm4_iq >> 8);
	TxMessage.Data[7] = (uint8_t)cm4_iq;	
	CAN_Transmit(CAN1, &TxMessage);   	
}


/*******************发送云台电机、拨轮电机的电流值*****************/
void Set_Gimbal_Current(s16 gimbal_yaw_iq, s16 gimbal_pitch_iq,s16 shoot_iq)
{
    CanTxMsg TxMessage;    
    TxMessage.StdId = 0x1FF;
    TxMessage.IDE = CAN_Id_Standard;
    TxMessage.RTR = CAN_RTR_Data;
    TxMessage.DLC = 0x08;  
    TxMessage.Data[0] = (uint8_t)(gimbal_yaw_iq >> 8);
    TxMessage.Data[1] = (uint8_t)gimbal_yaw_iq;
    TxMessage.Data[2] = (uint8_t)(gimbal_pitch_iq >> 8);
    TxMessage.Data[3] = (uint8_t)gimbal_pitch_iq;
    TxMessage.Data[4] = (uint8_t)(shoot_iq>>8);
    TxMessage.Data[5] = (uint8_t)shoot_iq;
    TxMessage.Data[6] = 0x00;
    TxMessage.Data[7] = 0x00;
    CAN_Transmit(CAN1,&TxMessage);
}

/******************电机反馈数据的处理********************/
void Motor_Data_Receive(CanRxMsg *msg)
{		
	CAN_Receive(CAN1, CAN_FIFO0, msg);//读取数据	
	switch(msg->StdId)
	{
		case 0x201:
		{
			Speed_Data_deal(&CM1_Feedback,msg);
			break;	
		}
		case 0x202:
		{
			Speed_Data_deal(&CM2_Feedback,msg);
			break;
		}
		case 0x203:
		{
			Speed_Data_deal(&CM3_Feedback,msg);			
			break;
		}
		case 0x204:
		{
			Speed_Data_deal(&CM4_Feedback,msg);
		break;
		}
		case 0x205:
		{		
			Position_Data_deal(&Yaw_position_Feedback,msg);		
			break;			
		}
		case 0x206:
		{
			Position_Data_deal(&Pitch_position_Feedback,msg);		
			break;
		}
		case 0x207:
		{
			Shooting_Speed_Data_deal(&Shoot_speed_Feedback,msg);//暂时打算进行位置环控制
			Position_Round_Data_deal(&Shoot_position_Feedback,msg);
		}
		default:
			break;
	}
}


/***********************底盘电机的速度反馈处理**************************/
void Speed_Data_deal(ReceiveTypeDef *Receive,CanRxMsg * msg)
{
	Receive->real[Receive->count]=(msg->Data[2]<<8)|msg->Data[3];//接收到的真实数据值
	Receive->count++;
	if(	Receive->count>3)//四次处理一次平均值，接收反馈频率是1KHZ，数据平均值处理后频率为250HZ，PID处理频率为250HZ
	{
		//Receive->calc=(Receive->real[0]+Receive->real[1]+Receive->real[2]+Receive->real[3])/4;//处理过后的平均数据值
		Receive->count=0;
	}
}

/*********************云台电机的位置反馈处理****************************/
void Position_Data_deal(ReceiveTypeDef *Receive,CanRxMsg * msg)
{
		Receive->real[0] = (msg->Data[0]<<8)|msg->Data[1];
		Receive->real[1] = (msg->Data[2]<<8)|msg->Data[3];
		Receive->real[2] = (msg->Data[4]<<8)|msg->Data[5];
}

/***************拨轮电机的速度反馈值处理*************************/
//与底盘电机的区别为接受频率不一样
void Shooting_Speed_Data_deal(ReceiveTypeDef *Receive,CanRxMsg * msg)
{
	Receive->real[Receive->count]=(msg->Data[2]<<8)|msg->Data[3];//接收到的真实数据值
	Receive->count++;
	if(	Receive->count>1)//四次处理一次平均值，接收反馈频率是1KHZ，数据平均值处理后频率为500HZ，PID处理频率为500HZ
	{
		//Receive->calc=(Receive->real[0]+Receive->real[1])/2;//处理过后的平均数据值
		Receive->count=0;
	}
}


/*************拨轮电机位置处理***************************/
//因为拨轮电机有减速比，需要特殊处理一下
void Position_Round_Data_deal(ReceiveTypeDef *Receive,CanRxMsg * msg)
{
	Receive->count++;
	if(1)
	{
		Receive->real[0]=((msg->Data[0]<<8)|msg->Data[1])/100;//接收到的真实数据值
		
		if((Receive->real[0]-Receive->real[1])>40)
		{
			Receive->round_cnt++;
		}
		else if((Receive->real[0]-Receive->real[1])< -40)
		{
			Receive->round_cnt--;
		}
		if(Receive->round_cnt>= 192)
		{
			Receive->round_cnt=0;
		}
		else if(Receive->round_cnt<0)
		{
			Receive->round_cnt=191;
		}
		Receive->ecd_value=Receive->real[0]+82*Receive->round_cnt+Receive->bias;
		Receive->real[1]=Receive->real[0];
		Receive->count=0;
	}
}

/************获取拨轮电机刚上电时的位置偏差****************/
void Get_Position_Bias(ReceiveTypeDef *Receive,CanRxMsg * msg)
{
	Receive->bias=((msg->Data[0]<<8)|msg->Data[1])/100;
}