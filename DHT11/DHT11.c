/**
  *****************************************************************************    
  * FileName   : DHT11.c   
  * Version    : v1.0		
  * Author     : LX_GY			
  * Date       : 2023-10-07         
  * Description: DHT11温湿度传感器驱动   
  * Function List:  
  	1. ....
  	   <version>: 		
  <modify staff>:
  		  <data>:
   <description>:  
  	2. ...
  ******************************************************************************
 */


#include "DHT11.h"

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    DHT11_Mode_Output
  * @param   None
  * @brief   输出模式
  * @Data    2023-09-20
 **/
/* -------------------------------- end -------------------------------- */
void DHT11_Mode_Output(void){
  GPIO_InitTypeDef GPIO_InitStruct;
    
  GPIO_InitStruct.Pin = DHT11_DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT11_DATA_GPIO_Port, &GPIO_InitStruct);
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    DHT11_Mode_Input
  * @param   None
  * @brief   输入模式
  * @Data    2023-09-20
 **/
/* -------------------------------- end -------------------------------- */
void DHT11_Mode_Input(void){
  GPIO_InitTypeDef GPIO_InitStruct;
    
  GPIO_InitStruct.Pin = DHT11_DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(DHT11_DATA_GPIO_Port, &GPIO_InitStruct);
}
  
/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    DHT11_Reset
  * @param   None
  * @brief   发送响应
  * @Data    2023-09-20
 **/
/* -------------------------------- end -------------------------------- */
void DHT11_Reset(void){
    DHT11_Mode_Output();
    DHT11_Data_Out_Low;
    HAL_Delay(20);      //延时20ms > 18ms
    DHT11_Data_Out_High;
    DHT11_Mode_Input();
}  

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    DHT11_Rev_Ack
  * @param   None
  * @brief   等待DHT响应，低电平 80us
  * @Data    2023-09-20
 **/
/* -------------------------------- end -------------------------------- */
uint8_t DHT11_Rev_Ack(void){

    HAL_Delay_us(40);
    
    if(DHT11_Data_READ ==0){
        return 0;
    }
    else {
        return 1;
    }
       
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    DHT11_Rev_Bit
  * @param   None
  * @brief   获取1bit数据
  * @Data    2023-09-20
 **/
/* -------------------------------- end -------------------------------- */
uint8_t DHT11_Rev_Bit(void){
    
//    uint8_t timeout=0;
   
    while(!DHT11_Data_READ);            //等待度过50us的低电平间隔
   
    HAL_Delay_us(40);                   //等待后，延时40us，检测电平
    
    
    /*
        每个数据之前都会由50us低电平，当检测到低电平后等待，
        若电平变为高，则数据为1，
        若电平一直为低，且超时100us，则数据为0   （50us+70us）
        
        数据0： 26-28us高电平
        数据1： 70us 高电平
    */
    if(DHT11_Data_READ)
    {   
        while(DHT11_Data_READ);         //等待数据1的高电平结束
        return 1;
    }
    else
        return 0;
}
/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    DHT11_Rev_Byte
  * @param   None
  * @brief   接收1字节数据
  * @Data    2023-09-20
 **/
/* -------------------------------- end -------------------------------- */
uint8_t DHT11_Rev_Byte(void){
    uint8_t data=0;
    
    for(uint8_t i=0;i<8;i++){
        data<<=1;
        data |= DHT11_Rev_Bit();
    }
    return data;
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    DHT11_Read_Data
  * @param   R_Data[5]:接收数据buff 
  * @brief   接收收到的数据
  * @Data    2023-09-20
 **/
/* -------------------------------- end -------------------------------- */
void DHT11_Read_Data(uint8_t *R_Data,uint8_t size){
    for(uint8_t i=0;i<size;i++){
        R_Data[i]=DHT11_Rev_Byte();
    }
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    DHT11_Get_TEMP_HUM
  * @param   temp: [输入/出] 
**			 hum: [输入/出] 
  * @brief   获取温湿度
  * @Data    2023-09-20
 **/
/* -------------------------------- end -------------------------------- */
uint8_t DHT11_Get_TEMP_HUM(float *temp,float *hum){
    uint8_t R_Data[5]={0};
    
    DHT11_Reset();      //发送响应
    
    if( DHT11_Rev_Ack()==0 )        //等待响应
    {
        while(!DHT11_Data_READ);    
        while(DHT11_Data_READ); 
        
        DHT11_Read_Data(R_Data,5);      //读取数据
        
                                        //处理数据
        if(R_Data[4] == (R_Data[0]+R_Data[1]+R_Data[2]+R_Data[3]) ){
            *hum = R_Data[0] + R_Data[1];
            *temp = R_Data[2]+R_Data[3];
        }
    }else{
        return 1;
    }
    return 0;  
}


/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    DHT11_Init
  * @param   None
  * @brief   DHT11初始化
  * @Data    2023-09-20
 **/
/* -------------------------------- end -------------------------------- */
void DHT11_Init(void){
    DHT11_Mode_Output();
    DHT11_Data_Out_High;
}
