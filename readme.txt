PITCH	λ�� 5100 - 6200���ɵ͵����������ߣ� 	
		�����ٶ�Ϊ���������ٶ�Ϊ��
		����Ϊ����������
		
3.19����  speed P0.9����

�������� PE5

yaw ��- ��+ ��������ת


����ͼ������ʹ�ò������䣺
//void PID_Init(PidTypeDef * pid,double kp,double ki,double kd,double ka,double max_out,double dead_band,double i_band,double max_input)
PID_Init(&CM1_speed_pid,15,0.7,0,0,4000,100,0,5260); //1�ŵ���ٶȻ�
PID_Init(&CM2_speed_pid,15,0.7,0,0,4000,100,0,5260);
PID_Init(&CM3_speed_pid,15,0.7,0,0,4000,100,0,5260);
PID_Init(&CM4_speed_pid,15,0.7,0,0,4000,100,0,5260);		//7260
PID_Init(&Gyro_normal_pid,0.40,0,0,0,2000,10,0,8192); //������ת��

PID_Init(&Yaw_speed_pid, 0.5, 0, 0, 0, 4500, 20, 0,15000);//�����1500����
PID_Init(&Yaw_position_pid, 10, 0, 15, 0, 15000, 5, 0, 1800);// 26 1.4

PID_Init(&Pitch_speed_pid, 0.3, 0, 1, 0, 4000, 50, 400, 6000);		//�����3000���ܴ�
PID_Init(&Pitch_position_pid, 5, 0, 5, 0, 4500, 50, 30, 6200); // 28 0.12       8 2 3

PID_Init(&Shoot_position_pid,9,0,2,0,12000,100,0,15744);
PID_Init(&Shoot_speed_pid,3,6,4,0,2000,100,0,12000);