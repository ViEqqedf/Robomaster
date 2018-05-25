#include "main.h"
uint8_t  Send_Data_Buf[34];
u8 Rx_Buf[2][64];	//����32�ֽڵĴ��ڽ��ջ���
static u8 Rx_Act=0;		//����ʹ�õ�buf��
static u8 Rx_Adr=0;		//���ڽ��յڼ��ֽ�
u8 Rx_Ok = 0;
uint16_t  roll_p,roll_i,roll_d;   //��λ�����ͺ�����ֵΪpid��ʵ����10��
uint16_t  pitch_p,pitch_i,pitch_d;
uint16_t  yaw_p,yaw_i,yaw_d;

void Usart2_init(uint32_t bound)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);//ʹ��USART2_GPIOʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); //GPIOD_10����ΪUSART2
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); //GPIOD_11����ΪUSART2
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; //TX RX
	GPIO_Init(GPIOD,&GPIO_InitStructure); 
	
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure);//��ʼ������2

	USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	


	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�
	
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����3�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

}
void USART2_Send_Data(u8 data)
{
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); 
	USART2->DR=(data & (uint16_t)0x01FF);;
}

void USART2_Send_Buf(u8 *DataToSend , u8 data_num)
{
	int data_i=0;
	for(data_i=0;data_i<data_num;data_i++)
	USART2_Send_Data(*(DataToSend+data_i));
}

void USART6_Send_Data(u8 data)
{
	while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET); 
	USART6->DR=(data & (uint16_t)0x01FF);;
}

void USART6_Send_Buf(u8 *DataToSend , u8 data_num)
{
	int data_i=0;
	for(data_i=0;data_i<data_num;data_i++)
	USART6_Send_Data(*(DataToSend+data_i));
}

//pid���δ��ڽ����ж�
void USART2_IRQHandler(void) 
{ 
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)                              //**��д���ܺ���
	{
		u8 com_data;
		com_data = USART2->DR;
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		if(Rx_Adr==0)		//Ѱ��֡ͷ0XAAAF
		{
			if(com_data==0x8A)	
			{
				Rx_Buf[Rx_Act][0] = com_data;
				Rx_Adr = 1;
			}
		}
		
		else if(Rx_Adr==1)
		{
			if(com_data==0x8B)	
			{
				Rx_Buf[Rx_Act][1] = com_data;
				Rx_Adr = 2;
			}
			else
				Rx_Adr = 0;
		}
		
		else if(Rx_Adr==2)
		{
			if(com_data==0x1C)	
			{
				Rx_Buf[Rx_Act][2] = com_data;
				Rx_Adr = 3;
			}
			else
				Rx_Adr = 0;
		}
		else if(Rx_Adr==3)
		{
			if(com_data==0xAE)	
			{
				Rx_Buf[Rx_Act][3] = com_data;
				Rx_Adr = 4;
			}
			else
				Rx_Adr = 0;
		}
		else
		{
			Rx_Buf[Rx_Act][Rx_Adr] = com_data;
			Rx_Adr ++;
		}
		if(Rx_Adr==32)		//���ݽ������
		{
			Rx_Adr = 0;
			if(Rx_Act)	
			{ 
				Rx_Act = 0; 			//�л�����
				Rx_Ok = 1;
			}
			else 				
			{
				Rx_Act = 1;
				Rx_Ok = 0;
			}
			roll_p=(uint16_t)Rx_Buf[Rx_Ok][4]<<8|(uint16_t)Rx_Buf[Rx_Ok][5];
			roll_i=(uint16_t)Rx_Buf[Rx_Ok][6]<<8|(uint16_t)Rx_Buf[Rx_Ok][7];
			roll_d=(uint16_t)Rx_Buf[Rx_Ok][8]<<8|(uint16_t)Rx_Buf[Rx_Ok][9];
			
			pitch_p=(uint16_t)Rx_Buf[Rx_Ok][10]<<8|(uint16_t)Rx_Buf[Rx_Ok][11];
			pitch_i=(uint16_t)Rx_Buf[Rx_Ok][12]<<8|(uint16_t)Rx_Buf[Rx_Ok][13];
			pitch_d=(uint16_t)Rx_Buf[Rx_Ok][14]<<8|(uint16_t)Rx_Buf[Rx_Ok][15];
			
			yaw_p=(uint16_t)Rx_Buf[Rx_Ok][16]<<8|(uint16_t)Rx_Buf[Rx_Ok][17];
			yaw_i=(uint16_t)Rx_Buf[Rx_Ok][18]<<8|(uint16_t)Rx_Buf[Rx_Ok][19];
			yaw_d=(uint16_t)Rx_Buf[Rx_Ok][20]<<8|(uint16_t)Rx_Buf[Rx_Ok][21];
			PID_Init(&Pitch_speed_pid,pitch_p/100.0f,pitch_i/100.0f,pitch_d/100.0f,0,8000,0,8000,15000);
			PID_Init(&Pitch_position_pid,yaw_p/100.f,yaw_i/10000.f,yaw_d/100.f,0.8,8000,0,8000,15000);
		}
	}
}

void Send_data16(int16_t *dataddr,u8 len,u8 func_word)             //16λ����  ֻ�ܷ���ż��λ
{
 
 u8 length=0;
 u8 i;
 u8 sum=0;
	if(len>28) len=28;
 	 switch(func_word)
	{
		case 1:func_word=0xA1;break;
		case 2:func_word=0xA2;break;
		case 3:func_word=0xA3;break;
		case 4:func_word=0xA4;break;
	}
	Send_Data_Buf[length++] = 0x88;
	Send_Data_Buf[length++] = func_word;                //����fun
	Send_Data_Buf[length++] = len*2;                   //����len
	for(i=0;i<len;i++)
	{Send_Data_Buf[length++] =  ((*(dataddr+i))>>8)&0xFF;       //��������               
	 Send_Data_Buf[length++] = (*(dataddr+i)&0xFF); }
	for(i=0;i<length;i++)                         //�����λ
	{	
	sum += Send_Data_Buf[i];
	}
	Send_Data_Buf[length++] = sum;
	USART2_Send_Buf(Send_Data_Buf,length);
	
}

void Send_data8(u8 *dataddr,u8 len,u8 func_word)    //���� 
{ 
	u8 i;
	if(len>28) len=28;
	switch(func_word)
	{
		case 1:func_word=0xA1;break;
		case 2:func_word=0xA2;break;
		case 3:func_word=0xA3;break;
		case 4:func_word=0xA4;break;
	}
	Send_Data_Buf[0] = 0x88;
	Send_Data_Buf[1] = func_word;                //����fun
	Send_Data_Buf[2] = len;                   //����len
	for(i=0;i<len;i++) Send_Data_Buf[3+i] = *(dataddr+i);       //��������   
	Send_Data_Buf[3+len]=0;
	for(i=0;i<len+3;i++) Send_Data_Buf[3+len]+=  Send_Data_Buf[i];
 	USART2_Send_Buf(Send_Data_Buf,len+4);

}


