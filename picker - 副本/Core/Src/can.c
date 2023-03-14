/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */
#include "usart.h"

CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
uint8_t TxData[8];
uint8_t Rx_Data[8];
uint32_t TxMailbox;
/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 18;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_3TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */


//发送手动上升和下降占空比 、 灵敏度、快速性、仿形标志、手动升标志、手动降标志
void CAN_Transmit_C1D7(void)
{
	TxHeader.StdId = 0;
	TxHeader.ExtId = 0x19C1D2D7;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_EXT;
	TxHeader.DLC = 8;

	TxData[0] = 0xFF & data_storage.PWM_Value_Up;
	TxData[1] = 0xFF & (data_storage.PWM_Value_Up>>8);
	TxData[2] = 0xFF & data_storage.PWM_Value_Down;
	TxData[3] = 0xFF & (data_storage.PWM_Value_Down>>8);
	TxData[4] = 0xFF & data_storage.Sensitivity;
	TxData[5] = 0xFF & data_storage.Rapidity;
	TxData[6] = (0x01 & bit_flag.Auto_Manual_Flag) | 
							(0x02 & (bit_flag.Manual_Up<<1)) | 	
							(0x04 & (bit_flag.Manual_Down<<2));
	TxData[7] = data_storage.Set_Height;
	
	if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		Error_Handler();
	}
}
//发送采头实际高度、采头实际上升占空比、采头实际下降占空比
void CAN_Transmit_C2D7orD8(void)
{
	TxHeader.StdId = 0;
	if(bit_flag.L_R_Picker_Choose_Flag == 0)
	{
		TxHeader.ExtId = 0x19C2D2D7;
	}else
	{
		TxHeader.ExtId = 0x19C2D2D8;
	}

	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_EXT;
	TxHeader.DLC = 8;

	TxData[0] = data_result.ADC1_Height;
	TxData[1] = 0xFF;
	TxData[2] = 0xFF;
	TxData[3] = 0xFF;
	TxData[4] = 0xFF & out_par.PWM_Up_Value;
	TxData[5] = 0xFF & (out_par.PWM_Up_Value>>8);
	TxData[4] = 0xFF & out_par.PWM_Down_Value;
	TxData[5] = 0xFF & (out_par.PWM_Down_Value>>8);
	
	if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		Error_Handler();
	}
}

//传感器参数标定
void CAN_Transmit_Calibration(void)
{
	TxHeader.StdId = 0;
	TxHeader.ExtId = 0x19C3D2D7;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_EXT;
	TxHeader.DLC = 8;

	TxData[0] = bit_flag.Calibration_Success;
	TxData[1] = 0;
	TxData[2] = 0;
	TxData[3] = 0;
	TxData[4] = 0;
	TxData[5] = 0;
	TxData[6] = 0;
	TxData[7] = 0;
	
	if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		Error_Handler();
	}
	
}

//采头传感器1、2的最高最低设定值
void CAN_Transmit_C4D7orD8(void)
{
	TxHeader.StdId = 0;
	if(bit_flag.L_R_Picker_Choose_Flag == 0)
	{
		TxHeader.ExtId = 0x19C4D2D7;
	}else
	{
		TxHeader.ExtId = 0x19C4D2D8;
	}

	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_EXT;
	TxHeader.DLC = 8;

	TxData[0] = 0xFF & data_storage.Picker_Set.Left1_Min;
	TxData[1] = 0xFF & (data_storage.Picker_Set.Left1_Min>>8);
	TxData[2] = 0xFF & data_storage.Picker_Set.Left1_Max;
	TxData[3] = 0xFF & (data_storage.Picker_Set.Left1_Max>>8);
	printf("%d\r\n",data_storage.Picker_Set.Left1_Max);
	TxData[4] = 0xFF & data_storage.Picker_Set.Right1_Min;
	TxData[5] = 0xFF & (data_storage.Picker_Set.Right1_Min>>8);
	TxData[6] = 0xFF & data_storage.Picker_Set.Right1_Max;
	TxData[7] = 0xFF & (data_storage.Picker_Set.Right1_Max>>8);
	
	if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		Error_Handler();
	}
}

