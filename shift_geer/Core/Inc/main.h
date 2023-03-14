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
#include "tim.h"
#include "adc.h"
#include "process.h"
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

#define ADC1_CHANNEL_CNT					2
#define FILTER_N 									101
#define Default_Proportion_Value 	10
#define Default_Control_Pressure 	10
#define Default_Filter_Len				5
#define Channel_1 								1

//封装EEPROM地址
#define Addr_Control_Pressure 2
#define Addr_Proportion_Value 4

#define Addr_AT24C02_CheckNO 240
#define Addr_AT24C02_CheckRW 255


//数据采集结构体
typedef struct {	
	
	uint8_t Neutral;
	uint8_t High_Speed;
	uint8_t Low_Speed;
	uint16_t ADC1_Value;
	uint16_t ADC1_Val_Buffer[ADC1_CHANNEL_CNT];		//ADC读取的值
	uint16_t ADC1_DataBuff1[FILTER_N];
	uint16_t ADC1_DataBuff2[FILTER_N];		//用于滑动滤波算法
	uint16_t Filter_Len;
	
}DATA_COLLECT;

//电子换挡结构体
typedef struct {	
	uint8_t Level;
	DATA_COLLECT data_collect;
	uint8_t FWD;
	uint8_t Proportion_Value;
	uint16_t Contol_Pressure;
	uint16_t Real_Pressure;
	uint16_t PWM_Value;
	uint16_t PWM_Channel;
	uint16_t ADC1_Val_Buffer[ADC1_CHANNEL_CNT];
	uint16_t ADC1_after_Filter1;
	uint16_t ADC1_after_Filter2;
	uint8_t Write_EEPROM_Flag;
}SHIFT_GEER;




extern SHIFT_GEER 	shift_geer;
extern DATA_COLLECT data_collect;



/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
