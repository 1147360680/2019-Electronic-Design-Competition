#ifndef __DATA_PROCESS_H_
#define __DATA_PROCESS_H_

#include "DataType.h"

void get_capcity_value(void);

/* ��ȡ�ϼ��� ��ֵ*/
float get_top_capacity(void);

/* ��ȡ�¼��� ��ֵ */
float get_bottom_capacity(void);



void FDC2214_Data_Adjust(void);//����У׼

void Uart_Send_HMI(uint8 *cmd);
#endif



