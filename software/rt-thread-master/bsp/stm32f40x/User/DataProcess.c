/*
 * DataProcess.c
 *
 *  Created on: 2019��8��7��
 *      Author: zengwangfa
 *      Notes:  FDC2214���ݴ���
 */


#include "DataProcess.h"
#include "FDC2214.h"
#include <rtthread.h>
#include "filter.h"
#include "led.h"
#include "HMI.h"

unsigned int CH0_DATA,CH1_DATA,CH2_DATA,CH3_DATA;//ͨ��ֵ

float ShortValue1,ShortValue2,ShortValue3,ShortValue4;//��·ֵ

float res1,res2,res3,res4;//�����ݴ�ֵ

float res1_arr[10],res2_arr[10],res3_arr[10],res4_arr[10];//�����ݴ�ֵ


PaperCountEngine_Type Paper = {
			.Finish_Flag = 2
};/*Paper���ݽṹ��*/

extern int HMI_Status_Flag;

//�ж�data�Ƿ��� value �� range��
int is_in_range(float data,float value,float range)
{
		if( (data < (value+range) && data > (value-range))){
				return 1;
		}		
		return 0;
}


/* ��ȡ����ֵ */
void Get_Capcity_Value(void)
{
		static uint8 ON_OFF = 0;
		/* �����������������󣬽������л��������̣߳�����Ӧ�ж� */
		rt_enter_critical();

		FDC2214_GetChannelData(FDC2214_Channel_0, &CH0_DATA);//��ȡ����
		FDC2214_GetChannelData(FDC2214_Channel_1, &CH1_DATA);
		FDC2214_GetChannelData(FDC2214_Channel_2, &CH2_DATA);
		FDC2214_GetChannelData(FDC2214_Channel_3, &CH3_DATA);
		
		res1 = Cap_Calculate(&CH0_DATA);//���ݸ�ֵ
		res2 = Cap_Calculate(&CH1_DATA);
		res3 = Cap_Calculate(&CH2_DATA);
		res4 = Cap_Calculate(&CH3_DATA);

	
		res1 = KalmanFilter1(&res1); //���ݽ��п������˲�
		res2 = KalmanFilter2(&res2);
		res3 = KalmanFilter3(&res3);
		res4 = KalmanFilter4(&res4);
	
		/* ���������� */	
		rt_exit_critical();

		Paper.Capacitance = res4;
		if(ON_OFF == 0 && res1 != 0.0f){/* ��ʼ�� ��ȡ��·ֵ*/
				ON_OFF = 1;//��������
			
				ShortValue1 = Cap_Calculate(&CH0_DATA);
				ShortValue2 = Cap_Calculate(&CH1_DATA);
				ShortValue3 = Cap_Calculate(&CH2_DATA);
				ShortValue4 = Cap_Calculate(&CH3_DATA);
		}
		
		
//		res1 = res1-temp1;//���ݼ�ȥ��ʼֵ ���津����
//		res2 = res2-temp2;//���津����
//		res3 = res3-temp3;//���������
//		res4 = res4-temp4;//���泤����
}




/* ��У׼ʱ����ȡ�ϼ��� ��ֵ*/
float get_top_capacity(void)
{
		static unsigned int res_CH0_DATA = 0;
		static float res_ch1_arr[10];
		static float res_ch1 = 0.0f;
		static int i = 0;
		
		/* �����������������󣬽������л��������̣߳�����Ӧ�ж� */
		rt_enter_critical();
	
		for(i = 0;i < 10;i++){
				FDC2214_GetChannelData(FDC2214_Channel_0, &res_CH0_DATA);//��ȡ����

				rt_thread_mdelay(10);
				res_ch1_arr[i] = Cap_Calculate(&res_CH0_DATA);//���ݸ�ֵ

		}
		/* ���������� */	
		rt_exit_critical();
		
		res_ch1 = Bubble_Filter_Float(res_ch1_arr);
		return res_ch1;

}
/* ��У׼ʱ����ȡ�¼��� ��ֵ */
float get_bottom_capacity(void)
{

		static unsigned int res_CH1_DATA = 0;
		static float res_ch2_arr[10];
		static float res_ch2 = 0.0f;
		static int i = 0;
		
		/* �����������������󣬽������л��������̣߳�����Ӧ�ж� */
		rt_enter_critical();
	
		for(i = 0;i < 10;i++){
				FDC2214_GetChannelData(FDC2214_Channel_1, &res_CH1_DATA);

				rt_thread_mdelay(10);
				res_ch2_arr[i] = Cap_Calculate(&res_CH1_DATA);
		}
		/* ���������� */	
		rt_exit_critical();
		
		res_ch2 = Bubble_Filter_Float(res_ch2_arr);		
	
		return res_ch2;
}



/* ��У׼ʱ����ȡ������ ��ֵ */
float get_single_capacity(void)
{

		static unsigned int res_CH4_DATA = 0;
		static float res_ch4_arr[10];
		static float res_ch4 = 0.0f;
		static int i = 0;
		
		/* �����������������󣬽������л��������̣߳�����Ӧ�ж� */
		rt_enter_critical();
	
		for(i = 0;i < 10;i++){
				FDC2214_GetChannelData(FDC2214_Channel_3, &res_CH4_DATA);

				rt_thread_mdelay(10);
				res_ch4_arr[i] = Cap_Calculate(&res_CH4_DATA);
		}
		/* ���������� */	
		rt_exit_critical();
		
		res_ch4 = Bubble_Filter_Float(res_ch4_arr);		
	
		return res_ch4;
}



/* ��·�ж� */
void Short_Circuit_Detection(void)
{
    //�������� ��·��ֵ��Χ�仯
//		if(  (is_in_range(res4,ShortValue4,40.0f)) || (is_in_range(res1,ShortValue1,40.0f) &&  is_in_range(res2,ShortValue2,40.0f)) \
//			|| (res1 < 1.0f) ||  (res2 < 1.0f)  ){//���ߵ�ֵ�ǳ�С��ʱ���ж�Ϊ�ܵ�����
		if(  (is_in_range(res4,ShortValue4,40.0f)) || (res4 < 1.0f) ){//���ߵ�ֵ�ǳ�С��ʱ���ж�Ϊ�ܵ�����
				Paper.ShortStatus = 1;//�ж���·
				Bling_Set(&Light_Blue,200,100,0.5,0,79,0);//������ʾ��·
		}			
		else{
				Bling_Set(&Light_Green,200,100,0.5,0,78,0);//�̵���ʾ����·
				Paper.ShortStatus = 2; //�ж�����·
		}
		uart_send_hmi_is_short();
		
}



