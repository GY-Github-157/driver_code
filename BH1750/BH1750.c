/**
  *****************************************************************************    
  * FileName   : BH1750.c   
  * Version    : v1.0		
  * Author     : LX_GY			
  * Date       : 2023-10-06         
  * Description: BH1750驱动 基于HAL库的
  * Function List:  
  	1. ....
  	   <version>: 		
  <modify staff>:
  		  <data>:
   <description>:  
  	2. ...
  ******************************************************************************
 */
#include "BH1750.h"
#include "i2c.h"

I2C_HandleTypeDef *hi2c = &hi2c2;       //IIC号

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    BH1780_Init
  * @param   None
  * @brief   初始化，上电指令
  * @Data    2023-10-06
 **/
/* -------------------------------- end -------------------------------- */
void BH1750_Init(void){
    BH1750_Send_Cmd(BH1750_Power_On_Cmd);
    return;
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    BH1750_Send_Cmd
  * @param   buf: cmd 
  * @brief   发送指令
  * @Data    2023-10-06
 **/
/* -------------------------------- end -------------------------------- */
void BH1750_Send_Cmd(uint8_t buf){
    HAL_I2C_Master_Transmit(hi2c,BH1750_Slave_Addr,&buf,1,100);
    return;
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    BH1750_Read_data
  * @param   None
  * @brief   接收消息
  * @Data    2023-10-06
 **/
/* -------------------------------- end -------------------------------- */
uint16_t BH1750_Read_data(void){
    uint8_t buf[2];
    HAL_I2C_Master_Receive(hi2c,BH1750_Slave_Addr,buf,2,100); 

    return ((buf[0]<<8)|buf[1]);
}


/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    BH1750_Get_Data
  * @param   None
  * @brief   获取光照值（Lx）
  * @Data    2023-10-06
 **/
/* -------------------------------- end -------------------------------- */
float BH1750_Get_Data(void){
    uint16_t rec=0;
    float value=0.0;
    BH1750_Send_Cmd(BH1750_H_Resolution_Continuously_Mode_Cmd);
    HAL_Delay(180);
    rec = BH1750_Read_data(); 
    value = rec/1.2;
    return value;
}
