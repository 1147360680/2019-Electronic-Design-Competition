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
#include <string.h>
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


#define PITCH_YUNTAI_MED  670
#define YAW_YUNTAI_MEN    1810

Cycle_Type Yuntai_Cycle = {-PI/4,-PI/4,0,0};

int target_x = 160;//Ŀ������
int target_y = 120;


int persent_x = 160;//��ǰ����
int persent_y = 120;

float target_pit = 0.0f; //�����Ƕ�
float target_yaw = 0.0f;

float persent_pit = 0.0f;//��ǰ�Ƕ�
float persent_yaw = 0.0f;

uint16 Pitch_Axis_Output_Limit_Left(int16 value)
{
		//������+500   ������-500
		value = (value) > PITCH_YUNTAI_MED + 600 ? PITCH_YUNTAI_MED + 600 : value ;//�����޷�
		value = (value) < PITCH_YUNTAI_MED - 600 ? PITCH_YUNTAI_MED - 600 : value;//�����޷�
	
		return value ;
}

uint16 Yaw_Axis_Output_Limit_Right(int16 value)
{
		//������+500   ������-500
		value = (value) > YAW_YUNTAI_MEN + 1000 ? YAW_YUNTAI_MEN + 1000 : value ;//�����޷�
		value = (value) < YAW_YUNTAI_MEN - 1000 ? YAW_YUNTAI_MEN - 1000 : value;//�����޷�
	
		return value ;
}



float draw_cycle(float pit,float yaw,float l,float r)
{
		
		return atan(sqrt( ((r*r)/(l*l)) - (tan(pit)*tan(pit))));
}

void Yuntai_Draw_Cycle(Cycle_Type *cyc)
{
		cyc->l = 10;
		cyc->r = 10;
		
		cyc->pit += 0.01f;
		if(cyc->pit >= PI/4){
				cyc->pit = -PI/4;
		}
		
		//cyc->yaw = draw_cycle(cyc->pit,cyc->pit,cyc->l,cyc->r);
		cyc->yaw += 0.01f;
		
		if(cyc->yaw >= PI/4){
				cyc->yaw = -PI/4;
		}
}
/* ��̨��Բ */
//void Yuntai_Draw_Cycle(Cycle_Type *cyc)
//{
//		cyc->x = cyc->a * cos(cyc->Angle);
//		cyc->y = cyc->b * sin(cyc->Angle);
//		
//		PropellerPower.leftMiddle = YAW_YUNTAI_MEN + Yuntai_Cycle.x;
//		PropellerPower.rightMiddle = PITCH_YUNTAI_MED + Yuntai_Cycle.y;
//	
//}





/* Ѱ��ɫ�� */
void Yuntai_Find_Blobs(void)
{
	

		persent_x = get_persent_x();//��ȡ С��X��
	  persent_y = get_persent_y();//��ȡ С��Y��
	
		yuntai_pid_control(persent_x,target_x,persent_y,target_y);

 		PropellerPower.leftMiddle  = YAW_YUNTAI_MEN   - Total_Controller.Yaw_Angle_Control.Control_OutPut;   //ˮƽ
	  PropellerPower.rightMiddle = PITCH_YUNTAI_MED + Total_Controller.Pitch_Angle_Control.Control_OutPut; //����    �����  ������Ƹ����Ķ�� Y��
	
}

float first_yaw = 0.0f;

float rad_first_yaw = 0.0f;
void Close_Loop_Angle_Control(void)
{

		static uint8 ON_OFF = 0;//��������
		if(0 == ON_OFF ){
				ON_OFF = 1;
				first_yaw = Sensor.JY901.Euler.Yaw;
		}
	
		Yuntai_Draw_Cycle(&Yuntai_Cycle);
	
		target_pit = -Rad2Deg(Yuntai_Cycle.pit);
		
		//rad_first_yaw = Deg2Rad(first_yaw);
		target_yaw = Rad2Deg(Yuntai_Cycle.yaw) + first_yaw;
	
	  persent_yaw = Sensor.JY901.Euler.Yaw;
		persent_pit = Sensor.JY901.Euler.Pitch;

		yuntai_pid_control(persent_yaw,target_yaw,persent_pit,target_pit);		
	
		PropellerPower.leftMiddle  = YAW_YUNTAI_MEN   + Total_Controller.Yaw_Angle_Control.Control_OutPut;   //ˮƽ
	  PropellerPower.rightMiddle = PITCH_YUNTAI_MED - Total_Controller.Pitch_Angle_Control.Control_OutPut; //����    �����  ������Ƹ����Ķ�� Y��	

}


uint8 servo_test_flag = 0;//�Ƿ��� �����ֵ���� flag

void Two_Axis_Yuntai_Control(void)
{
		if(0 == servo_test_flag){
				Close_Loop_Angle_Control();
				//Yuntai_Find_Blobs();
		}



		PropellerPower.leftMiddle  = Yaw_Axis_Output_Limit_Right(PropellerPower.leftMiddle);  //��̨
		PropellerPower.rightMiddle = Pitch_Axis_Output_Limit_Left(PropellerPower.rightMiddle);			
	
		TIM4_PWM_CH1_D12(PropellerPower.leftMiddle);  //����   D12
		TIM4_PWM_CH2_D13(PropellerPower.rightMiddle); //����   D13
}


static int expect_yaw(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage:");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 360){
				target_yaw = atoi(argv[1]);
		}

		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(expect_yaw,ag: yaw );


static int expect_pitch(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage:");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 360){
				target_pit = atoi(argv[1]);
		}

		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(expect_pitch,ag: yaw );

static int servo_med_value_test_on_off(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: servo_med_value_on_off <on/off>");
				result = -RT_ERROR;
        goto _exit;
    }
		if(!strcmp(argv[1],"off")){
				servo_test_flag = 1;
		}
		else if(!strcmp(argv[1],"on")){
				servo_test_flag = 0;
		}
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(servo_med_value_test_on_off,ag: servo_med_value_on_off <on/off>);


/*����� ������yaw MSH���� */
static int servo_med_value_test(int argc, char **argv)
{
    int result = 0;
    if (argc >= 2){
        log_e("Error! Proper Usage: coords pit yaw");
				result = -RT_ERROR;
        goto _exit;
    }

		if(atoi(argv[1])<5000 && atoi(argv[2])<5000  ){
				PropellerPower.leftMiddle  = atoi(argv[1]);
				PropellerPower.rightMiddle = atoi(argv[2]);	
		}
		else {
				log_e("Error! The  value is out of range!");
		}

		
_exit:
    return result;
}
MSH_CMD_EXPORT(servo_med_value_test,ag: coords 50 50);


























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

