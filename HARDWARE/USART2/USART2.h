#ifndef __Usart2_
#define __Usart2_
#include "stm32f4xx.h"

#define odd_even(x)     ((x%2==0)?(x):(x+=1))    
//#include "deca_types.h"

//typedef unsigned char uint8;

#define EN_USART2_RX
void Usart2_init(uint32_t bound);
void USART2_Send_Data(u8 data);
void USART2_Send_Buf(u8 *DataToSend , u8 data_num);
void USART2_IRQHandler(void) ;
void Send_data8(u8 *dataddr,u8 len,u8 func_word);
void Send_data16(int16_t *dataddr,u8 len,u8 func_word);

#endif

