#ifndef _ROMOTE_H_
#define _ROMOTE_H_

#include "sys.h" 
//2018.5.12 zhu ��������***********************/

#define Friction_speed1 58
#define Friction_speed2 62//30A 68  40A 65 32M/S
/***********************/
#define CLOSE 0
#define OPEN  1
#define RC 0
#define PC 1
#define VAL_LIMIT(val, min, max)\
if(val<=min)\
{\
	val = min;\
}\
else if(val>=max)\
{\
	val = max;\
}\


typedef struct
{
	uint8_t chassis;//������ QWEASD
	uint8_t mode;//ģʽ�� F V ����Ҽ� �Զ���׼ ��� ����
	uint8_t reset;//��λ�� Z X C ���� Ħ���� ���Ź���λ
	uint8_t stall;//��λ�� shift ctrl ���� ����
}Keyboard_t;

extern s32 shootSpeed;
void Remote_Task(void);
void chassis_PC_control(void);
void reset_PC_control(void);
void RC_control(void);
void PC_control(void);
void mouse_PC_control(void);
void mode_PC_control(void);
extern Keyboard_t Keyboard;
extern double vx;
extern double vy;
extern double zz;

extern double w0;
extern double vz;
extern float PC_pitch_ref;
extern float PC_yaw_ref;
extern float RC_pitch_ref;
extern float RC_yaw_ref;
extern u8 denfense_mode;
extern u8 minipc_mode;//��̨ģʽ���壬0Ϊ�ֶ�ģʽ��1Ϊ���Զ�ģʽ��
extern u8 buff_mode;
extern u8 control_mode;
extern u8 friction_flag;
extern double K_Speed;
extern u8 shooting_flag;
extern u8 shoot_num;
extern u8 shooting_allow;
extern s16 shooting_position;
extern u8 Remote_run_flag;
#endif /*_ROMOTE_H_*/
