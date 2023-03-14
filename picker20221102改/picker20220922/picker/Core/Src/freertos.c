/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "usart.h"
#include "can.h"
#include "iwdg.h"
#include "i2c.h"
#include "tim.h"
#include "adc.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
DATA_COLLECT data_collect;
DATA_STORAGE data_storage;
DATA_RESULT  data_result;
BIT_FLAG		 bit_flag;
OUT_PAR			 out_par;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId MyHeartTaskHandle;
osThreadId CAN_TxTaskHandle;
osThreadId Collect_ProcessHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void Task_MyHeart(void const * argument);
void Task_CAN_Tx(void const * argument);
void Task_Collec_ProcessData(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 64);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of MyHeartTask */
  osThreadDef(MyHeartTask, Task_MyHeart, osPriorityRealtime, 0, 64);
  MyHeartTaskHandle = osThreadCreate(osThread(MyHeartTask), NULL);

  /* definition and creation of CAN_TxTask */
  osThreadDef(CAN_TxTask, Task_CAN_Tx, osPriorityHigh, 0, 64);
  CAN_TxTaskHandle = osThreadCreate(osThread(CAN_TxTask), NULL);

  /* definition and creation of Collect_Process */
  osThreadDef(Collect_Process, Task_Collec_ProcessData, osPriorityIdle, 0, 64);
  Collect_ProcessHandle = osThreadCreate(osThread(Collect_Process), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	osDelay(500);//此延时用于角度传感器最大最小值校准
  /* Infinite loop */
  for(;;)
  {
   if (bit_flag.Write_EEPROM_H_Flag == 1)
		{
			AT24C02_WriteLenByte(Addr_SetHeight,data_storage.SetHeight,1);
			osDelay(10);
			#if DEBUG
			printf("SetHeight = %d \r\n", data_storage.SetHeight);
			#endif
			bit_flag.Write_EEPROM_H_Flag = 0;
		}
	
		if (bit_flag.Write_EEPROM_PWM_S_R_Flag == 1)
		{
			AT24C02_WriteLenByte(Addr_PWM_Value_Down,data_storage.PWM_Value_Down,2);
			osDelay(10);
			AT24C02_WriteLenByte(Addr_PWM_Value_Up,data_storage.PWM_Value_Up,2);
			osDelay(10);
			AT24C02_WriteLenByte(Addr_Sensitivity,data_storage.Sensitivity,1);
			osDelay(10);
			AT24C02_WriteLenByte(Addr_Rapidity,data_storage.Rapidity,1);
			osDelay(10);
			#if DEBUG
			printf("PWM_Value_Down = %d \r\n", data_storage.PWM_Value_Down);
			printf("PWM_Value_Up 	 = %d \r\n", data_storage.PWM_Value_Up);
			printf("data_storage.Sensitivity = %d \r\n", data_storage.Sensitivity);
			printf("data_storage.Rapidity    = %d \r\n", data_storage.Rapidity);
			#endif
			bit_flag.Write_EEPROM_PWM_S_R_Flag = 0;
		}	

		//角度传感器最大最小值校准		
		if(bit_flag.Calibration_Flag == 1)
		{
			Hegiht_Calibration();	
			bit_flag.Calibration_Flag = 0;
		}
    osDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Task_MyHeart */
/**
* @brief Function implementing the MyHeartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_MyHeart */
void Task_MyHeart(void const * argument)
{
  /* USER CODE BEGIN Task_MyHeart */
	#if 1
	uint16_t t;		
	#endif

  /* Infinite loop */
  for(;;)
  {
		HAL_IWDG_Refresh(&hiwdg);
		HAL_GPIO_TogglePin(LED_Pin_GPIO_Port,LED_Pin_Pin);
		#if 1
    printf("LED OK!!  %d \r\n",t++); 
		if(t > 300)
		{
			t =300;
		}
		#endif
    osDelay(500);
  }
  /* USER CODE END Task_MyHeart */
}

/* USER CODE BEGIN Header_Task_CAN_Tx */
/**
* @brief Function implementing the CAN_TxTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_CAN_Tx */
void Task_CAN_Tx(void const * argument)
{
  /* USER CODE BEGIN Task_CAN_Tx */
	osDelay(1000);
  /* Infinite loop */
  for(;;)
  {

		CAN_Transmit_C1D7();
		osDelay(10);
		CAN_Transmit_C2D7orD8();
		osDelay(10);
		CAN_Transmit_C4D7orD8();
		osDelay(10);
		CAN_Transmit_C5D7orD8();
		osDelay(10);
		CAN_Transmit_C6D7orD8();
		osDelay(10);
		CAN_Transmit_C7D7orD8();
		osDelay(10);
		CAN_Transmit_C8D7orD8();
		osDelay(10);		
		if(bit_flag.Calibration_Success == 1)
		{
			CAN_Transmit_Calibration();
			osDelay(10);	
			bit_flag.Calibration_Success = 0;
		}
		osDelay(20);			

	}
  /* USER CODE END Task_CAN_Tx */
}

/* USER CODE BEGIN Header_Task_Collec_ProcessData */
/**
* @brief Function implementing the Collect_Process thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_Collec_ProcessData */
void Task_Collec_ProcessData(void const * argument)
{
  /* USER CODE BEGIN Task_Collec_ProcessData */

  /* Infinite loop */
  for(;;)
  {

		/*********数据采集*********/
		Collect_Data();

		/*********数据处理*********/
		Process_Data();
		
		/*********输出*************/
		Control_Out();
		
    osDelay(10);
  }
  /* USER CODE END Task_Collec_ProcessData */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

