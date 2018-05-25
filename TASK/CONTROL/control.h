#include "sys.h" 

void Control_Task(void);
void CMControlLoop(void);
void GMControlLoop(void);
void Friction_ON(int pwm_speed);
void Friction_OFF(void);
void Power_Limited(void);
void SMControlLoop(void);
extern uint32_t time_tick_1ms;
