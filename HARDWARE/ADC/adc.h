
#ifndef _ADC_H_
#define _ADC_H_
#include "sys.h"
#include "includes.h"

#define TOTAL_STATUS_NUM	(12)

u16 Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 

void adc_init(void);
void Update_Adc_Average(void);
u16 GetStatus(u8 ch);
void SetStatus(u8 ch, u16 value);

void SetGPIO(u8 ch, u16 value);

u8 adc_server_init(void);

#endif 
