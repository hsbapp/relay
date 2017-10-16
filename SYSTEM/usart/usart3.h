#ifndef _USART3_H_
#define _USART3_H_

#include "sys.h"

void uart3_init(u32 bound);
u16 uart3_data_length(void);
u8 uart3_get_data(void);
void uart3_send(u8 *buf, u16 len);

#endif