//采头传感器3、4的最高最低设定值
void CAN_Transmit_C5D7orD8(void)
{
	TxHeader.StdId = 0;
	if(bit_flag.L_R_Picker_Choose_Flag == 0)
	{
		TxHeader.ExtId = 0x19C5D2D7;
	}else
	{
		TxHeader.ExtId = 0x19C5D2D8;
	}
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_EXT;
	TxHeader.DLC = 8;
	#if !Model_Switch
	TxData[0] = 0;
	TxData[1] = 0;
	TxData[2] = 0;
	TxData[3] = 0;
	TxData[4] = 0;
	TxData[5] = 0;
	TxData[6] = 0;
	TxData[7] = 0;
	#endif	
	#if Model_Switch
	TxData[0] = 0xFF & data_storage.Picker_Set.Left2_Min;
	TxData[1] = 0xFF & (data_storage.Picker_Set.Left2_Min>>8);
	TxData[2] = 0xFF & data_storage.Picker_Set.Left2_Max;
	TxData[3] = 0xFF & (data_storage.Picker_Set.Left2_Max>>8);
	TxData[4] = 0xFF & data_storage.Picker_Set.Right2_Min;
	TxData[5] = 0xFF & (data_storage.Picker_Set.Right2_Min>>8);
	TxData[6] = 0xFF & data_storage.Picker_Set.Right2_Max;
	TxData[7] = 0xFF & (data_storage.Picker_Set.Right2_Max>>8);
	#endif

	
	if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		Error_Handler();
	}
}

//采头传感器1、2的最高最低实时值
void CAN_Transmit_C6D7orD8(void)
{
	TxHeader.StdId = 0;
	if(bit_flag.L_R_Picker_Choose_Flag == 0)
	{
		TxHeader.ExtId = 0x19C6D2D7;
	}else
	{
		TxHeader.ExtId = 0x19C6D2D8;
	}
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_EXT;
	TxHeader.DLC = 8;

	TxData[0] = 0xFF & data_storage.Picker_Real.Left1_Min;
	TxData[1] = 0xFF & (data_storage.Picker_Real.Left1_Min>>8);
	TxData[2] = 0xFF & data_storage.Picker_Real.Left1_Max;
	TxData[3] = 0xFF & (data_storage.Picker_Real.Left1_Max>>8);
	TxData[4] = 0xFF & data_storage.Picker_Real.Right1_Min;
	TxData[5] = 0xFF & (data_storage.Picker_Real.Right1_Min>>8);
	TxData[6] = 0xFF & data_storage.Picker_Real.Right1_Max;
	TxData[7] = 0xFF & (data_storage.Picker_Real.Right1_Max>>8);
	
	if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		Error_Handler();
	}
}

//采头传感器3、4的最高最低实时值
void CAN_Transmit_C7D7orD8(void)
{
	TxHeader.StdId = 0;
	if(bit_flag.L_R_Picker_Choose_Flag == 0)
	{
		TxHeader.ExtId = 0x19C7D2D7;
	}else
	{
		TxHeader.ExtId = 0x19C7D2D8;
	}

	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_EXT;
	TxHeader.DLC = 8;
	
	#if !Model_Switch
	TxData[0] = 0;
	TxData[1] = 0;
	TxData[2] = 0;
	TxData[3] = 0;
	TxData[4] = 0;
	TxData[5] = 0;
	TxData[6] = 0;
	TxData[7] = 0;
	#endif	
	#if Model_Switch
	TxData[0] = 0xFF & data_storage.Picker_Real.Left2_Min;
	TxData[1] = 0xFF & (data_storage.Picker_Real.Left2_Min>>8);
	TxData[2] = 0xFF & data_storage.Picker_Real.Left2_Max;
	TxData[3] = 0xFF & (data_storage.Picker_Real.Left2_Max>>8);
	TxData[4] = 0xFF & data_storage.Picker_Real.Right2_Min;
	TxData[5] = 0xFF & (data_storage.Picker_Real.Right2_Min>>8);
	TxData[6] = 0xFF & data_storage.Picker_Real.Right2_Max;
	TxData[7] = 0xFF & (data_storage.Picker_Real.Right2_Max>>8);
	#endif

	
	if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		Error_Handler();
	}
}

