/**
  *****************************************************************************                                
  * FileName   : DHT11.h   
  * Version    : v1.0		
  * Author     : LX_GY			
  * Date       : 2023-09-20         
  * Description:    
  * Function List:  
  	1. ....
  	   <version>: 		
  <modify staff>:
  		  <data>:
   <description>:  
  	2. ...
  ******************************************************************************
 */

#ifndef __DHT11_H_
#define __DHT11_H_
#include "stm32f1xx_hal.h"
#include "main.h"

#define DHT11_Data_Out_Low      HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port,DHT11_DATA_Pin,GPIO_PIN_RESET)
#define DHT11_Data_Out_High     HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port,DHT11_DATA_Pin,GPIO_PIN_SET)
#define DHT11_Data_READ         HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port,DHT11_DATA_Pin)


void DHT11_Mode_Output(void);
void DHT11_Mode_Input(void);
void DHT11_Reset(void);
uint8_t DHT11_Rev_Ack(void);
uint8_t DHT11_Rev_Bit(void);
uint8_t DHT11_Rev_Byte(void);
void DHT11_Read_Data(uint8_t *R_Data,uint8_t size);



void DHT11_Init(void);
uint8_t DHT11_Get_TEMP_HUM(float *temp,float *hum);

#endif


 

