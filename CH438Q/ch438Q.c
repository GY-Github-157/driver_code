#include "ch438Q.h"


#define Fpclk    	  1843200               /* //666600 �����ڲ�ʱ��Ƶ��,Ĭ���ⲿ�����12��Ƶ    */
#define MaxRecvLen    50         	        /* ���ջ�������С    */
/*
	WR:	�͵�ƽ��Ч��Ĭ��Ϊ��
	RD���͵�ƽ��Ч��Ĭ��Ϊ��
	ALE:�ߵ�ƽ��Ч����ַ���棬Ĭ��Ϊ��
	RST������Ч��Ĭ��Ϊ��
	INT������Ч����©
	CS����·�У�Ĭ��Ϊ�ͣ�ѡ��״̬;
	AMOD��Ĭ��Ϊ�ߣ�ģʽΪ����ַ���÷�ʽ��ͨ��ALE�����ַ
*/
#define WR_OUT(N)	HAL_GPIO_WritePin(CH438_WR_GPIO_Port,CH438_WR_Pin,N)
#define RD_OUT(N)	HAL_GPIO_WritePin(CH438_RD_GPIO_Port,CH438_RD_Pin,N)
#define ALE_OUT(N)	HAL_GPIO_WritePin(CH438_ALE_GPIO_Port,CH438_ALE_Pin,N)	 
#define RST_OUT(N)	HAL_GPIO_WritePin(CH438_RST_GPIO_Port,CH438_RST_Pin,N)	  

const unsigned char offsetadd[]     = {0x00,0x10,0x20,0x30,0x08,0x18,0x28,0x38,};		/* ���ںŵ�ƫ�Ƶ�ַ */
const unsigned char Interruptnum[]  = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,};	    /* SSR�Ĵ����жϺŶ�Ӧֵ */

unsigned char Revbuff[MaxRecvLen];		/* ���ջ����� */
unsigned char RevLen;					/* ���ռ��� */


/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    HAL_GPIO_EXTI_Callback
  * @param   GPIO_Pin:  ���
  * @brief   GPIO�жϷ�������
  * @Data    2023-08-30
 **/
/* -------------------------------- end -------------------------------- */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == CH438_INT_Pin)
	{
		uint8_t gInterruptStatus;
		uint8_t InterruptStatus;
		uint8_t i;

		gInterruptStatus = CH438ReadReg( REG_SSR_ADDR );
//		printf("\r\nSSR: %02x\r\n",gInterruptStatus );
		if(!gInterruptStatus)
		{ 
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
			return ;
		}	
		
		for(i=0; i<8; i++)
		{
			if( gInterruptStatus & Interruptnum[i] )    /* ����ĸ����ڷ����ж� */
			{	
				InterruptStatus = CH438ReadReg( offsetadd[i] | REG_IIR_ADDR ) & 0x0f;    /* �����ڵ��ж�״̬ */	
				switch( InterruptStatus )
				{
					case INT_NOINT:			/* û���ж� */							
						break;
					case INT_THR_EMPTY:		/* THR���ж� */						
						break;
					case INT_RCV_OVERTIME:	/* ���ճ�ʱ�ж� */
					
						RevLen = CH438_RecvDatas(i, Revbuff);					

						/********************* ���յ����ݣ��������� *********/
						Parse_Ch438(i,Revbuff,RevLen);
					
						//printf("USART:%d ->",i);
						//printf("DATA:%s\r\n",Revbuff);
						memset(Revbuff,0,RevLen);
						break;
					case INT_RCV_SUCCESS:	/* �������ݿ����ж� */
						//RevLen = CH438_RecvDatas(i, Revbuff);
						//CH438_SendDatas(i, Revbuff, RevLen);
						break;
					case INT_RCV_LINES:		/* ������·״̬�ж� */
						//CH438ReadReg( offsetadd[i] | REG_LSR_ADDR );
						break;
					case INT_MODEM_CHANGE:	/* MODEM����仯�ж� */
						//CH438ReadReg( offsetadd[i] | REG_MSR_ADDR );
						break;
					default:
						break;

				}
			}
		}
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
												
	}
	
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    SetOutPut
  * @param   None
  * @brief   ��CH438_D0-D7����Ϊ������޸ĵ�·��˺������޸ģ�
  * @Data    2023-08-30
 **/
