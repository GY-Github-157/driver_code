#include "ch438Q.h"


#define Fpclk    	  1843200               /* //666600 定义内部时钟频率,默认外部晶振的12分频    */
#define MaxRecvLen    50         	        /* 接收缓冲区大小    */
/*
	WR:	低电平有效，默认为高
	RD：低电平有效，默认为高
	ALE:高电平有效，地址锁存，默认为低
	RST：低有效，默认为低
	INT：低有效，开漏
	CS：电路中，默认为低，选中状态;
	AMOD：默认为高，模式为，地址复用方式，通过ALE锁存地址
*/
#define WR_OUT(N)	HAL_GPIO_WritePin(CH438_WR_GPIO_Port,CH438_WR_Pin,N)
#define RD_OUT(N)	HAL_GPIO_WritePin(CH438_RD_GPIO_Port,CH438_RD_Pin,N)
#define ALE_OUT(N)	HAL_GPIO_WritePin(CH438_ALE_GPIO_Port,CH438_ALE_Pin,N)	 
#define RST_OUT(N)	HAL_GPIO_WritePin(CH438_RST_GPIO_Port,CH438_RST_Pin,N)	  

const unsigned char offsetadd[]     = {0x00,0x10,0x20,0x30,0x08,0x18,0x28,0x38,};		/* 串口号的偏移地址 */
const unsigned char Interruptnum[]  = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,};	    /* SSR寄存器中断号对应值 */

unsigned char Revbuff[MaxRecvLen];		/* 接收缓存区 */
unsigned char RevLen;					/* 接收计数 */


