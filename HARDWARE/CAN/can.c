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

	    //ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��PORTAʱ��	                   											 

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	
	
    //��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1| GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��PA11,PA12
	
	//���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_CAN1); //GPIOA11����ΪCAN1
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_CAN1); //GPIOA12����ΪCAN1
	
	  
  	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;	 //ģʽ���� 
  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=CAN_BS1_8tq; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=CAN_BS2_6tq;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=3;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1 
    
		//���ù�����
	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		  
  	
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;     // �����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
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
	    //CAN_Receive(CAN1, CAN_FIFO0, &rx_message);//���ݴ����������һ��
		Motor_Data_Receive(&rx_message);
    }
}
/***********************�����ĸ�����ĵ���ֵ******************************/
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


/*******************������̨��������ֵ���ĵ���ֵ*****************/
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

/******************����������ݵĴ���********************/
void Motor_Data_Receive(CanRxMsg *msg)
{		
	CAN_Receive(CAN1, CAN_FIFO0, msg);//��ȡ����	
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
			Shooting_Speed_Data_deal(&Shoot_speed_Feedback,msg);//��ʱ�������λ�û�����
			Position_Round_Data_deal(&Shoot_position_Feedback,msg);
		}
		default:
			break;
	}
}


/***********************���̵�����ٶȷ�������**************************/
void Speed_Data_deal(ReceiveTypeDef *Receive,CanRxMsg * msg)
{
	Receive->real[Receive->count]=(msg->Data[2]<<8)|msg->Data[3];//���յ�����ʵ����ֵ
	Receive->count++;
	if(	Receive->count>3)//�Ĵδ���һ��ƽ��ֵ�����շ���Ƶ����1KHZ������ƽ��ֵ�����Ƶ��Ϊ250HZ��PID����Ƶ��Ϊ250HZ
	{
		//Receive->calc=(Receive->real[0]+Receive->real[1]+Receive->real[2]+Receive->real[3])/4;//��������ƽ������ֵ
		Receive->count=0;
	}
}

/*********************��̨�����λ�÷�������****************************/
void Position_Data_deal(ReceiveTypeDef *Receive,CanRxMsg * msg)
{
		Receive->real[0] = (msg->Data[0]<<8)|msg->Data[1];
		Receive->real[1] = (msg->Data[2]<<8)|msg->Data[3];
		Receive->real[2] = (msg->Data[4]<<8)|msg->Data[5];
}

/***************���ֵ�����ٶȷ���ֵ����*************************/
//����̵��������Ϊ����Ƶ�ʲ�һ��
void Shooting_Speed_Data_deal(ReceiveTypeDef *Receive,CanRxMsg * msg)
{
	Receive->real[Receive->count]=(msg->Data[2]<<8)|msg->Data[3];//���յ�����ʵ����ֵ
	Receive->count++;
	if(	Receive->count>1)//�Ĵδ���һ��ƽ��ֵ�����շ���Ƶ����1KHZ������ƽ��ֵ�����Ƶ��Ϊ500HZ��PID����Ƶ��Ϊ500HZ
	{
		//Receive->calc=(Receive->real[0]+Receive->real[1])/2;//��������ƽ������ֵ
		Receive->count=0;
	}
}


/*************���ֵ��λ�ô���***************************/
//��Ϊ���ֵ���м��ٱȣ���Ҫ���⴦��һ��
void Position_Round_Data_deal(ReceiveTypeDef *Receive,CanRxMsg * msg)
{
	Receive->count++;
	if(1)
	{
		Receive->real[0]=((msg->Data[0]<<8)|msg->Data[1])/100;//���յ�����ʵ����ֵ
		
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

/************��ȡ���ֵ�����ϵ�ʱ��λ��ƫ��****************/
void Get_Position_Bias(ReceiveTypeDef *Receive,CanRxMsg * msg)
{
	Receive->bias=((msg->Data[0]<<8)|msg->Data[1])/100;
}