#include "sys.h"
#define JudgeBufferLength       150
#define JudgeFrameLength_1      44
#define JudgeFrameLength_2      11
#define JudgeFrameLength_3      24

#define JudgeFrameHeader        0xA5        //帧头 



//2018.5.9 zhu 添加
#define Level1_heat16 90
#define Level2_heat16 180
#define Level3_heat16 360


	/*************************
	命令码ID：0x0001   功能：比赛机器人状态，10Hz频率周期发送
	字节偏移      大小      说明
		 0						2				当前阶段剩余时间，单位s
		 2						1				当前比赛阶段
													0：未开始比赛
													1：准备阶段
													2：自检阶段
													3：5s倒计时
													4：对战中
													5：比赛结算中
		 3						1				机器人当前等级
		 4						2				机器人当前血量
		 6						2				机器人满血量
		 8						1				位置，角度信息有效标志位
		 9						4				位置X坐标值
		 13						4				位置Y坐标值
		 17						4				位置Z坐标值
		 21						4				枪口朝向角度值
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
	命令码ID：0x0002    功能：伤害数据
	字节偏移    大小    说明
		0						1			0-3bits：若变化类型为装甲伤害时，标识装甲ID
											0x0：0号装甲（前）
											0x1：1号装甲（左）
											0x2：2号装甲（后）
											0x3：3号装甲（右）
											0x4：4号装甲（上1）
											0x5：5号装甲（上2）
											其他保留
											4-7bits：血量变化类型
											0x0：装甲伤害（受到攻击）
											0x1：模块掉线
											0x2：弹丸超速
											0x3：弹丸超频
											0x4：枪口超热量
											0x5：底盘超功率
	*************************/
	typedef __packed struct
	{
		uint8_t armorType : 4;
		uint8_t hurtType : 4;
	}extRobotHurt_t;
	
	/*************************
	命令码ID：0x0003    功能：实时射击信息
	字节偏移    大小    说明
			0					1			弹丸类型
											1：17mm弹丸
											2：42mm弹丸
			1					1			弹丸射频
			2					4			弹丸射速
			6					4			保留
	*************************/
	typedef __packed struct
	{
		uint8_t bulletType;
		uint8_t bulletFreq;
		float bulletSpeed;
		float reserved;
	}extShootData_t;
	
	/************************
	命令码ID: 0x0004    功能：实时功率热量数据
	字节偏移    大小    说明
			0					4			底盘输出电压
			4					4			底盘输出电流
			8					4			底盘输出功率
			12				4			底盘功率缓冲
			16				2			17mm枪口热量
			18				2			42mm枪口热量
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
	命令码ID：0x0005    功能：场地交互数据
	字节偏移		大小		说明
			0					1			卡类型
											0：攻击加成卡
											1：防御加成卡
											2：红方加血卡
											3：蓝方加血卡
											4：红方大能量机关卡
											5：蓝方大能量机关卡
			1					1			卡索引号，可用于区分不同区域
	***********************/
	typedef __packed struct
	{
		uint8_t cardType;
		uint8_t cardIdx;
	}extRfidDetect_t;
	
	/**********************
	命令码：0x0006    功能：比赛胜负数据
	字节偏移		大小		说明
			0					1			比赛结果
											0：平局
											1：红方胜
											2：蓝方胜
	**********************/
	typedef __packed struct
	{
		uint8_t winner;
	}extGameResult_t;
	
	/**********************
	命令码：0x0007		功能：Buff获取数据
	字节偏移		大小		说明
			0					1			Buff类型
											0：攻击加成
											1：防御加成
											2：获得大能量机关
			1					1			加成百分比（比如10代表加成10%）
	**********************/
	typedef __packed struct
	{
		uint8_t buffType;
		uint8_t buffAddition;
	}extGetBuff_t;
	
	/**********************
	命令码ID：0x0100		功能：参赛队自定义数据
	字节偏移		大小		说明
			0					4			自定义数据1
			4					4			自定义数据2
			8					4			自定义数据3
			12				1			自定义数据4
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
		联合体，存储2位的数据
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
	

	
/*********裁判系统结构体************/	
	typedef struct
	{
		uint8_t referee_count;              //包序号
    float realVoltage;                  //实时电压
    float realCurrent;                  //实时电流
    int16_t lastBlood;                  //剩余血量
    uint8_t lastHartID;                 //上次收到伤害的装甲板ID号
    float lastBulletShotSpeed;          //上次弹丸射击速度
		float lastGolfShotSpeed;            //上次高尔夫射击速度
		float remainPower;
		double Power;                       //功率
		
		double PowerBuffer;
		uint32_t RemainTime;                //剩余时间
		uint16_t level;                     //等级
			uint16_t shooterHeat16mm;           //16mm热量
		uint16_t shooterHeat42mm;           //42mm热量
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
		
//裁判系统数据缓存
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

