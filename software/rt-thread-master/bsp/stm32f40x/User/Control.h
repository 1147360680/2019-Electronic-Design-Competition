#ifndef __CONTROL_H_
#define __CONTROL_H_

#include "DataType.h"
#include "rc_data.h"


//typedef struct 
//{
//	int x;
//	int y;
//	int a;
//	int b;
//	float Angle;
//		
//}Cycle_Type;// Բ

typedef struct 
{
	float pit;
	float yaw;
	float l; //����
	float r; //�뾶

		
}Cycle_Type;// Բ





void Angle_Control(void);
void Depth_PID_Control(float expect_depth,float sensor_depth);


void FourtAxis_Control(Rocker_Type *rc);

void SixAxis_Control(Rocker_Type *rc);


void Convert_RockerValue(Rocker_Type *rc); //��ȡҡ��ֵ

void Speed_Buffer(short *now_value,short *last_value,short BufferRange);


void Two_Axis_Yuntai_Control(void);
#endif


