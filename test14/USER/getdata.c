#include "getdata.h"

/*	 整合过程更新说明
*   2014/6/25
*   
*   2014/6/23
*   更新了一些注释的正确性
*   
*   2014/6/22
*   加入了IIC.c 然后要用到delay_us delay_ms ，这里修改了SysTick.c 
*   每1us中断一次，delay_us 和delay_ms
*   取消了Syseminit 因为使用了3.5版本库不需要调用。
*   对Acc Gyro LSB值改为宏   
*/

/* Gyro and accel data are written to the FIFO by the DMP in chip
* frame and hardware units. This behavior is convenient because it
* keeps the gyro and accel outputs of dmp_read_fifo and
* mpu_read_fifo consistent.
* if (sensors & INV_XYZ_GYRO )
* send_packet(PACKET_TYPE_GYRO, gyro);
* if (sensors & INV_XYZ_ACCEL)
* send_packet(PACKET_TYPE_ACCEL, accel); 
* Unlike gyro and accel, quaternions are written to the FIFO in
* the body frame, q30. The orientation is set by the scalar passed
* to dmp_set_orientation during initialization.
*/

// define variable
int stop = 0;
char num[50];

float Pitch,Roll,Yaw;

unsigned long sensor_timestamp;
short gyro[3], accel[3], sensors;
unsigned char more;

long quat[4];
float m_acc[3];
float m_gyro[3];

volatile float SVM_Th1 =  1.5, SVM_Th2 = 3.9, SVM_Th3 = 4.5, Ang_Th1 = 20, Ang_Th2 = 20;

int8_t SVM_Que = 0;
int8_t Ang_Que = 0;

double phi,psi,theta;

float SVM;
float Ang;
float gravity;

float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;

VectorFloat v;
// end of define variable
// this is for increase the SVM_Th and angle_Th

void setStop(void)
{
	stop = 1;
}
void resetStop(void)
{
	stop = 0;
}
void incrsSVM_Th1(void)
{
	SVM_Th1 += 0.1;
}
void incrsSVM_Th2(void)
{
	SVM_Th2 += 0.1;
}
void incrsSVM_Th3(void)
{
	SVM_Th3 += 0.1;
}
void restoreSVM_Th(void)
{
	SVM_Th1 = 1.5;
	SVM_Th2 = 3.9;
	SVM_Th3 = 4.5;
}

void incrsAng_Th1(void)
{
	Ang_Th1 += 0.1;
}
void incrsAng_Th2(void)
{
	Ang_Th2 += 0.1;
}
void restoreAng_Th(void)
{
	Ang_Th1 = 20;
	Ang_Th2 = 30;
}

float getSVM()
{
	dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,&more);	
	if (sensors & INV_WXYZ_QUAT )
	{
		q0=quat[0] / q30;	
		q1=quat[1] / q30;
		q2=quat[2] / q30;
		q3=quat[3] / q30;
		dmpGetGravity(&v);
		gravity = sqrt(pow(v.x,2) + pow(v.y,2) + pow(v.z,2));
	}
	if (sensors & INV_XYZ_ACCEL)
	{
		m_acc[0] = (float)accel[0]/Acc2gLSB + v.x;
		m_acc[1] = (float)accel[1]/Acc2gLSB + v.y;
		m_acc[2] = (float)accel[2]/Acc2gLSB - v.z;
	}
	SVM = (sqrt(pow(m_acc[0],2) + pow(m_acc[1],2) + pow(m_acc[2],2)))*AccGravity;
	return SVM;
}

float getAng()
{
	dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,&more);	
	if (sensors & INV_WXYZ_QUAT )
	{
		q0=quat[0] / q30;	
		q1=quat[1] / q30;
		q2=quat[2] / q30;
		q3=quat[3] / q30;
		theta = acos(q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3)*180/PI;
	}
	return theta;
}

// return 0 if successful return 1 if failed
int mpuDMPinit()
{
	int result;
	result = mpu_init();

	if(!result)
	{	
		printf("mpu initialization complete......\n ");	 	  //mpu_set_sensor
		if(!mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL))
		printf("mpu_set_sensor complete ......\n"); 		
		else
		printf("mpu_set_sensor come across error ......\n");
		if(!mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL))	   	  //mpu_configure_fifo
		printf("mpu_configure_fifo complete ......\n");
		else
		printf("mpu_configure_fifo come across error ......\n");
		if(!mpu_set_sample_rate(DEFAULT_MPU_HZ))	   	  //mpu_set_sample_rate	   100Hz
		printf("mpu_set_sample_rate complete ......\n");
		else
		printf("mpu_set_sample_rate error ......\n");
		if(!dmp_load_motion_driver_firmware())   	  //dmp_load_motion_driver_firmvare
		printf("dmp_load_motion_driver_firmware complete ......\n");
		else
		printf("dmp_load_motion_driver_firmware come across error ......\n");
		if(!dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation))) 	  //dmp_set_orientation
		printf("dmp_set_orientation complete ......\n");
		else
		printf("dmp_set_orientation come across error ......\n");
		if(!dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
					DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
					DMP_FEATURE_GYRO_CAL))		   	  //dmp_enable_feature
		printf("dmp_enable_feature complete ......\n");
		else
		printf("dmp_enable_feature come across error ......\n");
		if(!dmp_set_fifo_rate(DEFAULT_MPU_HZ))   	  //dmp_set_fifo_rate
		printf("dmp_set_fifo_rate complete ......\n");
		else
		printf("dmp_set_fifo_rate come across error ......\n");
		run_self_test();
		if(!mpu_set_dmp_state(1))
		printf("mpu_set_dmp_state complete ......\n");
		else
		printf("mpu_set_dmp_state come across error ......\n");
		return 0;
	}
	else return 1;
}

