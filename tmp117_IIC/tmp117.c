/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "i2c.h"
#include "tmp117.h"
#include "gpio.h"
#include <stdio.h>



#define TMP117_TmpRst 0x06          //复位命令，为确定地址后第二个字节命令
#define MemAddSize    2             //TMP117寄存器大小

uint8_t TMPEAddr = TMP117_TempAddr;
uint8_t UPAddr = TMP117_UP_LIMIT;
int regflag1 = 0;
int regflag2 = 0;
int regflag3 = 0;

/*警报标志
uint8_t aralm1[2] ={0x01,0xE0}; 
uint8_t aralm2[2] ={0x02,0xE0}; 
uint8_t aralm3[2] ={0x03,0xE0}; 
*/

/*TMP117模式初始化：0320h 警报模式；连续转换；ALERT低电平有效，反应报警标志位状态；8次平均值转换*/
/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    TMP117_Init
  * @param   None
  * @brief   模式初始化
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
void TMP117_Init(void)
{	
	uint8_t OpenBuf[3] 		= {TMP117_OpenEEPAddr,TMP117_EEPROM_Open_HIGH,TMP117_EEPROM_Open_LOW};
	uint8_t CloseBuf[3] 	= {TMP117_OpenEEPAddr,TMP117_EEPROM_Close_HIGH,TMP117_EEPROM_Close_LOW};
	uint8_t ConfigBuf[3] 	= {TMP117_ConfigAddr,0x02,0x00};
	uint8_t upLimBuf[3] 	= {TMP117_UP_LIMIT,0x1E,0x00};
	uint8_t downLimBuf[3] 	= {TMP117_DOWN_LIMIT,0xF1,0x00};

	regflag1 = 1;
	regflag2 = 1;
	regflag3 = 1;
	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp1Addr,OpenBuf,sizeof(OpenBuf),1000);//解锁EEPROM寄存器
	HAL_Delay(7);

	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp1Addr,ConfigBuf,sizeof(ConfigBuf),20);	//设置配置寄存器
	HAL_Delay(7);
	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp1Addr,upLimBuf,sizeof(upLimBuf),20);//设置温度上限寄存器
	HAL_Delay(7);
	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp1Addr,downLimBuf,sizeof(downLimBuf),20);//设置温度下限寄存器
	HAL_Delay(7);
	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp1Addr,CloseBuf,sizeof(CloseBuf),20);	 //EEPROM寄存器上锁	
			
	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp2Addr,OpenBuf,sizeof(OpenBuf),20);//解锁EEPROM寄存器
	HAL_Delay(7);
	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp2Addr,ConfigBuf,sizeof(ConfigBuf),20);	//设置配置寄存器
	HAL_Delay(7);
	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp2Addr,upLimBuf,sizeof(upLimBuf),20);//设置温度上限寄存器
	HAL_Delay(7);
	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp2Addr,downLimBuf,sizeof(downLimBuf),20);//设置温度下限寄存器
	HAL_Delay(7);
	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp2Addr,CloseBuf,sizeof(CloseBuf),20);	 //EEPROM寄存器上锁

	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp3Addr,OpenBuf,sizeof(OpenBuf),20);//解锁EEPROM寄存器
	HAL_Delay(7);
	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp3Addr,ConfigBuf,sizeof(ConfigBuf),20);	//设置配置寄存器
	HAL_Delay(7);
	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp3Addr,upLimBuf,sizeof(upLimBuf),20);//设置温度上限寄存器
	HAL_Delay(7);
	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp3Addr,downLimBuf,sizeof(downLimBuf),20);//设置温度下限寄存器
	HAL_Delay(7);
	HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp3Addr,CloseBuf,sizeof(CloseBuf),20);	 //EEPROM寄存器上锁
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CompareTmpe
  * @param   up: [输入/出] 
