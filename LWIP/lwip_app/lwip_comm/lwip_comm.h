#ifndef _LWIP_COMM_H
#define _LWIP_COMM_H 
//#include "enc28j60.h" 
#include "stm32f10x.h"

#define LWIP_MAX_DHCP_TRIES		4   //DHCP������������Դ���
   
//lwip���ƽṹ��
typedef struct  
{
	u8 mac[6];      //MAC��ַ
	u8 remoteip[4];	//Զ������IP��ַ 
	u8 ip[4];       //����IP��ַ
	u8 netmask[4]; 	//��������
	u8 gateway[4]; 	//Ĭ�����ص�IP��ַ
	
	vu8 dhcpstatus;	//dhcp״̬ 
					//0,δ��ȡDHCP��ַ;
					//1,����DHCP��ȡ״̬
					//2,�ɹ���ȡDHCP��ַ
					//0XFF,��ȡʧ��.
}__lwip_dev;
extern __lwip_dev lwipdev;	//lwip���ƽṹ��

u8 lwip_comm_mem_malloc(void);
void lwip_comm_mem_free(void);
void lwip_comm_default_ip_set(__lwip_dev *lwipx);
u8 lwip_comm_init(void);
void lwip_pkt_handle(void);
void lwip_periodic_handle(void);
void lwip_dhcp_process_handle(void);

#endif












