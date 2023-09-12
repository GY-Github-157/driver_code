#ifndef __TMP117_H__
#define __TMP117_H__

#include "main.h"

//设备地址
#define TMP117_Wri_Tmp1Addr                       0x90          //温度传感器1写入操作地址   接地
#define TMP117_Wri_Tmp2Addr                       0x92          //温度传感器2写入操作地址   接＋
#define TMP117_Wri_Tmp3Addr                       0x94          //温度传感器3写入操作地址   接SDA
#define TMP117_Red_Tmp1Addr                       0x91          //温度传感器1读取操作地址
#define TMP117_Red_Tmp2Addr                       0x93          //温度传感器2读取操作地址
#define TMP117_Red_Tmp3Addr                       0x95          //温度传感器3读取操作地址
#define TMP117_Wri_zTmpAddr                       0x00          //单I2C总线写入操作广播地址
#define TMP117_Red_zTmpAddr                       0x01          //单I2C总线读取操作广播地址

//寄存器地址
#define TMP117_TempAddr                           0x00          //温度结果寄存器地址			/*最近一次温度转换值，1 LSB = 7.8125m℃*/
#define TMP117_ConfigAddr                         0x01          //配置寄存器地址  
#define TMP117_UP_LIMIT                           0x02          //上限温度寄存器地址
#define TMP117_DOWN_LIMIT                         0x03          //下限温度寄存器地址
#define TMP117_OpenEEPAddr                        0x04          //EEPROM解锁寄存器地址
#define TMP117_EEPROM1_Addr						  0x05			//EEPROM1 寄存器
#define TMP117_EEPROM2_Addr						  0x06			//EEPROM2 寄存器
#define TMP117_TEMP_OFFSET_Addr					  0x07			//温度偏移寄存器
#define TMP117_EEPROM3_Addr						  0x08			//EEPROM3 寄存器
#define TMP117_Device_ID_Addr					  0x0F			//器件 ID 寄存器

#define TMP117_D30C                               0xF100        //负30度参数
#define TMP117_U60C                               0x1E00        //正60度参数
#define TMP117_RESOLUTION                         0.0078125     //转换精度

#define TMP117_EEPROM_Open_HIGH                   0x80          //配置EEPROM解锁，Busy置0忙碌
#define TMP117_EEPROM_Open_LOW                    0x00          //配置EEPROM解锁，Busy置0忙碌
#define TMP117_EEPROM_Close_HIGH                  0x00          //配置EEPROM上锁，Busy置0空闲
#define TMP117_EEPROM_Close_LOW                   0x00          //配置EEPROM上锁，Busy置0空闲

#define TMP117_EEPROM_SetFunHOTCC_HIGH            0x03          //设置配置寄存器(热模式；连续转换；ALERT低电平有效，反应数据就绪标志位状态；8次平均值转换)Busy置0空闲
#define TMP117_EEPROM_SetFunHOTCC_LOW             0x34
#define TMP117_EEPROM_SetFunHOTOS_HIGH            0x0E          //设置配置寄存器(热模式；单稳态转换；ALERT低电平有效，反应数据就绪标志位状态；8次平均值转换)Busy置0空闲
#define TMP117_EEPROM_SetFunHOTOS_LOW             0x34
#define TMP117_EEPROM_SetFunALARMNotCC_HIGH       0x03          //设置配置寄存器(警报模式；连续转换；ALERT低电平有效，反应数据就绪标志位状态；8次平均值转换)Busy置0空闲
#define TMP117_EEPROM_SetFunALARMNotCC_LOW        0x24
#define TMP117_EEPROM_SetFunALARMNotOS_HIGH       0x0E          //设置配置寄存器(警报模式；单稳态转换；ALERT低电平有效，反应数据就绪标志位状态；8次平均值转换)Busy置0空闲
#define TMP117_EEPROM_SetFunALARMNotOS_LOW        0x24  
#define TMP117_EEPROM_SetFunALARMOpenCC_HIGH      0x03          //设置配置寄存器(警报模式；连续转换；ALERT低电平有效，反应报警标志状态；8次平均值转换)Busy置0空闲
#define TMP117_EEPROM_SetFunALARMOpenCC_LOW       0x20 
#define TMP117_EEPROM_SetFunALARMOpenOS_HIGH      0x0E          //设置配置寄存器(警报模式；单稳态转换；ALERT低电平有效，反应报警标志位状态；8次平均值转换)Busy置0空闲
#define TMP117_EEPROM_SetFunALARMOpenOS_LOW       0x20



void   TMP117_Init(void);
void   updateTMP117(uint16_t DevAddress,uint16_t RegAddress,uint8_t *TXBuffer);
void   getTMP117(uint16_t DevAddress,uint8_t *RXBuffer);
//void   TempAlarm(void);

#endif
