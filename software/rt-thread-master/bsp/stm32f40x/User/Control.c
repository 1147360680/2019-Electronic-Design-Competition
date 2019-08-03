/*
 * Control.c
 *
 *  Created on: 2019��3��20��
 *      Author: zengwangfa
 *      Notes:  �˶��ܿ���
 */
 
#define LOG_TAG "Control"

#include <rtthread.h>
#include <elog.h>
#include <stdlib.h>
#include <math.h>

#include "Control.h"
#include "PID.h"
#include "rc_data.h"

#include "focus.h"
#include "led.h"
#include "servo.h"
#include "PropellerControl.h"
#include "propeller.h"
#include "sensor.h"


#define PITCH_YUNTAI_MED  750
#define YAW_YUNTAI_MEN  1800
int target_x = 160;//Ŀ������
int target_y = 120;


int coords_x = -128;//��ǰ����
int coords_y = 0;

uint16 Pitch_Axis_Output_Limit_Left(int16 value)
{
		//������+500   ������-500
		value = (value) > PITCH_YUNTAI_MED+300 ? PITCH_YUNTAI_MED+300 : value ;//�����޷�
		value = (value) < PITCH_YUNTAI_MED-300 ? PITCH_YUNTAI_MED-300 : value;//�����޷�
	
		return value ;
}

uint16 Yaw_Axis_Output_Limit_Right(int16 value)
{
		//������+500   ������-500
		value = (value) > YAW_YUNTAI_MEN+400 ? YAW_YUNTAI_MEN+400 : value ;//�����޷�
		value = (value) < YAW_YUNTAI_MEN-400 ? YAW_YUNTAI_MEN-400 : value;//�����޷�
	
		return value ;
}

uint8 cycle_flag = 0;
/* ������x,y,�뾶r*/
void draw_cycle(int *x,int *y,int r)
{	

			if(0 == cycle_flag){//�ϰ�Բ
					(*x)++;
					if((*x)>=r){
							(*x) = r;
							cycle_flag = 1;
					}
					*y = -sqrt((r*r)- ((*x)*(*x))); 
			}
			else if(1 == cycle_flag){
					(*x)--;
					if((*x)<=(-r)){
							(*x) = (-r);
							cycle_flag = 0;
					}
					*y = sqrt((r*r)- ((*x)*(*x))); 
			}
			
			//rt_thread_mdelay(1);

}


int test_x = -250;
int test_y = 0;
void Two_Axis_Yuntai_Control(void)
{

	
	
//		coords_x = get_persent_x();//��ȡ С��X��
//	  coords_y = get_persent_y();//��ȡ С��Y��

		
    //yuntai_pid_control(coords_x,target_x,coords_y,target_y);
		draw_cycle(&test_x,&test_y,250);

// 		PropellerPower.leftMiddle  = YAW_YUNTAI_MEN   + Total_Controller.Yaw_Angle_Control.Control_OutPut;   //ˮƽ
//		PropellerPower.rightMiddle = PITCH_YUNTAI_MED - Total_Controller.Pitch_Angle_Control.Control_OutPut; //����    �����  ������Ƹ����Ķ�� Y��
		PropellerPower.leftMiddle = YAW_YUNTAI_MEN + test_x;
		PropellerPower.rightMiddle = PITCH_YUNTAI_MED + test_y;

		PropellerPower.leftMiddle  = Yaw_Axis_Output_Limit_Right(PropellerPower.leftMiddle);  //��̨
		PropellerPower.rightMiddle = Pitch_Axis_Output_Limit_Left(PropellerPower.rightMiddle);			
	
		TIM4_PWM_CH1_D12(PropellerPower.leftMiddle);  //����   D12
		TIM4_PWM_CH2_D13(PropellerPower.rightMiddle); //����   D13
}




/*����� ������yaw MSH���� */
static int coords(int argc, char **argv)
{
    int result = 0;
    if (argc != 3){
        rt_kprintf("Error! Proper Usage: coords 50 50");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1])<10000 && atoi(argv[2])<10000  ){
				
				coords_x = atoi(argv[1]);
				coords_y = atoi(argv[2]);
			
		}
		else {
				log_e("Error! The  value is out of range!");
		}

		
_exit:
    return result;
}
MSH_CMD_EXPORT(coords,ag: coords 50 50);


























void Depth_PID_Control(float expect_depth,float sensor_depth)
{
		
		Total_Controller.High_Position_Control.Expect = expect_depth ; //���������ң��������
		Total_Controller.High_Position_Control.FeedBack = sensor_depth;  //��ǰ��ȷ���
		PID_Control(&Total_Controller.High_Position_Control);//�߶�λ�ÿ�����
	
		robot_upDown(&Total_Controller.High_Position_Control.Control_OutPut);		//��ֱ�ƽ�������
}



void Gyro_Control(void)//���ٶȻ�
{

//  	ƫ����ǰ������
//  	Total_Controller.Yaw_Gyro_Control.FeedBack=Yaw_Gyro;


//		PID_Control_Div_LPF(&Total_Controller.Yaw_Gyro_Control);
//		Yaw_Gyro_Control_Expect_Delta=1000*(Total_Controller.Yaw_Gyro_Control.Expect-Last_Yaw_Gyro_Control_Expect)
//			/Total_Controller.Yaw_Gyro_Control.PID_Controller_Dt.Time_Delta;
//		//**************************ƫ����ǰ������**********************************
//		Total_Controller.Yaw_Gyro_Control.Control_OutPut+=Yaw_Feedforward_Kp*Total_Controller.Yaw_Gyro_Control.Expect
//			+Yaw_Feedforward_Kd*Yaw_Gyro_Control_Expect_Delta;//ƫ����ǰ������
//		Total_Controller.Yaw_Gyro_Control.Control_OutPut=constrain_float(Total_Controller.Yaw_Gyro_Control.Control_OutPut,
//																																		 -Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit,
//																																		 Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit);
//		Last_Yaw_Gyro_Control_Expect=Total_Controller.Yaw_Gyro_Control.Expect;
//		

}

/*����� ������yaw MSH���� */
static int depth(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        rt_kprintf("Error! Proper Usage: RoboticArm_openvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1])<100){
				Expect_Depth = atoi(argv[1]);
		}
		else {
				log_e("Error! The  value is out of range!");
		}

		
_exit:
    return result;
}
MSH_CMD_EXPORT(depth,ag: depth 10);





/*�������� �޸�MSH���� */
static int unlock(int argc, char **argv) //ֻ���� 0~3.0f
{
		ControlCmd.All_Lock = UNLOCK;
		return 0;
}
MSH_CMD_EXPORT(unlock,unlock);


/*�������� �޸�MSH���� */
static int lock(int argc, char **argv) //ֻ���� 0~3.0f
{
		ControlCmd.All_Lock = LOCK;
		return 0;
}
MSH_CMD_EXPORT(lock,lock);

