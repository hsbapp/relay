
#include "adc.h"
#include "delay.h"
#include <stdio.h>

#define CTRL01	PBout(14)
#define CTRL02	PBout(15)
#define CTRL03	PCout(6)
#define CTRL04	PCout(7)
#define CTRL05	PCout(8)
#define CTRL06	PCout(9)
#define CTRL07	PAout(8)
#define CTRL08	PCout(12)
#define CTRL09	PDout(2)
#define CTRL10	PBout(3)
#define CTRL11	PBout(4)
#define CTRL12	PBout(5)

#define N	50
#define M	8
#define THRESHOLD	(150)

vu16 AD_Value[N][M];
vu16 Average[M];

static void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
												RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO,
												ENABLE);
	
	// remap
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	
	// PA 0/3/4/5/6 for AD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PB 0/1 for AD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// PC 0/2/3 for AD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// PA 8 for Ctrl
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PB 3/4/5/14/15 for Ctrl
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// PC 6/7/8/9/12 for Ctrl
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// PD 2 for Ctrl
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

static void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = N * M;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
}

static void RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	  //使能ADC1通道时钟

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void  ADC1_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure; 

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = M;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 2, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 3, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 4, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 5, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 6, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 7, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 8, ADC_SampleTime_239Cycles5 );
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 9, ADC_SampleTime_239Cycles5 );
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 10, ADC_SampleTime_239Cycles5 );
	
	ADC_DMACmd(ADC1, ENABLE);
  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
}


void ADC1_Start(void)
{
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void adc_init(void)
{
	RCC_Configuration();
	GPIO_Configuration();
	ADC1_Configuration();
	DMA_Configuration();
	
	SetStatus(9, 0);
	SetStatus(10, 0);
	SetStatus(11, 0);
	SetStatus(12, 0);
	
	ADC1_Start();
}

u16 Get_Adc(u8 ch)
{
	u32 i, sum = 0;
	u16 val = 0;
	
	for (i = 0; i < N; i++)
	{
		sum += AD_Value[i][ch];
	}
	
	val = sum / N;

	return val;
}

u16 Get_Adc2(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

void Update_Adc_Average(void)
{	
	u8 i;
#if 0
	u8 t;
	u32 temp[M];

	for (i = 0; i < M; i++)
		temp[i] = 0;
	
	for (t = 0; t < 5; t++)
	{
		for (i = 0; i < M; i++)
		{
			temp[i] += Get_Adc(i);
		}

		if (t < 4)
			delay_ms(4);
	}
	
	for (i = 0; i < M; i++)
	{
		Average[i] = temp[i] / 5;
		//printf("average[%d]=%d\n", i, Average[i]);
	}
#else
	for (i = 0; i < M; i++)
	{
		Average[i] = Get_Adc(i);
		//printf("average[%d]=%d\n", i, Average[i]);
	}
#endif
} 	 

#define REVERT(x)	(x > 0 ? 0 : 1)

void RevertStatus(u8 ch)
{
	switch (ch)
	{
		case 1:
			CTRL01 = REVERT(CTRL01);
			break;
		case 2:
			CTRL02 = REVERT(CTRL02);
			break;
		case 3:
			CTRL03 = REVERT(CTRL03);
			break;
		case 4:
			CTRL04 = REVERT(CTRL04);
			break;
		case 5:
			CTRL05 = REVERT(CTRL05);
			break;
		case 6:
			CTRL06 = REVERT(CTRL06);
			break;
		case 7:
			CTRL07 = REVERT(CTRL07);
			break;
		case 8:
			CTRL08 = REVERT(CTRL08);
			break;
		case 9:
			CTRL09 = REVERT(CTRL09);
			break;
		case 10:
			CTRL10 = REVERT(CTRL10);
			break;
		case 11:
			CTRL11 = REVERT(CTRL11);
			break;
		case 12:
			CTRL12 = REVERT(CTRL12);
			break;
		default:
			break;
	}
}

u16 GetStatus(u8 ch)
{
	u16 ret = 0;
	
	if (ch <= 8 && ch >= 1) {
		ret =  Average[ch-1] > THRESHOLD ? 1 : 0;
	} else {
		switch (ch)
		{
			case 9:
				ret = CTRL09 > 0 ? 1 : 0;
				break;
			case 10:
				ret = CTRL10 > 0 ? 1 : 0;
				break;
			case 11:
				ret = CTRL11 > 0 ? 1 : 0;
				break;
			case 12:
				ret = CTRL12 > 0 ? 1 : 0;
				break;
			default:
				break;
		}
	}
	
	return ret;
}

void SetStatus(u8 ch, u16 value)
{
	u16 status = 0;
	
	if (ch <= 8 && ch >= 1)
	{
		status = Average[ch-1] > THRESHOLD ? 1 : 0;
		if (value == status)
			return;
		
		RevertStatus(ch);
	} else {
		switch (ch)
		{
			case 9:
				CTRL09 = value;
				break;
			case 10:
				CTRL10 = value;
				break;
			case 11:
				CTRL11 = value;
				break;
			case 12:
				CTRL12 = value;
				break;
			default:
				break;
		}
	}
}

void SetGPIO(u8 ch, u16 value)
{
	switch (ch)
		{
			case 1:
				CTRL01 = value;
				break;
			case 2:
				CTRL02 = value;
				break;
			case 3:
				CTRL03 = value;
				break;
			case 4:
				CTRL04 = value;
				break;
			case 5:
				CTRL05 = value;
				break;
			case 6:
				CTRL06 = value;
				break;
			case 7:
				CTRL07 = value;
				break;
			case 8:
				CTRL08 = value;
				break;
			case 9:
				CTRL09 = value;
				break;
			case 10:
				CTRL10 = value;
				break;
			case 11:
				CTRL11 = value;
				break;
			case 12:
				CTRL12 = value;
				break;
			default:
				break;
		}
}

static u8 get_double_status(void)
{
	u8 i = 0;
	u8 ret = 0;
	
	for (i = 0; i < 8; i++)
	{
		if (GetStatus(i + 1) > 0) {
			ret |= (1 << i);
		}
	}
	
	return ret;
}

#define ADC_SERVER_PRIO		10
//任务堆栈大小
#define ADC_SERVER_STK_SIZE	300
//任务堆栈
OS_STK ADC_SERVER_TASK_STK[ADC_SERVER_STK_SIZE];

static void adc_server_thread(void *arg)
{
    u8 status = 0;
		u8 tmp = 0;
	  int cnt = 0;
	
    //OS_CPU_SR cpu_sr;
    while (1) {
        Update_Adc_Average();
			
				tmp = get_double_status();
			
				if (tmp != status) {
					status = tmp;
					printf("status=%x\r\n", tmp);
				}
			
        delay_ms(100);
				//printf("x=%x", tmp);
				cnt++;
				
				if (cnt >= 10) {
					printf("tmp=%x\r\n", tmp);
					cnt = 0;
				}
		}
}

u8 adc_server_init(void)
{
    OS_CPU_SR cpu_sr;
    u8 res=0;

	printf("a1");
	
	OS_ENTER_CRITICAL();
	res = OSTaskCreate(adc_server_thread,
						(void*)0,
						(OS_STK*)&ADC_SERVER_TASK_STK[ADC_SERVER_STK_SIZE-1],
						ADC_SERVER_PRIO);

	printf("adc_server_thread:%d\r\n",res);
	OS_EXIT_CRITICAL();		//开中断
	
	printf("a3");
						
	return res;
}