void checkAlgo()
{
	int i,j,k;
	while(1)
	{
			
	lab1:
//		printf("after go to the lab1\n");
//		printf("SVM is %0.3f\n",SVM);
		while(stop)
		{
			SVM=getSVM();
//			printf("in while SVM is %0.3f\n",SVM);
		}
		SVM = getSVM();//取一次新的数据
//		printf("SVM is %0.3f\n",SVM);
		if( SVM >= SVM_Th1)
		{
			SVM_Que = (SVM_Que << 1) + 1;
			if(SVM >= SVM_Th2)
			{
				if(SVM >= SVM_Th3)
				{
					for( k = 0 , j = 0; k < 5; k++)
					{
						Ang = getAng();//取一次新的数据
//						printf("Ang is %0.3f\n",Ang);
						if(Ang > Ang_Th2)
						{
							j= j + 1;
							if( j >= 2)
							{
								//转中断处理USART2中断发送1
								LED_toggle(3);
								setStop();								
								printf("1");
//								printf("first :before the lab1\n");
								goto lab1;
							}
						}
					}
				}
				else
				{
					float SVMlast = SVM;
					float Anglast = Ang;
					float deltaAng;
					for(i = 0; i < 20; i++)
					{
						SVM = getSVM();//取一次新的数据
						Ang = getAng();//取一次新的数据
//						printf("SVM is %0.3f\n",SVM);
//						printf("Ang is %0.3f\n",Ang);
						deltaAng = Ang - Anglast;
						if(SVM > SVMlast || Ang > Ang_Th1 || deltaAng > 10 )
						{
							if(SVM > SVM_Th3)
							{
								for( k = 0 , j = 0; k < 5; k++)
								{
									Ang = getAng();//取一次新的数据
//									printf("Ang is %0.3f\n",Ang);
									if(Ang > Ang_Th2)
									{
										j= j + 1;
										if( j >= 2)
										{
											//转中断处理USART2中断发送1
											LED_toggle(3);
											setStop();
											printf("1");
//											printf("second before the lab1\n");
//											LED3(OFF);
											goto lab1;
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				float SVMlast = SVM;
				float Anglast = Ang;
				float deltaAng;
				for(i = 0; i < 50; i++)
				{
					SVM = getSVM();//取一次新的数据
					Ang = getAng();//取一次新的数据
//					printf("SVM is %0.3f\n",SVM);
//					printf("Ang is %0.3f\n",Ang);
					deltaAng = Ang - Anglast;
					if(SVM > SVMlast || SVM > SVM_Th2 || Ang > Ang_Th1 || deltaAng > 10)
					{
						if(SVM >= SVM_Th3)
						{
							for( k = 0 , j = 0; k < 5; k++)
							{
								Ang = getAng();//取一次新的数据
//								printf("Ang is %0.3f\n",Ang);
								if(Ang > Ang_Th2)
								{
									j= j + 1;
									if( j >= 2)
									{
										//转中断处理USART2中断发送1
										LED_toggle(3);
										setStop();
										printf("1");
//										printf("third : before the lab1\n");
//										LED3(OFF);
										goto lab1;
									}
								}
							}	
						}
						else
						{
							if((SVM_Que & 1) && (SVM_Que&(1<<7)) && (Ang_Que & 1) && (Ang_Que & (1<<7)))
							{
								for( k = 0 , j = 0; k < 5; k++)
								{
									Ang = getAng();//取一次新的数据
//									printf("Ang is %0.3f\n",Ang);
									if(Ang > Ang_Th2)
									{
										j= j + 1;
										if( j >= 2)
										{
											//转中断处理USART2中断发送1
											LED_toggle(3);
											setStop();
											printf("1");
//											printf("forth : before the lab1\n");
//											LED3(OFF);
											goto lab1;
										}
									}
								}
									
							}
							else if(i >= -10 && i <= 49 )
							{
								i = i - 10;
							}
						}
					}
				}
			}
		}
		else
		{
			SVM_Que = (SVM_Que << 1) + 0;
			if(Ang >= Ang_Th1)
			{
				Ang_Que = (Ang_Que << 1) + 1;
			}
			else{
				Ang_Que = (Ang_Que << 1) + 0;
			}
		}
	}	
}	
uint8_t dmpGetGravity(VectorFloat *v) {
    v -> x = 2 * (q1*q3 - q0*q2);
    v -> y = 2 * (q0*q1 + q2*q3);
    v -> z = q0*q0 - q1*q1 - q2*q2 + q3*q3;
    return 0;
}
