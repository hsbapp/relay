#include "sys.h"
#include "usart.h"

#define USART3_RXBUF_SIZE	(200)
u8 USART3_RXBUF[USART3_RXBUF_SIZE];
u16 USART3_RXBUF_WP = 0;
u16 USART3_RXBUF_RP = 0;

void uart3_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	USART_DeInit(USART3);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
	
	GPIO_StructInit(&GPIO_InitStructure);

	//USART3_RX	  GPIOC.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	

	
	//USART3_TX   GPIOC.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
  
   //USART Init
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

#if 0
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;     
    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
    USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
    USART_ClockInit(USART3, &USART_ClockInitStructure);
#endif

	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	
	USART_Cmd(USART3, ENABLE);
	
	//Usart3 NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static u16 uart3_get_free(void)
{
	if (USART3_RXBUF_WP >= USART3_RXBUF_RP)
	{
		return USART3_RXBUF_SIZE - 1 - (USART3_RXBUF_WP - USART3_RXBUF_RP);
	} else {
		return USART3_RXBUF_RP - USART3_RXBUF_WP - 1;
	}
}

static void uart3_add_data(u8 data)
{
	USART3_RXBUF[USART3_RXBUF_WP] = data;
	USART3_RXBUF_WP++;
	
	if (USART3_RXBUF_WP == USART3_RXBUF_SIZE)
		USART3_RXBUF_WP = 0;
}

u16 uart3_data_length(void)
{
	if (USART3_RXBUF_WP >= USART3_RXBUF_RP)
	{
		return USART3_RXBUF_WP - USART3_RXBUF_RP;
	} else {
		return USART3_RXBUF_SIZE - (USART3_RXBUF_RP - USART3_RXBUF_WP);
	}
}

u8 uart3_get_data(void)
{
	u8 data = USART3_RXBUF[USART3_RXBUF_RP];
	
	USART3_RXBUF_RP++;
	if (USART3_RXBUF_RP == USART3_RXBUF_SIZE)
		USART3_RXBUF_RP = 0;
	
	return data;
}

void uart3_send(u8 *buf, u16 len)
{
	u16 cnt;
	
	for (cnt = 0; cnt < len; cnt++)
	{
		USART_SendData(USART3, buf[cnt]);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	}
}

void USART3_IRQHandler(void)
{
	u8 data;

	//printf("it\n");
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(USART3);
		//printf("get [%02X]\n", data);
		if (uart3_get_free() > 0)
			uart3_add_data(data);
	}
	//printf("exit\n");
}


