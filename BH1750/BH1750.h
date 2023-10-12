/**
  *****************************************************************************                                
  * FileName   : BH1750.h   
  * Version    : v1.0		
  * Author     : LX_GY			
  * Date       : 2023-09-20         
  * Description:    BH1750 光照强度驱动
  * Function List:  
  	1. ....
  	   <version>: 		
  <modify staff>:
  		  <data>:
   <description>:  
  	2. ...
  ******************************************************************************
 */

#ifndef __BH1750_H_
#define __BH1750_H_

#include "stm32f1xx_hal.h"
#include "stdio.h"

#define BH1750_Slave_Addr       0x46                    //010 0011     0100 0110     


#define BH1750_Power_Down_Cmd   0x00
#define BH1750_Power_On_Cmd     0x01   
#define BH1750_Reset_Cmd        0x07

#define BH1750_H_Resolution_Continuously_Mode_Cmd         0x10       //高分辨率(1 lx)   连续测量  测量时间为 120ms
#define BH1750_H_Resolution_Continuously_Mode2_Cmd        0x11       //高分辨率(0.5 lx) 连续测量  测量时间为 120ms
#define BH1750_L_Resolution_Continuously_Mode_Cmd         0x13       //高分辨率(4 lx)   连续测量  测量时间为 16ms

#define BH1750_H_Resolution_Single_Mode_Cmd               0x20       //高分辨率(1 lx)   单次测量  测量时间为 120ms
#define BH1750_H_Resolution_Single_Mode2_Cmd              0x21       //高分辨率(0.5 lx) 单次测量  测量时间为 120ms
#define BH1750_L_Resolution_Single_Mode_Cmd               0x23       //高分辨率(4 lx)   连续测量  测量时间为 16ms


void BH1750_Init(void);
void BH1750_Send_Cmd(uint8_t cmd);
uint16_t BH1750_Read_data(void);

float BH1750_Get_Data(void);

#endif


 

