PITCH	位置 5100 - 6200（由低到高依次升高） 	
		向下速度为正，向上速度为负
		电流为负方向向上
		
3.19中午  speed P0.9超调

超级电容 PE5

yaw 右- 左+ 正电流右转


不加图传测速使得参数分配：
//void PID_Init(PidTypeDef * pid,double kp,double ki,double kd,double ka,double max_out,double dead_band,double i_band,double max_input)
PID_Init(&CM1_speed_pid,15,0.7,0,0,4000,100,0,5260); //1号电机速度环
PID_Init(&CM2_speed_pid,15,0.7,0,0,4000,100,0,5260);
PID_Init(&CM3_speed_pid,15,0.7,0,0,4000,100,0,5260);
PID_Init(&CM4_speed_pid,15,0.7,0,0,4000,100,0,5260);		//7260
PID_Init(&Gyro_normal_pid,0.40,0,0,0,2000,10,0,8192); //底盘旋转环

PID_Init(&Yaw_speed_pid, 0.5, 0, 0, 0, 4500, 20, 0,15000);//输出在1500左右
PID_Init(&Yaw_position_pid, 10, 0, 15, 0, 15000, 5, 0, 1800);// 26 1.4

PID_Init(&Pitch_speed_pid, 0.3, 0, 1, 0, 4000, 50, 400, 6000);		//输出在3000力很大
PID_Init(&Pitch_position_pid, 5, 0, 5, 0, 4500, 50, 30, 6200); // 28 0.12       8 2 3

PID_Init(&Shoot_position_pid,9,0,2,0,12000,100,0,15744);
PID_Init(&Shoot_speed_pid,3,6,4,0,2000,100,0,12000);