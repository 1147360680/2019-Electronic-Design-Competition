#ifndef __DATA_PROCESS_H_
#define __DATA_PROCESS_H_

#include "DataType.h"

typedef struct
{
	uint8 Status;      //���ʱ״̬
	uint8 ShortStatus; //��·��״̬
	uint8 Finish_Flag;  //��ȡ��ɵı�־
	uint8 PaperNumber;  //ֽ������
	float Capacitance;

	
}PaperCountEngine_Type;//ֽ�Ų�����״̬



extern PaperCountEngine_Type Paper; //SB


void get_capcity_value(void);

/* ��ȡ�ϼ��� ��ֵ*/
float get_top_capacity(void);

/* ��ȡ�¼��� ��ֵ */
float get_bottom_capacity(void);

/* ��У׼ʱ����ȡ������ ��ֵ */
float get_single_capacity(void);

void FDC2214_Data_Adjust(void);//����У׼

void Uart_Send_HMI(uint8 *cmd);

void	Get_Capcity_Value(void); //��ȡ����ֵ

void	Short_Circuit_Detection(void);//��·���


void DataSubsection(float CapacitanceSubsection[],float arrey[],int Number);

uint8 ProbablityCapacitance(float CompareArrey[])	;
#endif