**			 down: [输入/出] 
  * @brief   
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
int CompareTmpe(uint8_t * up ,uint8_t * down)
{
	if(up[0]<0x80&&down[0]>0x79)
	{
		return 1;
	}
	else if(up[0]>0x79&&down[0]>0x79)
	{
		if((up - down)>0x80)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if(up[0]<0x80&&down[0]<0x80)
	{
		if((up - down)<0x80)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    setTMP117_Init
  * @param   DevAddress: [输入/出] 
**			 RegAddress: 需要更新的传感器地址 
**			 TXBuf: 2字节寄存器命令 
  * @brief   设置TMP117模式
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
void setTMP117_Init(uint16_t DevAddress,uint16_t RegAddress,uint8_t *TXBuf)
{
	uint8_t OpenBuf[3] = {TMP117_OpenEEPAddr,TMP117_EEPROM_Open_HIGH,TMP117_EEPROM_Open_LOW};
	uint8_t CloseBuf[3] = {TMP117_OpenEEPAddr,TMP117_EEPROM_Close_HIGH,TMP117_EEPROM_Close_LOW};
	uint8_t ConfigBuf[3] = {TMP117_ConfigAddr,TXBuf[0],TXBuf[1]};
	uint8_t upLimBuf[3] = {TMP117_UP_LIMIT,TXBuf[0],TXBuf[1]};
	uint8_t downLimBuf[3] = {TMP117_DOWN_LIMIT,TXBuf[0],TXBuf[1]};
	uint8_t COMPARE[3];
	switch(RegAddress)
	{
		case 0x01:
		{
			HAL_I2C_Master_Transmit(&hi2c1,DevAddress,OpenBuf,sizeof(OpenBuf),20);//解锁EEPROM寄存器
			HAL_I2C_Master_Transmit(&hi2c1,DevAddress,ConfigBuf,sizeof(ConfigBuf),20);//设置配置寄存器
			if(HAL_I2C_Master_Transmit(&hi2c1,DevAddress,CloseBuf,sizeof(CloseBuf),20) == HAL_OK)//EEPROM寄存器上锁
			{
				HAL_Delay(7);
//				printf("EEPROM CONFIG IDLE!");
			}
				else
			{
//				printf("EEPROM BUSY!");
				OpenBuf[0] = 0x00;
			}
			break;
		}
    case 0x02:
		{
			HAL_I2C_Master_Transmit(&hi2c1,DevAddress,OpenBuf,sizeof(OpenBuf),20);//解锁EEPROM寄存器
			HAL_I2C_Master_Transmit(&hi2c1,DevAddress,upLimBuf,sizeof(upLimBuf),20);//设置温度上限寄存器
			if(HAL_I2C_Master_Transmit(&hi2c1,DevAddress,CloseBuf,sizeof(CloseBuf),20) == HAL_OK)//EEPROM寄存器上锁
			{
				HAL_Delay(7);
//				printf("EEPROM UP_LIMIT IDLE!");、
			}
				else
			{
//				printf("EEPROM BUSY!");
				OpenBuf[0] = 0x00;
			}
			break;
		} 
    case 0x03:
		{
			HAL_I2C_Master_Transmit(&hi2c1,DevAddress,&UPAddr,sizeof(UPAddr),20);
			HAL_I2C_Master_Transmit(&hi2c1,DevAddress+0x01,&UPAddr,sizeof(UPAddr),20);
			HAL_I2C_Master_Receive(&hi2c1,DevAddress+0x01,COMPARE,MemAddSize,20);
			if(CompareTmpe(COMPARE,&downLimBuf[1])==1)
			{
				HAL_I2C_Master_Transmit(&hi2c1,DevAddress,OpenBuf,sizeof(OpenBuf),20);//解锁EEPROM寄存器
				HAL_I2C_Master_Transmit(&hi2c1,DevAddress,downLimBuf,sizeof(downLimBuf),20);//设置温度下限寄存器
				if(HAL_I2C_Master_Transmit(&hi2c1,DevAddress,CloseBuf,sizeof(CloseBuf),20) == HAL_OK)//EEPROM寄存器上锁
				{
					HAL_Delay(7);
//					printf("EEPROM DOWN_LIMIT IDLE!");
				}
					else
				{
//					printf("EEPROM BUSY!");
					OpenBuf[0] = 0x00;
			  }
			}
			else
			{
//				printf("DOWN EXCEED UP!");
				OpenBuf[0] = 0x00;
			}
			break;
		}		
	}
	

}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    updateTMP117
  * @param   DevAddress: 设备地址
**			 RegAddress: 需要更新的传感器地址
**			 TXBuf: 2字节寄存器命令 
  * @brief   选择特定地址的TMP117设置模式
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
 void updateTMP117(uint16_t DevAddress,uint16_t RegAddress,uint8_t *TXBuf)
{		
  switch(DevAddress)
  {
		case 0x01: 
		{
		  setTMP117_Init(TMP117_Wri_Tmp1Addr,RegAddress,TXBuf);
		  regflag1 = 1;
			break;
		}
    case 0x02:
    {
			setTMP117_Init(TMP117_Wri_Tmp2Addr,RegAddress,TXBuf);
		  regflag2 = 1;
			break;
		}			
		case 0x03: 
		{
			setTMP117_Init(TMP117_Wri_Tmp3Addr,RegAddress,TXBuf);
		  regflag3 = 1;
			break;
		}
    case 0x7F:
		{
			setTMP117_Init(TMP117_Wri_Tmp1Addr,RegAddress,TXBuf);
	  	regflag1 = 1;
		  setTMP117_Init(TMP117_Wri_Tmp2Addr,RegAddress,TXBuf);
	   	regflag2 = 1;
		  setTMP117_Init(TMP117_Wri_Tmp3Addr,RegAddress,TXBuf);
		  regflag3 = 1; 	
	  }	
	}		
}

/*获取特定地址的TMP117温度数据(警报模式；连续转换；ALERT低电平有效，反应报警标志位状态；8次平均值转换)
  DevAddress：需要更新的传感器地址
  TmpeBuf：温度数据缓存区域
*/
/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    getTMP117
  * @param   DevAddress: [输入/出] 
**			 TmpeBuf: [输入/出] 
  * @brief   获取温度值
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
void getTMP117(uint16_t DevAddress,uint8_t *TmpeBuf)
{
	switch(DevAddress)
  {
		case 0x01: 
		{
			if(regflag1 == 1)
			{
				HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp1Addr,&TMPEAddr,sizeof(TMPEAddr),20);
				HAL_I2C_Master_Transmit(&hi2c1,TMP117_Red_Tmp1Addr,&TMPEAddr,sizeof(TMPEAddr),20);
				HAL_I2C_Master_Receive(&hi2c1,TMP117_Red_Tmp1Addr,TmpeBuf,MemAddSize,20);
//				HAL_UART_Transmit(&huart3,TmpeBuf,2,0xffff);
				regflag1 = 0;
			}
			else
			{
				HAL_I2C_Master_Receive(&hi2c1,TMP117_Red_Tmp1Addr,TmpeBuf,MemAddSize,20);
//				HAL_UART_Transmit(&huart3,TmpeBuf,2,0xffff);
			}
		  break;
		}	
    case 0x02: 
		{
		  if(regflag2 == 1)
			{
				HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp2Addr,&TMPEAddr,sizeof(TMPEAddr),20);
				HAL_I2C_Master_Transmit(&hi2c1,TMP117_Red_Tmp2Addr,&TMPEAddr,sizeof(TMPEAddr),20);
				HAL_I2C_Master_Receive(&hi2c1,TMP117_Red_Tmp2Addr,TmpeBuf,MemAddSize,20);
//				HAL_UART_Transmit(&huart3,TmpeBuf,2,0xffff);
				regflag2 = 0;
			}
			else
			{
   		  HAL_I2C_Master_Receive(&hi2c1,TMP117_Red_Tmp2Addr,TmpeBuf,MemAddSize,20);
//				HAL_UART_Transmit(&huart3,TmpeBuf,2,0xffff);
			}
		  break;
		}	
		case 0x03:{
			if(regflag3 == 1)
			{
        HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp3Addr,&TMPEAddr,sizeof(TMPEAddr),20);
				HAL_I2C_Master_Transmit(&hi2c1,TMP117_Red_Tmp3Addr,&TMPEAddr,sizeof(TMPEAddr),20);
				HAL_I2C_Master_Receive(&hi2c1,TMP117_Red_Tmp3Addr,TmpeBuf,MemAddSize,20);
//				HAL_UART_Transmit(&huart3,TmpeBuf,2,0xffff);
				regflag3 = 0;
			}
			else
			{
   		  HAL_I2C_Master_Receive(&hi2c1,TMP117_Red_Tmp3Addr,TmpeBuf,MemAddSize,20);
//				HAL_UART_Transmit(&huart3,TmpeBuf,2,0xffff);
			}
			break;
		}		
    case 0x7F:{
			if(regflag1 == 0 && regflag2 == 0 && regflag3 == 0)
			{
				TmpeBuf[0] = 0xBB; TmpeBuf[1] = 0x00;  TmpeBuf[2] = 0x00;  TmpeBuf[3] = 0x01; 
				HAL_I2C_Master_Receive(&hi2c1,TMP117_Red_Tmp1Addr,&TmpeBuf[4],MemAddSize,20) ;
				HAL_I2C_Master_Receive(&hi2c1,TMP117_Red_Tmp2Addr,&TmpeBuf[6],MemAddSize,20) ;
				HAL_I2C_Master_Receive(&hi2c1,TMP117_Red_Tmp3Addr,&TmpeBuf[8],MemAddSize,20) ;
				HAL_Delay(1000);
//				HAL_UART_Transmit(&huart3,TmpeBuf,10,1000);
			}
			else
			{
				HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp1Addr,&TMPEAddr,sizeof(TMPEAddr),20);
				HAL_I2C_Master_Transmit(&hi2c1,TMP117_Red_Tmp1Addr,&TMPEAddr,sizeof(TMPEAddr),20);
				HAL_I2C_Master_Receive(&hi2c1,TMP117_Red_Tmp1Addr,&TmpeBuf[0],MemAddSize,20);
				regflag1 = 0;
				HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp2Addr,&TMPEAddr,sizeof(TMPEAddr),20);
				HAL_I2C_Master_Transmit(&hi2c1,TMP117_Red_Tmp2Addr,&TMPEAddr,sizeof(TMPEAddr),20);
				HAL_I2C_Master_Receive(&hi2c1,TMP117_Red_Tmp2Addr,&TmpeBuf[2],MemAddSize,20);
				regflag2 = 0;
				HAL_I2C_Master_Transmit(&hi2c1,TMP117_Wri_Tmp3Addr,&TMPEAddr,sizeof(TMPEAddr),20);
				HAL_I2C_Master_Transmit(&hi2c1,TMP117_Red_Tmp3Addr,&TMPEAddr,sizeof(TMPEAddr),20);
				HAL_I2C_Master_Receive(&hi2c1,TMP117_Red_Tmp3Addr,&TmpeBuf[4],MemAddSize,20);
				regflag3 = 0;
//				HAL_UART_Transmit(&huart3,TmpeBuf,6,20);
			}
		  break; 	
		}	
	}	
}