//采头传感器的实时值
void CAN_Transmit_C8D7orD8(void)
{
	TxHeader.StdId = 0;
	if(bit_flag.L_R_Picker_Choose_Flag == 0)
	{
		TxHeader.ExtId = 0x19C8D2D7;
	}else
	{
		TxHeader.ExtId = 0x19C8D2D8;
	}

	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_EXT;
	TxHeader.DLC = 8;
	TxData[0] = 0xFF & data_result.ADC1_after_Filter1;
	TxData[1] = 0xFF & (data_result.ADC1_after_Filter1>>8);

	TxData[2] = 0xFF & data_result.ADC1_after_Filter2;
	TxData[3] = 0xFF & (data_result.ADC1_after_Filter2>>8);

	#if !Model_Switch
	TxData[4] = 0;
	TxData[5] = 0;
	TxData[6] = 0;
	TxData[7] = 0;
	#endif	
	#if Model_Switch
	TxData[4] = 0xFF & data_result.ADC1_after_Filter3;
	TxData[5] = 0xFF & (data_result.ADC1_after_Filter3>>8);
	TxData[6] = 0xFF & data_result.ADC1_after_Filter4;
	TxData[7] = 0xFF & (data_result.ADC1_after_Filter4>>8);
	#endif

	
	if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		Error_Handler();
	}
}





void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

	//    __HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);//重新开启FIF00消息挂号中断

	/* Get RX message */

	//	__disable_irq();

	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, Rx_Data) != HAL_OK) {
		/* Reception Error */
		Error_Handler();
		return;
	}

	if (RxHeader.IDE == CAN_ID_EXT){ 	//确保接收到扩展帧数据
		

			switch (RxHeader.ExtId)
			{
				/********电子仿形*********/

				case 0x19C1D7D2:	data_storage.Set_Height= Rx_Data[0];

													bit_flag.Write_EEPROM_H_Flag = 1;

				break;
				
				case 0x19C2D7D2:	data_storage.PWM_Value_Up= (Rx_Data[0] | (Rx_Data[1]<<8) );
													data_storage.PWM_Value_Down = (Rx_Data[2] | (Rx_Data[3]<<8) );
													data_storage.Sensitivity = Rx_Data[4];
													data_storage.Rapidity = Rx_Data[5];
													
													bit_flag.Write_EEPROM_PWM_S_R_Flag = 1;

				break;
				case 0x19C3D7D2:	bit_flag.Calibration_Flag = Rx_Data[0];

													
				break;

				default:
				break;
			}
			
		
	
	}
	
	for (int i = 0; i < 8; i++) {
		Rx_Data[i] = 0;
	}
	RxHeader.ExtId = 0;
}

void CAN_User_Init(void) //用户初始化函数
{
	CAN_FilterTypeDef sFilterConfig;
	HAL_StatusTypeDef HAL_Status;


	sFilterConfig.FilterBank = 0;					  //过滤器0
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK; //设为列表模式
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;//CAN过滤器尺度，16位或32位

	sFilterConfig.FilterIdHigh = 0;//32位下，存储要过滤ID的高16位
	sFilterConfig.FilterIdLow = 0;//32位下，存储要过滤ID的低16位

	sFilterConfig.FilterMaskIdHigh = 0;//掩码模式下，存储的是掩码
	sFilterConfig.FilterMaskIdLow = 0;


	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; //接收到的报文放入到FIFO0中

	sFilterConfig.FilterActivation = ENABLE; //激活过滤器
	sFilterConfig.SlaveStartFilterBank = 14;
	if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK) 
	{
		Error_Handler();
  }

	HAL_Status = HAL_CAN_Start(&hcan); //开启CAN
	if (HAL_Status != HAL_OK)
	{
		printf("开启CAN失败\r\n");
	}
	HAL_Status = HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	if (HAL_Status != HAL_OK)
	{
		printf("开启挂起中段允许失败\r\n");
	}
}
/* USER CODE END 1 */
