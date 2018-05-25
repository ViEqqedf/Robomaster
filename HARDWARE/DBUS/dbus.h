#ifndef _DBUS_H_
#define _DBUS_H_
#include "sys.h"

#define KEY_W			0x0001
#define KEY_S			0x0002
#define KEY_A			0x0004
#define KEY_D			0x0008
#define KEY_SHIFT	0x0010
#define KEY_CTRL	0x0020
#define KEY_Q			0x0040
#define KEY_E  		0x0080
#define KEY_R 		0x0100
#define KEY_F 		0x0200
#define KEY_G 		0x0400
#define KEY_Z 		0x0800
#define KEY_X 		0x1000
#define KEY_C 		0x2000
#define KEY_V 		0x4000
#define KEY_B 		0x8000
/* ----------------------- RC Channel Definition---------------------------- */
#define RC_CH_VALUE_MIN ((uint16_t)364 )
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)
#define RC_CH_VALUE_MAX ((uint16_t)1684)
/* ----------------------- RC Switch Definition----------------------------- */
#define RC_SW_UP ((uint16_t)1)
#define RC_SW_MID ((uint16_t)3)
#define RC_SW_DOWN ((uint16_t)2)
/* ----------------------- PC Key Definition-------------------------------- */
#define KEY_PRESSED_OFFSET_W ((uint16_t)0x01<<0)
#define KEY_PRESSED_OFFSET_S ((uint16_t)0x01<<1)
#define KEY_PRESSED_OFFSET_A ((uint16_t)0x01<<2)
#define KEY_PRESSED_OFFSET_D ((uint16_t)0x01<<3)
#define KEY_PRESSED_OFFSET_Q ((uint16_t)0x01<<6)  //SHIFT和Q反了
#define KEY_PRESSED_OFFSET_E ((uint16_t)0x01<<7)  //CTRL和E反了
#define KEY_PRESSED_OFFSET_SHIFT ((uint16_t)0x01<<4)
#define KEY_PRESSED_OFFSET_CTRL ((uint16_t)0x01<<5)
/* ----------------------- Data Struct ------------------------------------- */
typedef struct
{
	struct
	{
		uint16_t ch0;
		uint16_t ch1;
		uint16_t ch2;
		uint16_t ch3;
		uint8_t s1;
		uint8_t s2;
	}rc;
	struct
	{
		int16_t x;
		int16_t y;
		int16_t z;
		uint8_t press_l;
		uint8_t press_r;
	}mouse;
	struct
	{
		uint16_t v; 
	}key;
}RC_Ctl_t;

extern volatile unsigned char sbus_rx_buffer[25];
extern RC_Ctl_t RC_Ctl;


void DBUS_Init(void);

/* ----------------------- Internal Data ----------------------------------- */



#endif



