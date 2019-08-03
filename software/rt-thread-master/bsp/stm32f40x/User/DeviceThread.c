/*
 * DeviceThread.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  �豸��������
 */

#include "DeviceThread.h"
#include <rtthread.h>
#include <elog.h>
#include "propeller.h"
#include "servo.h"
#include "light.h"
#include "rc_data.h"
#include "Control.h"
#include "PropellerControl.h"
#include "focus.h"
#include "debug.h"


/**
  * @brief  propeller_thread_entry(�ƽ�������������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void propeller_thread_entry(void *parameter)
{
		rt_thread_mdelay(3000);
		uint32 cnt = 0;
		while(1)
		{
				
				if(FOUR_AXIS == VehicleMode  ){ //��ȫ������ʩ
						
					
						cnt++;
						
						Servo_Dir_Control(0);
							
						if(cnt >= 2000){
								cnt= 0;
								//Two_Axis_Yuntai_Control();
						}
						Back_Wheel_Control(0);
				}
				else if(SIX_AXIS == VehicleMode ){

				}


				rt_thread_mdelay(1); //5ms
		}
	
}












/**
  * @brief  servo_thread_entry(�������������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void devices_thread_entry(void *parameter)//�ߵ�ƽ1.5ms ������20ms  ռ�ձ�7.5% volatil
{



		while(1)
		{
			
				if(WORK == WorkMode){//����ģʽ

						//Extractor_Control(&ControlCmd.Arm); //��ȡ������
						RoboticArm_Control(&ControlCmd.Arm);//��е�ۿ���
						Search_Light_Control(&ControlCmd.Light);  //̽�յƿ���
						YunTai_Control(&ControlCmd.Yuntai); //��̨����				
						Focus_Zoom_Camera_Control(&ControlCmd.Focus);//�佹����ͷ����					
				}
				else if(DEBUG == WorkMode)//����ģʽ
				{	
						//Debug_Mode(get_button_value(&ControlCmd));
				}
				rt_thread_mdelay(20);
		}
	
}


int propeller_thread_init(void)
{
    rt_thread_t propeller_tid;
		/*������̬�߳�*/
    propeller_tid = rt_thread_create("propoller",//�߳�����
                    propeller_thread_entry,			 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    2048,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    12,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (propeller_tid != RT_NULL){
			
				PWM_Init(); //�ƽ����������PWM��ʼ��
			
				rt_thread_startup(propeller_tid);
		}

		return 0;
}
INIT_APP_EXPORT(propeller_thread_init);

int devices_thread_init(void)
{
    rt_thread_t devices_tid;
		/*������̬�߳�*/
    devices_tid = rt_thread_create("devices",//�߳�����
                    devices_thread_entry,			 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    1024,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    12,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (devices_tid != RT_NULL){
				Light_PWM_Init(); //̽�յ�PWM��ʼ��
				log_i("Light_init()");

				rt_thread_startup(devices_tid);
				//rt_event_send(&init_event, PWM_EVENT); //�����¼�  ��ʾ��ʼ�����
		}

		return 0;
}
INIT_APP_EXPORT(devices_thread_init);






