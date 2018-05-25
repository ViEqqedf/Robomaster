#include "sys.h"

#define Incremental 0
#define Positional  1
#define NewIncremental 2
typedef struct
{
    //PID 三参数
	double Kp;
	double Ki;
	double Kd;
	double Ka;
	//最大输出 死区
	double max_out;  //最大输出
	double dead_band;//PID偏差死区
	double intergral_band;//积分区
	double max_input;//最大输入
	//PID输出值
	double output;
	double output_compensation;
	//误差
	double e_max;
	double e[3];//2最新 1上一次 0上上次
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