/*警报模标志位状态反馈
  低电平报警
*/
/*
void TempAlarm(void)
{
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2) == GPIO_PIN_RESET)
	{
//		printf("TMP117-1 ALARM");
//		printf("%02X %02X",aralm1[0],aralm1[1]);
		HAL_UART_Transmit(&huart3,aralm1,2,0xffff);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3) == GPIO_PIN_RESET)
		{
//			printf("TMP117-2 ALARM");
//			printf("%02X %02X",aralm2[0],aralm2[1]);
			HAL_UART_Transmit(&huart3,aralm2,2,0xffff);
		}
		else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == GPIO_PIN_RESET)
		{
//			printf("TMP117-3 ALARM");
//			printf("%02X %02X",aralm3[0],aralm3[1]);
			HAL_UART_Transmit(&huart3,aralm3,2,0xffff);
		}			
	}
	else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3) == GPIO_PIN_RESET)
	{
//		printf("TMP117-2 ALARM");
//		printf("%02X %02X",aralm2[0],aralm2[1]);
		HAL_UART_Transmit(&huart3,aralm2,2,0xffff);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == GPIO_PIN_RESET)
		{
//			printf("TMP117-3 ALARM");
//			printf("%02X %02X",aralm3[0],aralm3[1]);
			HAL_UART_Transmit(&huart3,aralm3,2,0xffff);
		}
	}
	else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == GPIO_PIN_RESET)
	{
//		printf("TMP117-3 ALARM");
//		printf("%02X %02X",aralm3[0],aralm3[1]);
		HAL_UART_Transmit(&huart3,aralm3,2,0xffff);
	}
}
*/

/*温度数据转换：精度0.0078125
  负温度为补码

void tempConversion(uint8_t *TmpeBuf)
{
	if(sizeof(TmpeBuf)>2)
  {
		if(TmpeBuf[1]<0x80)
    {
			
		}
	}
}
*/
