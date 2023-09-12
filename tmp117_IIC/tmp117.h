#ifndef __TMP117_H__
#define __TMP117_H__

#include "main.h"

//�豸��ַ
#define TMP117_Wri_Tmp1Addr                       0x90          //�¶ȴ�����1д�������ַ   �ӵ�
#define TMP117_Wri_Tmp2Addr                       0x92          //�¶ȴ�����2д�������ַ   �ӣ�
#define TMP117_Wri_Tmp3Addr                       0x94          //�¶ȴ�����3д�������ַ   ��SDA
#define TMP117_Red_Tmp1Addr                       0x91          //�¶ȴ�����1��ȡ������ַ
#define TMP117_Red_Tmp2Addr                       0x93          //�¶ȴ�����2��ȡ������ַ
#define TMP117_Red_Tmp3Addr                       0x95          //�¶ȴ�����3��ȡ������ַ
#define TMP117_Wri_zTmpAddr                       0x00          //��I2C����д������㲥��ַ
#define TMP117_Red_zTmpAddr                       0x01          //��I2C���߶�ȡ�����㲥��ַ

//�Ĵ�����ַ
#define TMP117_TempAddr                           0x00          //�¶Ƚ���Ĵ�����ַ			/*���һ���¶�ת��ֵ��1 LSB = 7.8125m��*/
#define TMP117_ConfigAddr                         0x01          //���üĴ�����ַ  
#define TMP117_UP_LIMIT                           0x02          //�����¶ȼĴ�����ַ
#define TMP117_DOWN_LIMIT                         0x03          //�����¶ȼĴ�����ַ
#define TMP117_OpenEEPAddr                        0x04          //EEPROM�����Ĵ�����ַ
#define TMP117_EEPROM1_Addr						  0x05			//EEPROM1 �Ĵ���
#define TMP117_EEPROM2_Addr						  0x06			//EEPROM2 �Ĵ���
#define TMP117_TEMP_OFFSET_Addr					  0x07			//�¶�ƫ�ƼĴ���
#define TMP117_EEPROM3_Addr						  0x08			//EEPROM3 �Ĵ���
#define TMP117_Device_ID_Addr					  0x0F			//���� ID �Ĵ���

#define TMP117_D30C                               0xF100        //��30�Ȳ���
#define TMP117_U60C                               0x1E00        //��60�Ȳ���
#define TMP117_RESOLUTION                         0.0078125     //ת������

#define TMP117_EEPROM_Open_HIGH                   0x80          //����EEPROM������Busy��0æµ
#define TMP117_EEPROM_Open_LOW                    0x00          //����EEPROM������Busy��0æµ
#define TMP117_EEPROM_Close_HIGH                  0x00          //����EEPROM������Busy��0����
#define TMP117_EEPROM_Close_LOW                   0x00          //����EEPROM������Busy��0����

#define TMP117_EEPROM_SetFunHOTCC_HIGH            0x03          //�������üĴ���(��ģʽ������ת����ALERT�͵�ƽ��Ч����Ӧ���ݾ�����־λ״̬��8��ƽ��ֵת��)Busy��0����
#define TMP117_EEPROM_SetFunHOTCC_LOW             0x34
#define TMP117_EEPROM_SetFunHOTOS_HIGH            0x0E          //�������üĴ���(��ģʽ������̬ת����ALERT�͵�ƽ��Ч����Ӧ���ݾ�����־λ״̬��8��ƽ��ֵת��)Busy��0����
#define TMP117_EEPROM_SetFunHOTOS_LOW             0x34
#define TMP117_EEPROM_SetFunALARMNotCC_HIGH       0x03          //�������üĴ���(����ģʽ������ת����ALERT�͵�ƽ��Ч����Ӧ���ݾ�����־λ״̬��8��ƽ��ֵת��)Busy��0����
#define TMP117_EEPROM_SetFunALARMNotCC_LOW        0x24
#define TMP117_EEPROM_SetFunALARMNotOS_HIGH       0x0E          //�������üĴ���(����ģʽ������̬ת����ALERT�͵�ƽ��Ч����Ӧ���ݾ�����־λ״̬��8��ƽ��ֵת��)Busy��0����
#define TMP117_EEPROM_SetFunALARMNotOS_LOW        0x24  
#define TMP117_EEPROM_SetFunALARMOpenCC_HIGH      0x03          //�������üĴ���(����ģʽ������ת����ALERT�͵�ƽ��Ч����Ӧ������־״̬��8��ƽ��ֵת��)Busy��0����
#define TMP117_EEPROM_SetFunALARMOpenCC_LOW       0x20 
#define TMP117_EEPROM_SetFunALARMOpenOS_HIGH      0x0E          //�������üĴ���(����ģʽ������̬ת����ALERT�͵�ƽ��Ч����Ӧ������־λ״̬��8��ƽ��ֵת��)Busy��0����
#define TMP117_EEPROM_SetFunALARMOpenOS_LOW       0x20



void   TMP117_Init(void);
void   updateTMP117(uint16_t DevAddress,uint16_t RegAddress,uint8_t *TXBuffer);
void   getTMP117(uint16_t DevAddress,uint8_t *RXBuffer);
//void   TempAlarm(void);

#endif
