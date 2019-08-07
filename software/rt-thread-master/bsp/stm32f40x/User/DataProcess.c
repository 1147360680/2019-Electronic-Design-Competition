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
unsigned int CH0_DATA,CH1_DATA,CH2_DATA,CH3_DATA;//ͨ��ֵ

float ShortValue1,ShortValue2,ShortValue3,ShortValue4;//��·ֵ

float res1,res2,res3,res4;//�����ݴ�ֵ

float res1_arr[10],res2_arr[10],res3_arr[10],res4_arr[10];//�����ݴ�ֵ
int PaperNumber = 0;      //ֽ������
int ShortFlag = 0;        //��·��־λ

//�ж�data�Ƿ��� value �� range��
int is_in_range(float data,float value,float range)
{
		if( (data < (value+range) && data > (value-range))){
				return 1;
		}		
		return 0;
}



void get_capcity_value(void)
{
		static uint8 ON_OFF = 0;
		/* �����������������󣬽������л��������̣߳�����Ӧ�ж� */
		rt_enter_critical();

		FDC2214_GetChannelData(FDC2214_Channel_0, &CH0_DATA);//��ȡ����
		FDC2214_GetChannelData(FDC2214_Channel_1, &CH1_DATA);
		//FDC2214_GetChannelData(FDC2214_Channel_2, &CH2_DATA);
		//FDC2214_GetChannelData(FDC2214_Channel_3, &CH3_DATA);
		
		res1 = Cap_Calculate(&CH0_DATA);//���ݸ�ֵ
		res2 = Cap_Calculate(&CH1_DATA);
		res3 = Cap_Calculate(&CH2_DATA);
		res4 = Cap_Calculate(&CH3_DATA);

	
		res1 = KalmanFilter1(&res1); //���ݽ��п������˲�
		res2 = KalmanFilter2(&res2);
		//res3 = KalmanFilter3(&res3);
		//res4 = KalmanFilter4(&res4);
	
		/* ���������� */	
		rt_exit_critical();

		if(ON_OFF == 0 && res1 != 0.0f){/* ��ʼ�� ��ȡ��·ֵ*/
				ON_OFF = 1;//��������
			
				ShortValue1 = Cap_Calculate(&CH0_DATA);
				ShortValue2 = Cap_Calculate(&CH1_DATA);
				//ShortValue3 = Cap_Calculate(&CH2_DATA);
				//ShortValue4 = Cap_Calculate(&CH3_DATA);
		}
		
		
//		res1 = res1-temp1;//���ݼ�ȥ��ʼֵ ���津����
//		res2 = res2-temp2;//���津����
//		res3 = res3-temp3;//���������
//		res4 = res4-temp4;//���泤����
}

/* ��ȡ�ϼ��� ��ֵ*/
float get_top_capacity(void)
{
		return res1;

}
/* ��ȡ�¼��� ��ֵ */
float get_bottom_capacity(void)
{
		return res2;

}
/* ��·�ж� */
void Short_Circuit_Detection(void)
{
    //�������� ��·��ֵ��Χ�仯
		if( (is_in_range(res1,ShortValue1,40.0f) &&  is_in_range(res2,ShortValue2,40.0f)) \
			|| (res1 < 1.0f) ||  (res2 < 1.0f) ){//����
				ShortFlag = 1;//�ж���·
				Bling_Set(&Light_Blue,200,100,0.5,0,79,0);//������ʾ��·
		}			
		else{
				Bling_Set(&Light_Green,200,100,0.5,0,78,0);//�̵���ʾ����·
				ShortFlag = 2;
		}
}


void fdc2214_thread_entry(void *parameter)//�ߵ�ƽ1.5ms ������20ms  ռ�ձ�7.5% volatil
{
		rt_thread_mdelay(3000);
		while(1)
		{
			
				get_capcity_value();
				Short_Circuit_Detection();
				rt_thread_mdelay(2);
		}
	
}


int fdc2214_thread_init(void)
{
    rt_thread_t fdc2214_tid;
		/*������̬�߳�*/
    fdc2214_tid = rt_thread_create("fdc2214",//�߳�����
                    fdc2214_thread_entry,			 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    2048,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    5,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 1ms

    if (fdc2214_tid != RT_NULL){
				IIC_Init();
				rt_thread_mdelay(100);
				FDC2214_Init();
				rt_thread_startup(fdc2214_tid);
		}

		return 0;
}
INIT_APP_EXPORT(fdc2214_thread_init);






