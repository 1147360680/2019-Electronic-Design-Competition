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
	
	int Time; //ʱ��

	
}PaperCountEngine_Type;//ֽ�Ų�����״̬



void get_capcity_value(void);

/* ��У׼ʱ����ȡ������ ��ֵ */
float get_single_capacity(void);

void FDC2214_Data_Adjust(void);//����У׼

void	Capcity_Paper_Detection(void); //��ȡ����ֵ

int	Short_Circuit_Detection(void);//��·���

void DataSubsection(float Cap_Division[],float arrey[],int Number);

uint8 ProbablityCapacitance(float CompareArrey[])	;


extern float Cap_Division[101];/**/

extern int HMI_Debug_Write_Button;
extern int HMI_Work_Button;

extern PaperCountEngine_Type Paper; //SB
extern int ShortFlag;        //��·��־λ


extern float KT_Board_Value_In_Flash; //KT��Flash��ֵ
extern float Fiber_Board_Value_In_Flash;//��ά�� Flash��ֵ

extern float Money_100_In_Flash ; //100Ԫ
extern float Money_50_In_Flash ;  //50Ԫ

extern int Div_40_50_Parameter; //�з� 40~50�ŵ�ϵ��
extern int Div_50_60_Parameter; //
extern int Div_60_70_Parameter; //
extern int RMB_Value;

/* ��ӡ�� ֽ�ż�� */
void Printer_Paper_Detection(void);

/* ���� ��� */
void Material_Detection(void);

/* ���� ��� */
void Money_Detection(void);

#endif



