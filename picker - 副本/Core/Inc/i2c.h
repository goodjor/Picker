/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.h
  * @brief   This file contains all the function prototypes for
  *          the i2c.c file
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
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN Private defines */
#define ADDR_AT24C02_Write  0xA0
#define ADDR_AT24C02_Read 	0xA1

extern uint8_t I2C_Buffer_Write[256];
extern uint8_t I2C_Buffer_Read[256];	//两个数组，分别用于往EEPROM里写数据和存储从EEPROM里读出的数据 
																			//全局变量声明

/* USER CODE END Private defines */

void MX_I2C1_Init(void);

/* USER CODE BEGIN Prototypes */
uint8_t AT24CXX_Check(void);
void EEPROM_DATA_Init(void);
void EEPROM_Init(void);
void Read_EEPROM(void);
void AT24C02_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint8_t NumToWrite);
void AT24C02_WriteLenByte(uint16_t WriteAddr,uint16_t DataToWrite,uint8_t Len);
void AT24C02_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint8_t NumToRead);
void At24c02_Read_Byte(uint16_t addr, uint8_t* read_buf);
uint32_t AT24C02_ReadLenByte(uint16_t ReadAddr,uint8_t Len);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

