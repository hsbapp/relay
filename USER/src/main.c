
#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "sram.h"
#include "malloc.h"
#include "lan8720.h" 	 
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "timer.h"
#include "string.h"
#include "adc.h"
#include "includes.h"
#include "tcp_client.h"
#include "tcp_server.h"


#define SYSTEMTICK_PERIOD_MS  10
#define ETHERNET

__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */

void Time_Update(void);
void delay_ms_T3(uint32_t ms);

//�ڵ��Դ��ڴ�ӡ��ַ��Ϣ
//mode:1 ��ʾDHCP��ȡ���ĵ�ַ
//	  ���� ��ʾ��̬��ַ
void show_address(u8 mode)
{
	u8 buf[30];
	if(mode==1)
	{
		sprintf((char*)buf,"MAC    :%d.%d.%d.%d.%d.%d",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);//��ӡMAC��ַ
		printf("MAC    : %s \r\n",buf); 
		sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
		printf("DHCP IP: %s \r\n",buf); 
		sprintf((char*)buf,"DHCP GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
		printf("DHCP GW: %s \r\n",buf); 
		sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
		printf("DHCP IP: %s \r\n",buf); 
	}
	else 
	{
		sprintf((char*)buf,"MAC      :%d.%d.%d.%d.%d.%d",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);//��ӡMAC��ַ
		printf("MAC      : %s \r\n",buf); 
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
		printf("Static IP: %s \r\n",buf); 
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
		printf("Static GW: %s \r\n",buf); 
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
		printf("Static IP: %s \r\n",buf); 
	}	
}

int main(void)
{
	delay_init();	    	 	                         //��ʱ������ʼ��

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	uart_init(115200);	 							   //���ڳ�ʼ��Ϊ115200

	printf("s1\n");
	
	adc_init();
	
	TIM3_Int_Init(1000, 719);	                     //��ʱ��3Ƶ��Ϊ100hz

	printf("s2\n");
	
	my_mem_init(SRAMIN);		                       //��ʼ���ڲ��ڴ��

	printf("s3\n");
	
	OSInit();				//UCOS��ʼ��
	
	printf("s4\n");
#ifdef ETHERNET
	while(lwip_comm_init())
	{
			delay_ms(100);
	}

	printf("s5\n");
	
	tcp_server_init();
	
#endif
	printf("s5.1");
	
	adc_server_init();
	printf("s6\n");

	OSStart(); //����UCOS
	while(1){};
}

/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
void Time_Update(void)
{
  LocalTime += SYSTEMTICK_PERIOD_MS;
}


