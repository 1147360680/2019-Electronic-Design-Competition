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



void get_capcity_value(void);

/* ��У׼ʱ����ȡ������ ��ֵ */
float get_single_capacity(void);

void FDC2214_Data_Adjust(void);//����У׼

void	Get_Capcity_Value(void); //��ȡ����ֵ

int	Short_Circuit_Detection(void);//��·���

void DataSubsection(float Cap_Division[],float arrey[],int Number);

uint8 ProbablityCapacitance(float CompareArrey[])	;


extern float Cap_Division[51];/**/

extern int HMI_Debug_Write_Button;
extern int HMI_Work_Button;

extern PaperCountEngine_Type Paper; //SB
extern int ShortFlag;        //��·��־λ



#endif



