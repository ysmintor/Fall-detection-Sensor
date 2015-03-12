#ifndef __GETDATA_H
#define	__GETDATA_H
#include "math.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "usart2.h"
#include "stm32f10x.h"
#include "led.h"
//把库函数处理成结合起来处理成自己需要的数据
#ifndef PI
#define PI 3.1415926
#endif

#define AccGravity 9.8
//AFS_SEL Full Scale Range LSB Sensitivity
//0 ±2g 16384 LSB/mg
//1	±4g 8192 LSB/mg
//2	±8g 4096 LSB/mg
//3 ±16g 2048 LSB/mg
#define Acc2gLSB 16348
#define Acc4gLSB 8192
#define Acc8gLSB 4096
#define ACC16gLSB 2048
//FS_SEL Full Scale Range LSB Sensitivity
//0   ± 250 °/s	 131 LSB/°/s
//1	  ± 500 °/s			 65.5 LSB/°/s
//2   ± 1000 °/s		 32.8 LSB/°/s
//3   ± 2000 °/s		 16.4 LSB/°/s
#define Gyro250LSB 131
#define Gyro500LSB 65.5
#define Gyro1000LSB 32.8
#define Gyro2000LSB	16.4
#define q30  1073741824.0f
static signed char gyro_orientation[9] = {-1, 0, 0,
                                           0,-1, 0,
                                           0, 0, 1};
typedef struct 
{
	float x,y,z;	
}VectorFloat;


uint8_t dmpGetGravity(VectorFloat *v);
// 得到最新的SVM值
float getSVM(void);
// 得到最新的角度值
float getAng(void);
// initilize the mpu and DMP device
int mpuDMPinit(void);

// fall detection algorithm
void checkAlgo(void);

void incrsSVM_Th1(void);
void incrsSVM_Th2(void);
void incrsSVM_Th3(void);
void restoreSVM_Th(void);

void incrsAng_Th1(void);
void incrsAng_Th2(void);
void restoreAng_Th(void);

void setStop(void);
void resetStop(void);

#endif
