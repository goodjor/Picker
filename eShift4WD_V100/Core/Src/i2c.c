/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
uint8_t I2C_Buffer_Write[256];
uint8_t I2C_Buffer_Read[256];	//两个数组，分别用于往EEPROM里写数据和存储从EEPROM里读出的数据

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//测试AT24CXX是否可以正常读写
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	uint8_t dat=0x55;
	AT24C02_Read(Addr_AT24C02_CheckRW,&temp,1);			   
	if(temp==0x55)
	{
		return 0;	
	}   
	else//排除第一次初始化的情况
	{
		AT24C02_Write(Addr_AT24C02_CheckRW,&dat,1);
		HAL_Delay(10);		//写一次和读一次之间需要短暂的延时
		AT24C02_Read(Addr_AT24C02_CheckRW,&temp,1);	
		HAL_Delay(10);	  
		if(temp==0x55) return 0;
	}
	return 1;											  
}

void EEPROM_DATA_Init(void)
{
	uint8_t temp;
	uint8_t dat = 0x5a;
	uint8_t ret;


	ret = AT24CXX_Check();
	if (ret == 0)
		printf("AT24CXX_Check  success!\r\n");
	else
		printf("AT24CXX_Check  fail!\r\n");
	AT24C02_Read( Addr_AT24C02_CheckNO,&temp,1);
	if (temp != dat)
	{
		printf("New board, Init EEPROM!\r\n");
		AT24C02_Write(Addr_AT24C02_CheckNO,&dat,1);
		EEPROM_Init();
	}
	Read_EEPROM();
}

void EEPROM_Init(void)
{	
//	//依次写入 四轮驱动的比例系数

	HAL_Delay(10);//hcx此延时不可删除
	AT24C02_WriteLenByte(Addr_Proportion_Value,Default_Proportion_Value,1);
	HAL_Delay(10);
	AT24C02_WriteLenByte(Addr_Control_Pressure,Default_Control_Pressure,2);
	HAL_Delay(10);


}


void Read_EEPROM(void)
{
	
//	//依次读取EEPROM中的 控制压力、比例系数

	shift_geer.Contol_Pressure = AT24C02_ReadLenByte(Addr_Control_Pressure,2);
	HAL_Delay(10);
		
	shift_geer.Proportion_Value = AT24C02_ReadLenByte(Addr_Proportion_Value,1);
	HAL_Delay(10);

	printf("shift_geer.Contol_Pressure: %d \r\n",shift_geer.Contol_Pressure);
	printf("shift_geer.Proportion_Value: %d \r\n",shift_geer.Proportion_Value);

}

void AT24C02_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint8_t NumToWrite)
{
	HAL_I2C_Mem_Write(&hi2c1, ADDR_AT24C02_Write, WriteAddr, I2C_MEMADD_SIZE_8BIT, pBuffer, NumToWrite, 1000);
}

void AT24C02_WriteLenByte(uint16_t WriteAddr,uint16_t DataToWrite,uint8_t Len)
{  	
	uint8_t dat[4]={0};

	for(int i = 0; i < Len; i++)
	{
		dat[i] = (DataToWrite >> (i*8)) & 0xFF;	
	}

	AT24C02_Write(WriteAddr,dat,Len);
										    
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24C02_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint8_t NumToRead)
{
	HAL_I2C_Mem_Read(&hi2c1, ADDR_AT24C02_Read, ReadAddr, I2C_MEMADD_SIZE_8BIT, pBuffer, NumToRead, 0xFFFF);

} 

void At24c02_Read_Byte(uint16_t addr, uint8_t* read_buf)
{
	HAL_I2C_Mem_Read(&hi2c1, ADDR_AT24C02_Read, addr, I2C_MEMADD_SIZE_8BIT, read_buf, 1, 0xFFFF);
	
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
uint32_t AT24C02_ReadLenByte(uint16_t ReadAddr,uint8_t Len)
{  	
	uint8_t i;
	uint8_t dat[4] = {0};
	uint32_t temp = 0;
	for(i = 0; i <Len ; i++)
	{
		At24c02_Read_Byte(ReadAddr+i,dat+i);
		temp += dat[i]<<(i*8);
	}

	return temp;	
											    
}

/* USER CODE END 1 */
