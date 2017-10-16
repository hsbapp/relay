/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "usart.h"
#include "lwip_comm.h"
#include "stm32_eth.h"

extern void Time_Update(void);
extern u32 lwip_localtime;	//lwip本地时间计数器,单位:ms
extern void lwip_pkt_handle(void);

 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
//void PendSV_Handler(void)
//{
//}
 


/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
}

/**
  * @brief  This function handles ETH interrupt request.
  * @param  None
  * @retval None
  */
extern void lwip_pkt_handle(void);		//在lwip_comm.c里面定义
//以太网DMA接收中断服务函数
void ETH_IRQHandler(void)
{
	while(ETH_GetRxPktSize1()!=0) 	//检测是否收到数据包
	{ 
		lwip_pkt_handle();		
	}
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R); 	//清除DMA中断标志位
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);	//清除DMA接收中断标志位
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		lwip_localtime +=10; //加10
		Time_Update();
		lwip_periodic_handle();	//LWIP内核需要定时处理的函数
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
