#include "sys.h"
#define JudgeBufferLength       150
#define JudgeFrameLength_1      44
#define JudgeFrameLength_2      11
#define JudgeFrameLength_3      24

#define JudgeFrameHeader        0xA5        //֡ͷ 



//2018.5.9 zhu ���
#define Level1_heat16 90
#define Level2_heat16 180
#define Level3_heat16 360


	/*************************
	������ID��0x0001   ���ܣ�����������״̬��10HzƵ�����ڷ���
	�ֽ�ƫ��      ��С      ˵��
		 0						2				��ǰ�׶�ʣ��ʱ�䣬��λs
		 2						1				��ǰ�����׶�
													0��δ��ʼ����
													1��׼���׶�
													2���Լ�׶�
													3��5s����ʱ
													4����ս��
													5������������
		 3						1				�����˵�ǰ�ȼ�
		 4						2				�����˵�ǰѪ��
		 6						2				��������Ѫ��
		 8						1				λ�ã��Ƕ���Ϣ��Ч��־λ
		 9						4				λ��X����ֵ
		 13						4				λ��Y����ֵ
		 17						4				λ��Z����ֵ
		 21						4				ǹ�ڳ���Ƕ�ֵ
	*************************/
	typedef __packed struct
	{
		uint8_t validFlag;
		float x;
		float y;
		float z;
		float yaw;
	}position_t;
	typedef __packed struct
	{
		uint16_t stageRemianTime;
		uint8_t gameProgress;
		uint8_t robotLevel;
		uint16_t remainHP;
		uint16_t maxHP;
		position_t position;
	}extGameRobotState_t;
	
	/*************************
	������ID��0x0002    ���ܣ��˺�����
	�ֽ�ƫ��    ��С    ˵��
		0						1			0-3bits�����仯����Ϊװ���˺�ʱ����ʶװ��ID
											0x0��0��װ�ף�ǰ��
											0x1��1��װ�ף���
											0x2��2��װ�ף���
											0x3��3��װ�ף��ң�
											0x4��4��װ�ף���1��
											0x5��5��װ�ף���2��
											��������
											4-7bits��Ѫ���仯����
											0x0��װ���˺����ܵ�������
											0x1��ģ�����
											0x2�����賬��
											0x3�����賬Ƶ
											0x4��ǹ�ڳ�����
											0x5�����̳�����
	*************************/
	typedef __packed struct
	{
		uint8_t armorType : 4;
		uint8_t hurtType : 4;
	}extRobotHurt_t;
	
	/*************************
	������ID��0x0003    ���ܣ�ʵʱ�����Ϣ
	�ֽ�ƫ��    ��С    ˵��
			0					1			��������
											1��17mm����
											2��42mm����
			1					1			������Ƶ
			2					4			��������
			6					4			����
	*************************/
	typedef __packed struct
	{
		uint8_t bulletType;
		uint8_t bulletFreq;
		float bulletSpeed;
		float reserved;
	}extShootData_t;
	
	/************************
	������ID: 0x0004    ���ܣ�ʵʱ������������
	�ֽ�ƫ��    ��С    ˵��
			0					4			���������ѹ
			4					4			�����������
			8					4			�����������
			12				4			���̹��ʻ���
			16				2			17mmǹ������
			18				2			42mmǹ������
	************************/
	typedef __packed struct
	{
		float chassisVolt;
		float chassisCurrent;
		float chassisPower;
		float chassisPowerBuffer;
		uint16_t shooterHeat0;
		uint16_t shooterHeat1;
	}extPowerHeatData_t;
	
	/***********************
	������ID��0x0005    ���ܣ����ؽ�������
	�ֽ�ƫ��		��С		˵��
			0					1			������
											0�������ӳɿ�
											1�������ӳɿ�
											2���췽��Ѫ��
											3��������Ѫ��
											4���췽���������ؿ�
											5���������������ؿ�
			1					1			�������ţ����������ֲ�ͬ����
	***********************/
	typedef __packed struct
	{
		uint8_t cardType;
		uint8_t cardIdx;
	}extRfidDetect_t;
	
	/**********************
	�����룺0x0006    ���ܣ�����ʤ������
	�ֽ�ƫ��		��С		˵��
			0					1			�������
											0��ƽ��
											1���췽ʤ
											2������ʤ
	**********************/
	typedef __packed struct
	{
		uint8_t winner;
	}extGameResult_t;
	
	/**********************
	�����룺0x0007		���ܣ�Buff��ȡ����
	�ֽ�ƫ��		��С		˵��
			0					1			Buff����
											0�������ӳ�
											1�������ӳ�
											2����ô���������
			1					1			�ӳɰٷֱȣ�����10����ӳ�10%��
	**********************/
	typedef __packed struct
	{
		uint8_t buffType;
		uint8_t buffAddition;
	}extGetBuff_t;
	
	/**********************
	������ID��0x0100		���ܣ��������Զ�������
	�ֽ�ƫ��		��С		˵��
			0					4			�Զ�������1
			4					4			�Զ�������2
			8					4			�Զ�������3
			12				1			�Զ�������4
	**********************/
	typedef __packed struct
	{
		float data1;
		float data2;
		float data3;
		uint8_t mask;
	}extShowData_t;
	
	typedef __packed struct
	{
		uint16_t sof;
		uint16_t data_length;
		uint16_t seq;
		uint16_t CRC8;
	}frame_header_t;
	
	/*****************
		�����壬�洢2λ������
	*******************/
	typedef union   
	{  
    struct   
    {  
        u8 low_byte;  
        u8 high_byte;  
     }float_2byte;  
            
     uint16_t  value;  
	}FLAOT_2UNION;  
	

	
/*********����ϵͳ�ṹ��************/	
	typedef struct
	{
		uint8_t referee_count;              //�����
    float realVoltage;                  //ʵʱ��ѹ
    float realCurrent;                  //ʵʱ����
    int16_t lastBlood;                  //ʣ��Ѫ��
    uint8_t lastHartID;                 //�ϴ��յ��˺���װ�װ�ID��
    float lastBulletShotSpeed;          //�ϴε�������ٶ�
		float lastGolfShotSpeed;            //�ϴθ߶�������ٶ�
		float remainPower;
		double Power;                       //����
		
		double PowerBuffer;
		uint32_t RemainTime;                //ʣ��ʱ��
		uint16_t level;                     //�ȼ�
			uint16_t shooterHeat16mm;           //16mm����
		uint16_t shooterHeat42mm;           //42mm����
		uint8_t hurt;
	}InfantryJudge_Struct;

typedef union   
{  
    struct   
    {  
        unsigned char low_byte;  
        unsigned char mlow_byte;  
        unsigned char mhigh_byte;  
        unsigned char high_byte;  
     }float_byte;  
            
     float  value;  
}FLAOT_UNION; 
		
//����ϵͳ���ݻ���
extern u8 JudgeDataBuffer[JudgeBufferLength];
extern InfantryJudge_Struct InfantryJudge;
extern int max_heat16;

unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8);
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);  
void Judge_InitConfig(void);
void Referee_Init(void);
void USART6_IRQHandler(void);

