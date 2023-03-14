/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdlib.h"
#include "usart.h"
#include "process.h"
#include "tim.h"
#include "adc.h"
#include "pid.h"
#include "can.h"
#include "i2c.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PB12_DO1_Pin GPIO_PIN_12
#define PB12_DO1_GPIO_Port GPIOB
#define PB13_DO2_Pin GPIO_PIN_13
#define PB13_DO2_GPIO_Port GPIOB
#define LED_Pin_Pin GPIO_PIN_3
#define LED_Pin_GPIO_Port GPIOB
#define PB4_DI1_Pin GPIO_PIN_4
#define PB4_DI1_GPIO_Port GPIOB
#define PB5_DI2_Pin GPIO_PIN_5
#define PB5_DI2_GPIO_Port GPIOB
#define PB8_DI3_Pin GPIO_PIN_8
#define PB8_DI3_GPIO_Port GPIOB
#define PB9_DI4_Pin GPIO_PIN_9
#define PB9_DI4_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

//DEBUG 调试
#define DEBUG							0   //调试打印 1打印  0不打印

//数据采集宏定义
#define FILTER_N 					101
#define	ADC1_CHANNEL_CNT 	4
#define SENSITIVITY				(101-data_storage.Sensitivity)
#define AUTO 							0
#define MANUAL 						1

//数据初始化默认值
#define DEFAULT_SETHEIGHT 			1000  //0.1mm
#define DEFAULT_SENSITUVITY 		10
#define DEFAULT_RAPIDITY 				10
#define DEFAULT_PWM_UP_VALUE 		600
#define DEFAULT_PWM_DOWN_VALUE  400
#define DEFAULT_HEIGHT_MIN   		372
#define DEFAULT_HEIGHT_MAX			3351
#define DEFAULT_SENSOR_B				275


//封装EEPROM地址
#define Addr_SetHeight 				2
#define Addr_Sensitivity 			4
#define Addr_Rapidity 				6
#define Addr_PWM_Value_Up 		8
#define Addr_PWM_Value_Down 	10
#define Addr_Sensor1_Min		  12
#define Addr_Sensor1_Max		  14
#define Addr_Sensor2_Min		  16
#define Addr_Sensor2_Max		  18
#define Addr_Sensor3_Min		  20
#define Addr_Sensor3_Max		  22
#define Addr_Sensor4_Min		  24
#define Addr_Sensor4_Max		  26
#define Addr_SensorB 					28
#define Addr_AT24C02_CheckNO  240
#define Addr_AT24C02_CheckRW  255


//比例阀
#define PWM_Choose_1 				1
#define PWM_Choose_2 				2
#define PWM_Up							1
#define PWM_Down						2

//电磁阀   阀门1和2的选择   阀门的状态
#define Drive_Choose_1 			1
#define Drive_Choose_2 			2
#define Drive_On 						GPIO_PIN_SET
#define Drive_Off 					GPIO_PIN_RESET


typedef struct {	
	uint16_t Min;											//传感器最小值
	uint16_t Max;											//传感器最大值
}HEIGHT;

//采头参数
typedef struct {	
	uint16_t SensorMin[4];						//传感器最小值
	uint16_t SensorMax[4];						//传感器最大值
	HEIGHT 	 Height;
	float 	 SensorK[4];   						//线性方程比例系数
	float    SensorB[4];									//线性方程截距
}PICKER_PAR;


//存储采集到的的数据的结构体
typedef struct {
	uint8_t DI1_State;
	uint8_t DI2_State;
	uint8_t DI3_State;
	uint8_t DI4_State;		//   GND/开  输入

	uint16_t ADC1_Val_Buffer[ADC1_CHANNEL_CNT];		//ADC读取的值
	uint16_t ADC1_DataBuff1[FILTER_N];
	uint16_t ADC1_DataBuff2[FILTER_N];		//用于滑动滤波算法
	uint16_t ADC1_DataBuff3[FILTER_N];
	uint16_t ADC1_DataBuff4[FILTER_N];		//用于滑动滤波算法


}DATA_COLLECT;


//存储参数的结构体
typedef struct {
	
	uint16_t SetHeight;			//设定高度值
	uint8_t  Sensitivity;				//灵敏度0-100
	uint8_t  Rapidity;     		 	//快速性0-100
	uint16_t PWM_Value_Up;			//手动上升占空比
	uint16_t PWM_Value_Down;		//手动下降占空比
	PICKER_PAR PickerPar;
	PICKER_PAR EEPROM_Init_Par;
	

}DATA_STORAGE;


//EEPROM的一些标志位  、自动手动仿形的标志位等
typedef struct {

	uint8_t Write_EEPROM_H_Flag;
	uint8_t Write_EEPROM_PWM_S_R_Flag;
	uint8_t Write_EEPROM_Picker_Height_Flag;
	
	uint8_t Write_EEPROM_CheckNO_Flag;
	uint8_t Write_EEPROM_CheckRW_Flag;

	uint8_t Auto_Manual_Flag;		// 自动仿形的标志位：1） Auto为自动   2） Manual为手动
	uint8_t L_R_Picker_Choose_Flag; // 0为左采头，1为右采头

	uint8_t Manual_Up;  //
	uint8_t Manual_Down;
	uint8_t Calibration_Flag;
	uint8_t Calibration_Success;
	uint8_t sersor_state[4];//1有效，0失效
	
}BIT_FLAG;

//储存PWM的占空比的值 、左右传感器的角度、ADC滤波后的值
typedef struct {
		//PWM占空比的值
	uint16_t PWM_Value_Up;			//手动上升占空比
	uint16_t PWM_Value_Down;		//手动下降占空比
	uint16_t ADC1_Voltage[4];
	uint16_t ADC1_Sensor_Height[4];
	uint16_t ADC1_Val_Buffer[4];		
	uint16_t ADC1_after_Filter[4];
	
}DATA_RESULT;

//电磁阀控制输出信号结构体 
typedef struct {	

	uint16_t PWM_Channel_Up;//PWM控制上升比例阀通道
	uint16_t PWM_Channel_Down;//PWM控制下降比例阀通道
	uint16_t PWM_Up_Value;//上升比例阀PWM 占空比
	uint16_t PWM_Down_Value;//下降比例阀PWM 占空比
	uint16_t Drive_Channel_1;//下降电磁阀PWM 占空比
	uint16_t Drive_Channel_2;//电磁阀备用
	GPIO_PinState  DownValve_01;//下降电磁阀的状态（打开或关闭）
	GPIO_PinState  DownValve_02;//备用电磁阀的状态（打开或关闭）
	
}OUT_PAR;



extern DATA_COLLECT data_collect;
extern DATA_STORAGE data_storage;
extern DATA_RESULT  data_result;
extern BIT_FLAG			bit_flag;
extern OUT_PAR		  out_par;


extern uint16_t Default_SetHeight;
extern uint16_t Default_Sensitivity;
extern uint16_t Default_Rapidity;
extern uint16_t Default_PWM_Value_Up;
extern uint16_t Default_PWM_Value_Down;
extern uint16_t Default_Height_Min;
extern uint16_t Default_Height_Max;

HEIGHT PickerHeight_Get(uint8_t sersor_state[]);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