/* -------------------------------- end -------------------------------- */
void SetOutPut(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = CH438_D0_Pin|CH438_D1_Pin|CH438_D2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(CH438_D0_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = CH438_D3_Pin|CH438_D4_Pin|CH438_D5_Pin|CH438_D6_Pin|CH438_D7_Pin;
	HAL_GPIO_Init(CH438_D4_GPIO_Port, &GPIO_InitStruct);	
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    SetInPut
  * @param   None
  * @brief   ��CH438_D0-D7����Ϊ���루�޸ĵ�·��˺������޸ģ�
  * @Data    2023-08-30
 **/
/* -------------------------------- end -------------------------------- */
void SetInPut(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = CH438_D0_Pin|CH438_D1_Pin|CH438_D2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(CH438_D0_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = CH438_D3_Pin|CH438_D4_Pin|CH438_D5_Pin|CH438_D6_Pin|CH438_D7_Pin;
	HAL_GPIO_Init(CH438_D4_GPIO_Port, &GPIO_InitStruct);
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    SetPinLevel
  * @param   data: ���� /��ַ
  * @brief   ��8λ ���ݻ��ߵ�ַ �ֱ����Ӧ�� 8��������
  * @Data    2023-08-30
 **/
/* -------------------------------- end -------------------------------- */
void WritePinLevel(uint8_t data){
	
	uint8_t level = data;
	
	HAL_GPIO_WritePin(CH438_D0_GPIO_Port,CH438_D0_Pin,(level>>0)&0x1);
	HAL_GPIO_WritePin(CH438_D1_GPIO_Port,CH438_D1_Pin,(level>>1)&0x1);
	HAL_GPIO_WritePin(CH438_D2_GPIO_Port,CH438_D2_Pin,(level>>2)&0x1);
	HAL_GPIO_WritePin(CH438_D3_GPIO_Port,CH438_D3_Pin,(level>>3)&0x1);
	HAL_GPIO_WritePin(CH438_D4_GPIO_Port,CH438_D4_Pin,(level>>4)&0x1);
	HAL_GPIO_WritePin(CH438_D5_GPIO_Port,CH438_D5_Pin,(level>>5)&0x1);
	HAL_GPIO_WritePin(CH438_D6_GPIO_Port,CH438_D6_Pin,(level>>6)&0x1);
	HAL_GPIO_WritePin(CH438_D7_GPIO_Port,CH438_D7_Pin,(level>>7)&0x1);
	return ;
}


/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    ReadPinLevel
  * @param   None
  * @brief   ���� 8������ ��ƽ
  * @Data    2023-08-30
 **/
/* -------------------------------- end -------------------------------- */
uint8_t ReadPinLevel(void){
	uint8_t data=0;
	uint8_t re_data=0;
	
	data = HAL_GPIO_ReadPin(CH438_D7_GPIO_Port,CH438_D7_Pin);
	re_data = (re_data<<1)+ data;
	data = HAL_GPIO_ReadPin(CH438_D6_GPIO_Port,CH438_D6_Pin);
	re_data = (re_data<<1)+ data;
	data = HAL_GPIO_ReadPin(CH438_D5_GPIO_Port,CH438_D5_Pin);
	re_data = (re_data<<1)+ data;
	data = HAL_GPIO_ReadPin(CH438_D4_GPIO_Port,CH438_D4_Pin);
	re_data = (re_data<<1)+ data;
	data = HAL_GPIO_ReadPin(CH438_D3_GPIO_Port,CH438_D3_Pin);
	re_data = (re_data<<1)+ data;
	data = HAL_GPIO_ReadPin(CH438_D2_GPIO_Port,CH438_D2_Pin);
	re_data = (re_data<<1)+ data;
	data = HAL_GPIO_ReadPin(CH438_D1_GPIO_Port,CH438_D1_Pin);
	re_data = (re_data<<1)+ data;
	data = HAL_GPIO_ReadPin(CH438_D0_GPIO_Port,CH438_D0_Pin);
	re_data = (re_data<<1)+ data;
	
	return re_data;
}


/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438WriteReg
  * @param   add: ��ַ 
**			 data: ���� 
  * @brief   дһ���ֽڵ��Ĵ���
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
void CH438WriteReg(uint8_t add,uint8_t data)  
{
	WR_OUT(1);
	RD_OUT(1);
	SetOutPut();
	
	WritePinLevel(add); //�Ͱ�λʮ����λȷ���߰�λ���ݲ���  д�Ĵ�����ַ 

	ALE_OUT(1);
	HAL_Delay_us(1);
	ALE_OUT(0);		//������	 ��������
	
	WritePinLevel(data); 	//д����
	WR_OUT(0);
	HAL_Delay_us(1);
	WR_OUT(1);
	RD_OUT(1);
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438ReadReg
  * @param   add: reg��ַ 
  * @brief   ��ȡ�Ĵ����е�ֵ
  * @Data    2023-08-30
 **/
/* -------------------------------- end -------------------------------- */
uint8_t CH438ReadReg(uint8_t add) //��ȡһ���ֽ�
{
	uint8_t value;
	
	WR_OUT(1);
	RD_OUT(1);
	SetOutPut();   

	ALE_OUT(1);		//������	���ǵ�ַ
	WritePinLevel(add);	//����ַд����Ӧ������
	ALE_OUT(0);		//������	 ��������
	
	SetInPut();
	RD_OUT(0);
	value = ReadPinLevel();
	
	RD_OUT(1);
	
//	printf("value: %02x\r\n",value);
	return value;
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438_CheckIIR
  * @param   num: [����/��] 
  * @brief   ���IIR�Ĵ�����ֵ
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
unsigned char CH438_CheckIIR(unsigned char num)
{
	unsigned char value;
	
	value = CH438ReadReg( offsetadd[num] | REG_IIR_ADDR );
	return value;
}


/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438_CloseSeril
  * @param   num: [����/��] 
  * @brief   �ر�ĳλ����
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
void CH438_CloseSeril(unsigned char num) 
{
	CH438WriteReg(offsetadd[num]|REG_IER_ADDR, BIT_IER_LOWPOWER);
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438_CloseALLSeril
  * @param   None
  * @brief   �ر����д���
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
void CH438_CloseALLSeril(void) 
{
	CH438WriteReg(offsetadd[0]|REG_IER_ADDR, BIT_IER_LOWPOWER|BIT_IER_SLP);
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438_ResetSeril
  * @param   num: [����/��] 
  * @brief   ��λ����
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
void CH438_ResetSeril(unsigned char num)
{
	CH438WriteReg(offsetadd[num]|REG_IER_ADDR, BIT_IER_RESET);
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438_INTConfig
  * @param   num:  
  * @brief   �����жϵĴ�����ʽ
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
void CH438_INTConfig(unsigned char num)
{	
	/* ע��: CH438��BIT_IER_IETHRE�ж�(0->1),�����һ���������ж� */	
	CH438WriteReg(offsetadd[num]|REG_IER_ADDR,BIT_IER_IERECV );
	CH438_CheckIIR(num);
	CH438WriteReg(offsetadd[num]|REG_MCR_ADDR, BIT_MCR_OUT2);//    | BIT_MCR_RTS     | BIT_MCR_DTR);//���Բ���һ��ʵ�ʵ��ж�	
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438_SendDatas
  * @param   num:  
**			 sendbuff: 
**			 len:  
  * @brief   ��ָ���Ĵ��ں� ��������
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
void CH438_SendDatas(unsigned char num, unsigned char* sendbuff,unsigned char len)
{

	while(len)
	{		
		if((CH438ReadReg(offsetadd[num]|REG_LSR_ADDR)&BIT_LSR_TEMT))	    //LSR->THRE==1  ���ּĴ�����
		{
			CH438WriteReg(offsetadd[num]|REG_THR_ADDR, *sendbuff++);
			len--;
		}
	}
}

//
/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438_RecvDatas
  * @param   num: [����/��] 
**			 revbuff: [����/��] 
  * @brief   ��ȡָ��λ�Ŵ��ڵ�����
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
unsigned char CH438_RecvDatas(unsigned char num, unsigned char* revbuff)
{
	uint8_t len=0;
	uint8_t *p_rev;
	
	p_rev = revbuff;
 
	while( ( CH438ReadReg( offsetadd[num]|REG_LSR_ADDR ) & BIT_LSR_DATARDY ) == 0 );    /*�ȴ�����׼���� */
	while((CH438ReadReg(offsetadd[num]|REG_LSR_ADDR)&BIT_LSR_DATARDY))	//LSR->DATARDY==1
	{
		*p_rev = CH438ReadReg(offsetadd[num]|REG_RBR_ADDR);
		p_rev++;
		len++;
	}
	return len;
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438_RegTEST
  * @param   num: λ��
  * @brief   ����ʹ�õĺ���
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
void CH438_RegTEST(unsigned char num)
{
	printf("current test serilnum:  %02x \r\n",(unsigned short)offsetadd[num]);
	printf("IER: %02x\r\n",(unsigned short)CH438ReadReg(offsetadd[num] | REG_IER_ADDR));//?IER
	printf("IIR: %02x\r\n",(unsigned short)CH438ReadReg(offsetadd[num] | REG_IIR_ADDR));//?IIR
	printf("LCR: %02x\r\n",(unsigned short)CH438ReadReg(offsetadd[num] | REG_LCR_ADDR));//?LCR
	printf("MCR: %02x\r\n",(unsigned short)CH438ReadReg(offsetadd[num] | REG_MCR_ADDR));//?MCR
	printf("LSR: %02x\r\n",(unsigned short)CH438ReadReg(offsetadd[num] | REG_LSR_ADDR));//?LSR
	printf("MSR: %02x\r\n",(unsigned short)CH438ReadReg(offsetadd[num] | REG_MSR_ADDR));//?MSR
//	CH438WriteReg(offsetadd[num] | REG_SCR_ADDR, 0x78);
	printf("SCR: %02x\r\n",(unsigned short)CH438ReadReg(offsetadd[num] | REG_SCR_ADDR));//?SCR
	printf("FCR: %02x\r\n",(unsigned short)CH438ReadReg(offsetadd[num] | REG_FCR_ADDR));//?SCR
	//printf("SSR: %02x\r\n",(unsigned short)CH438ReadReg( offsetadd[num] | REG_SSR_ADDR ));//?SCR
}


//���ڳ�ʼ������ ������� ���ںźͲ�����
/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438_Uart_Init
  * @param   num: [����/��] 
**			 value: [����/��] 
  * @brief  
  * @Data    2023-08-30
 **/
/* -------------------------------- end -------------------------------- */
void CH438_Uart_Init(unsigned char num,unsigned long value)
{
	RST_OUT(1);			//оƬʹ��
 
	uint8_t dlab=0;
	uint16_t bandspeed;
	dlab = CH438ReadReg(offsetadd[num]|REG_IER_ADDR);
	dlab &= 0xDF;
	CH438WriteReg(offsetadd[num]|REG_IER_ADDR, dlab);
	
	dlab = CH438ReadReg(offsetadd[num]|REG_LCR_ADDR);
	dlab |= 0x80;		//��LCR�Ĵ���DLABλΪ1
	CH438WriteReg(offsetadd[num]|REG_LCR_ADDR, dlab);
	
	bandspeed = Fpclk/16/value;
	CH438WriteReg(offsetadd[num]|REG_DLL_ADDR, (uint8_t)bandspeed);
	CH438WriteReg(offsetadd[num]|REG_DLM_ADDR, (uint8_t)(bandspeed>>8));
	dlab &= 0x7F;		//��IIR�Ĵ���DLABλΪ0
	CH438WriteReg(offsetadd[num]|REG_LCR_ADDR, dlab);
	CH438WriteReg(offsetadd[num]|REG_FCR_ADDR,BIT_FCR_RECVTG1 | BIT_FCR_RECVTG0 | BIT_FCR_FIFOEN );
	CH438WriteReg(offsetadd[num]|REG_LCR_ADDR,BIT_LCR_WORDSZ1 | BIT_LCR_WORDSZ0 );
	//CH438WriteReg(offsetadd[num]|REG_IER_ADDR,BIT_IER_IELINES | BIT_IER_IETHRE | BIT_IER_IERECV);
	//CH438WriteReg(offsetadd[num]|REG_MCR_ADDR,BIT_MCR_OUT2    | BIT_MCR_RTS     | BIT_MCR_DTR);
	CH438WriteReg(offsetadd[num]|REG_FCR_ADDR,CH438ReadReg(offsetadd[num]|REG_FCR_ADDR)| BIT_FCR_TFIFORST|BIT_FCR_RFIFORST);
    //CH438_TranConfig(num);
	CH438_INTConfig(num);
}
