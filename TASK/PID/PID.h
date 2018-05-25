#include "sys.h"

#define Incremental 0
#define Positional  1
#define NewIncremental 2
typedef struct
{
    //PID ������
	double Kp;
	double Ki;
	double Kd;
	double Ka;
	//������ ����
	double max_out;  //������
	double dead_band;//PIDƫ������
	double intergral_band;//������
	double max_input;//�������
	//PID���ֵ
	double output;
	double output_compensation;
	//���
	double e_max;
	double e[3];//2���� 1��һ�� 0���ϴ�
	double d_last;
	double intergral;
} PidTypeDef;
void PID_Init(PidTypeDef * pid,double kp,double ki,double kd,double ka,double max_out,double dead_band,double i_deadband,double max_input);
void PID_Calc(PidTypeDef * pid, double rel_val, double set_val,u8 PID_mode);
double pid_myself(double fb,double input);
float MyAbs(float num);
extern PidTypeDef CM1_speed_pid;
extern PidTypeDef CM2_speed_pid;
extern PidTypeDef CM3_speed_pid;
extern PidTypeDef CM4_speed_pid;
extern PidTypeDef Gyro_normal_pid;
extern PidTypeDef Power_limit_pid;
extern PidTypeDef Yaw_speed_pid;
extern PidTypeDef Yaw_position_pid;
extern PidTypeDef Yaw_resist_pid;
extern PidTypeDef Pitch_speed_pid;
extern PidTypeDef Pitch_position_pid;
extern PidTypeDef Shoot_speed_pid;
extern PidTypeDef Shoot_position_pid;