/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    HAL_GPIO_EXTI_Callback
  * @param   GPIO_Pin:  句柄
  * @brief   GPIO中断反馈函数
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
			if( gInterruptStatus & Interruptnum[i] )    /* 检测哪个串口发生中断 */
			{	
				InterruptStatus = CH438ReadReg( offsetadd[i] | REG_IIR_ADDR ) & 0x0f;    /* 读串口的中断状态 */	
				switch( InterruptStatus )
				{
					case INT_NOINT:			/* 没有中断 */							
						break;
					case INT_THR_EMPTY:		/* THR空中断 */						
						break;
					case INT_RCV_OVERTIME:	/* 接收超时中断 */
					
						RevLen = CH438_RecvDatas(i, Revbuff);					

						/********************* 接收到数据，处理数据 *********/
						Parse_Ch438(i,Revbuff,RevLen);
					
						//printf("USART:%d ->",i);
						//printf("DATA:%s\r\n",Revbuff);
						memset(Revbuff,0,RevLen);
						break;
					case INT_RCV_SUCCESS:	/* 接收数据可用中断 */
						//RevLen = CH438_RecvDatas(i, Revbuff);
						//CH438_SendDatas(i, Revbuff, RevLen);
						break;
					case INT_RCV_LINES:		/* 接收线路状态中断 */
						//CH438ReadReg( offsetadd[i] | REG_LSR_ADDR );
						break;
					case INT_MODEM_CHANGE:	/* MODEM输入变化中断 */
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
  * @brief   将CH438_D0-D7设置为输出（修改电路后此函数需修改）
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
  * @brief   将CH438_D0-D7设置为输入（修改电路后此函数需修改）
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
  * @param   data: 数据 /地址
  * @brief   将8位 数据或者地址 分别传入对应的 8个引脚上
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
  * @brief   读入 8个引脚 电平
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
  * @param   add: 地址 
**			 data: 数据 
  * @brief   写一个字节到寄存器
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
void CH438WriteReg(uint8_t add,uint8_t data)  
{
	WR_OUT(1);
	RD_OUT(1);
	SetOutPut();
	
	WritePinLevel(add); //低八位十数据位确保高八位数据不变  写寄存器地址 

	ALE_OUT(1);
	HAL_Delay_us(1);
	ALE_OUT(0);		//接下来	 的是数据
	
	WritePinLevel(data); 	//写数据
	WR_OUT(0);
	HAL_Delay_us(1);
	WR_OUT(1);
	RD_OUT(1);
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438ReadReg
  * @param   add: reg地址 
  * @brief   读取寄存器中的值
  * @Data    2023-08-30
 **/
/* -------------------------------- end -------------------------------- */
uint8_t CH438ReadReg(uint8_t add) //读取一个字节
{
	uint8_t value;
	
	WR_OUT(1);
	RD_OUT(1);
	SetOutPut();   

	ALE_OUT(1);		//接下来	的是地址
	WritePinLevel(add);	//将地址写到对应的引脚
	ALE_OUT(0);		//接下来	 的是数据
	
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
  * @param   num: [输入/出] 
  * @brief   检测IIR寄存器的值
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
  * @param   num: [输入/出] 
  * @brief   关闭某位串口
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
  * @brief   关闭所有串口
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
  * @param   num: [输入/出] 
  * @brief   复位串口
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
  * @brief   配置中断的触发方式
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
void CH438_INTConfig(unsigned char num)
{	
	/* 注意: CH438打开BIT_IER_IETHRE中断(0->1),会产生一个发生空中断 */	
	CH438WriteReg(offsetadd[num]|REG_IER_ADDR,BIT_IER_IERECV );
	CH438_CheckIIR(num);
	CH438WriteReg(offsetadd[num]|REG_MCR_ADDR, BIT_MCR_OUT2);//    | BIT_MCR_RTS     | BIT_MCR_DTR);//可以产生一个实际的中断	
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438_SendDatas
  * @param   num:  
**			 sendbuff: 
**			 len:  
  * @brief   向指定的串口号 发送数据
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
void CH438_SendDatas(unsigned char num, unsigned char* sendbuff,unsigned char len)
{

	while(len)
	{		
		if((CH438ReadReg(offsetadd[num]|REG_LSR_ADDR)&BIT_LSR_TEMT))	    //LSR->THRE==1  保持寄存器空
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
  * @param   num: [输入/出] 
**			 revbuff: [输入/出] 
  * @brief   读取指定位号串口的数据
  * @Data    2023-08-31
 **/
/* -------------------------------- end -------------------------------- */
unsigned char CH438_RecvDatas(unsigned char num, unsigned char* revbuff)
{
	uint8_t len=0;
	uint8_t *p_rev;
	
	p_rev = revbuff;
 
	while( ( CH438ReadReg( offsetadd[num]|REG_LSR_ADDR ) & BIT_LSR_DATARDY ) == 0 );    /*等待数据准备好 */
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
  * @param   num: 位号
  * @brief   测试使用的函数
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


//串口初始化函数 输入参数 串口号和波特率
/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    CH438_Uart_Init
  * @param   num: [输入/出] 
**			 value: [输入/出] 
  * @brief  
  * @Data    2023-08-30
 **/
/* -------------------------------- end -------------------------------- */
void CH438_Uart_Init(unsigned char num,unsigned long value)
{
	RST_OUT(1);			//芯片使能
 
	uint8_t dlab=0;
	uint16_t bandspeed;
	dlab = CH438ReadReg(offsetadd[num]|REG_IER_ADDR);
	dlab &= 0xDF;
	CH438WriteReg(offsetadd[num]|REG_IER_ADDR, dlab);
	
	dlab = CH438ReadReg(offsetadd[num]|REG_LCR_ADDR);
	dlab |= 0x80;		//置LCR寄存器DLAB位为1
	CH438WriteReg(offsetadd[num]|REG_LCR_ADDR, dlab);
	
	bandspeed = Fpclk/16/value;
	CH438WriteReg(offsetadd[num]|REG_DLL_ADDR, (uint8_t)bandspeed);
	CH438WriteReg(offsetadd[num]|REG_DLM_ADDR, (uint8_t)(bandspeed>>8));
	dlab &= 0x7F;		//置IIR寄存器DLAB位为0
	CH438WriteReg(offsetadd[num]|REG_LCR_ADDR, dlab);
	CH438WriteReg(offsetadd[num]|REG_FCR_ADDR,BIT_FCR_RECVTG1 | BIT_FCR_RECVTG0 | BIT_FCR_FIFOEN );
	CH438WriteReg(offsetadd[num]|REG_LCR_ADDR,BIT_LCR_WORDSZ1 | BIT_LCR_WORDSZ0 );
	//CH438WriteReg(offsetadd[num]|REG_IER_ADDR,BIT_IER_IELINES | BIT_IER_IETHRE | BIT_IER_IERECV);
	//CH438WriteReg(offsetadd[num]|REG_MCR_ADDR,BIT_MCR_OUT2    | BIT_MCR_RTS     | BIT_MCR_DTR);
	CH438WriteReg(offsetadd[num]|REG_FCR_ADDR,CH438ReadReg(offsetadd[num]|REG_FCR_ADDR)| BIT_FCR_TFIFORST|BIT_FCR_RFIFORST);
    //CH438_TranConfig(num);
	CH438_INTConfig(num);
}
