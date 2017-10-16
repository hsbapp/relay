#ifndef __LAN8720_H
#define __LAN8720_H
#include "sys.h"
#include "stm32f10x.h"
#include "stm32_eth.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32F407������
//LAN8720 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/8/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


#define LAN8720_PHY_ADDRESS  	0x0					//LAN8720 PHYоƬ��ַ.
#define LAN8720_RST 		   	PBout(10) 			//LAN8720��λ����	 


#define ETH_RXBUFNB        4
#define ETH_TXBUFNB        2

#define ETH_RX_BUF_SIZE    ETH_MAX_PACKET_SIZE //���ջ������Ĵ�С
#define ETH_TX_BUF_SIZE    ETH_MAX_PACKET_SIZE //���ͻ������Ĵ�С

typedef struct{
  u32 length;
  u32 buffer;
  __IO ETH_DMADESCTypeDef *descriptor;
}FrameTypeDef;










extern ETH_DMADESCTypeDef *DMARxDscrTab;			//��̫��DMA�������������ݽṹ��ָ��
extern ETH_DMADESCTypeDef *DMATxDscrTab;			//��̫��DMA�������������ݽṹ��ָ�� 
extern uint8_t *Rx_Buff; 							//��̫���ײ���������buffersָ�� 
extern uint8_t *Tx_Buff; 							//��̫���ײ���������buffersָ��
extern ETH_DMADESCTypeDef  *DMATxDescToSet;			//DMA����������׷��ָ��
extern ETH_DMADESCTypeDef  *DMARxDescToGet; 		//DMA����������׷��ָ�� 
//extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;	//DMA�����յ���֡��Ϣָ��
 

void ETHERNET_NVICConfiguration1(void);

u8 LAN8720_Init(void);
u8 LAN8720_Get_Speed(void);
u8 ETH_MACDMA_Config(void);

u8 Ethernet_Configuration(void);
uint32_t ETH_GetRxPktSize1(void);

FrameTypeDef ETH_Rx_Packet(void);
u8 ETH_Tx_Packet(u16 FrameLength);
u32 ETH_GetCurrentTxBuffer(void);
u8 ETH_Mem_Malloc(void);
void ETH_Mem_Free(void);
#endif 
