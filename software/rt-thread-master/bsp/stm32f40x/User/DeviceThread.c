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
#include <math.h>
#include <stdlib.h>
#include "propeller.h"
#include "servo.h"
#include "light.h"
#include "rc_data.h"
#include "Control.h"
#include "PropellerControl.h"
#include "focus.h"
#include "debug.h"
#include "timer.h"
#include "gyroscope.h"
#include "PID.h"
#include "DataProcess.h"
#include "HMI.h"
#include "drv_i2c.h"
#include "FDC2214.h"

uint8 thread_speed = 5;
extern int HMI_Write_Flag;
extern int HMI_Work_Button;
extern float CapacitanceSubsection[];
extern float FDC2214_Flash_Data_Single[];


void fdc2214_thread_entry(void *parameter)//�ߵ�ƽ1.5ms ������20ms  ռ�ձ�7.5% volatil
{

		rt_thread_mdelay(2000);
		uart_send_hmi_reboot();//��HMI��������λ
		rt_thread_mdelay(2000);
		DataSubsection(CapacitanceSubsection,FDC2214_Flash_Data_Single,50);
		while(1)
		{
				Short_Circuit_Detection();	
				if(1 == HMI_Status_Flag){//��ʼУ׼

						FDC2214_Data_Adjust(); //����У׼	
				}
				else{ //����ģʽ

						Get_Capcity_Value(); //��ȡ����ֵ

						if(HMI_Work_Button == 1){//�������
								Paper.Finish_Flag = 1;//��ʾ��ȡ���
								HMI_Work_Button = 0;
						}
				}

				
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
			
				IIC_Init(); /* ��ʼ�� */
				rt_thread_mdelay(100);
				FDC2214_Init();
			
				rt_thread_startup(fdc2214_tid);
		}

		return 0;
}
INIT_APP_EXPORT(fdc2214_thread_init);






























void Set_Z_Zero(void)//����Z���0
{
		/* �����������������󣬽������л��������̣߳�����Ӧ�ж� */
		rt_enter_critical();
	
		TIM4_PWM_CH1_D12(YAW_YUNTAI_MED);  //����   D12
		TIM4_PWM_CH2_D13(PITCH_YUNTAI_MED); //����   D13
		rt_thread_mdelay(2000);
	
		/* ���������� */
		rt_exit_critical();
	
		gyroscope_z_zero();

}
MSH_CMD_EXPORT(Set_Z_Zero,ag: Set_Z_Zero  );

//void drifting_check(void)//Ư�Ƽ��
//{
//		if( Total_Controller.Yaw_Angle_Control.Control_OutPut <= 1){
//				
//				gyroscope_z_zero();
//				log_w("drifting goto zero");
//		
//		}

//}


///**
//  * @brief  propeller_thread_entry(�ƽ�������������)
//  * @param  void* parameter
//  * @retval None
//  * @notice 
//  */
//void propeller_thread_entry(void *parameter)
//{
//		rt_thread_mdelay(1000);
//		//Set_Z_Zero();
//		//TIM3_ENABLE();
//		rt_thread_mdelay(1000);


//		while(1)
//		{

//			
//			
//			
//			
//			
//			
//				//drifting_check(); //Ư�� ���		
//				//Car_Pitch_Control();			
//				//Two_Axis_Yuntai_Control();
//			
//				//Servo_Dir_Control(0);			//�������

//				
//				rt_thread_mdelay(thread_speed); //5ms
//		}
//}




//static int speed(int argc, char **argv)
//{
//    int result = 0;
//    if (argc != 2){
//        log_e("Error! Proper Usage: speed <0~100>");
//				result = -RT_ERROR;
//        goto _exit;
//    }
//		if(atoi(argv[1]) <= 1000){
//				thread_speed = atoi(argv[1]);
//		}
//		else {
//				log_e("Error! The value is out of range!");
//		}
//_exit:
//    return result;
//}
//MSH_CMD_EXPORT(speed,ag: speed  <0~100>);








///**
//  * @brief  servo_thread_entry(�������������)
//  * @param  void* parameter
//  * @retval None
//  * @notice 
//  */
//void devices_thread_entry(void *parameter)//�ߵ�ƽ1.5ms ������20ms  ռ�ձ�7.5% volatil
//{



//		while(1)
//		{
//			
//				if(WORK == WorkMode){//����ģʽ

//						//Extractor_Control(&ControlCmd.Arm); //��ȡ������
//						//RoboticArm_Control(&ControlCmd.Arm);//��е�ۿ���
//						Search_Light_Control(&ControlCmd.Light);  //̽�յƿ���
//						YunTai_Control(&ControlCmd.Yuntai); //��̨����				
//						Focus_Zoom_Camera_Control(&ControlCmd.Focus);//�佹����ͷ����					
//				}
//				else if(DEBUG == WorkMode)//����ģʽ
//				{	
//						//Debug_Mode(get_button_value(&ControlCmd));
//				}
//				rt_thread_mdelay(20);
//		}
//	
//}


//int propeller_thread_init(void)
//{
//    rt_thread_t propeller_tid;
//		/*������̬�߳�*/
//    propeller_tid = rt_thread_create("propoller",//�߳�����
//                    propeller_thread_entry,			 //�߳���ں�����entry��
//                    RT_NULL,							   //�߳���ں���������parameter��
//                    2048,										 //�߳�ջ��С����λ���ֽڡ�byte��
//                    5,										 	 //�߳����ȼ���priority��
//                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 1ms

//    if (propeller_tid != RT_NULL){
//			
//				PWM_Init(); //�ƽ����������PWM��ʼ��
//			
//				rt_thread_startup(propeller_tid);
//		}

//		return 0;
//}
////INIT_APP_EXPORT(propeller_thread_init);












//int devices_thread_init(void)
//{
//    rt_thread_t devices_tid;
//		/*������̬�߳�*/
//    devices_tid = rt_thread_create("devices",//�߳�����
//                    devices_thread_entry,			 //�߳���ں�����entry��
//                    RT_NULL,							   //�߳���ں���������parameter��
//                    1024,										 //�߳�ջ��С����λ���ֽڡ�byte��
//                    12,										 	 //�߳����ȼ���priority��
//                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

//    if (devices_tid != RT_NULL){
//				Light_PWM_Init(); //̽�յ�PWM��ʼ��
//				log_i("Light_init()");

//				rt_thread_startup(devices_tid);
//				//rt_event_send(&init_event, PWM_EVENT); //�����¼�  ��ʾ��ʼ�����
//		}

//		return 0;
//}
//INIT_APP_EXPORT(devices_thread_init);






