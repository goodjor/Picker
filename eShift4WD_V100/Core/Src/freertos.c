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
SHIFT_GEER shift_geer;
DATA_COLLECT data_collect;



/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId MyHeartTaskHandle;
osThreadId CAN_TxTaskHandle;
osThreadId ProcessTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void Task_MyHeart(void const * argument);
void Task_CAN_Tx(void const * argument);
void Task_Process(void const * argument);

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
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of MyHeartTask */
  osThreadDef(MyHeartTask, Task_MyHeart, osPriorityRealtime, 0, 64);
  MyHeartTaskHandle = osThreadCreate(osThread(MyHeartTask), NULL);

  /* definition and creation of CAN_TxTask */
  osThreadDef(CAN_TxTask, Task_CAN_Tx, osPriorityHigh, 0, 64);
  CAN_TxTaskHandle = osThreadCreate(osThread(CAN_TxTask), NULL);

  /* definition and creation of ProcessTask */
  osThreadDef(ProcessTask, Task_Process, osPriorityHigh, 0, 64);
  ProcessTaskHandle = osThreadCreate(osThread(ProcessTask), NULL);

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
  /* Infinite loop */
  for(;;)
  {
		if (shift_geer.Write_EEPROM_Flag == 1)
		{
			AT24C02_WriteLenByte(Addr_Control_Pressure,shift_geer.Contol_Pressure,2);
			osDelay(10);
			AT24C02_WriteLenByte(Addr_Proportion_Value,shift_geer.Proportion_Value,1);
			osDelay(10);

			printf("shift_geer.Contol_Pressure = %d \r\n", shift_geer.Contol_Pressure);
			printf("shift_geer.Proportion_Value = %d \r\n", shift_geer.Proportion_Value);

			shift_geer.Write_EEPROM_Flag = 0;
		}
    osDelay(200);
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
//	uint16_t t = 0;
  /* Infinite loop */
  for(;;)
  {

    HAL_IWDG_Refresh(&hiwdg);
		HAL_GPIO_TogglePin(LED_Pin_GPIO_Port,LED_Pin_Pin);
//    printf("LED OK!!  %d \r\n",t++); 		
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
  /* Infinite loop */
  for(;;)
  {
		CAN_Transmit_1();
		osDelay(10);
		CAN_Transmit_2();
    osDelay(200);
  }
  /* USER CODE END Task_CAN_Tx */
}

/* USER CODE BEGIN Header_Task_Process */
/**
* @brief Function implementing the ProcessTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_Process */
void Task_Process(void const * argument)
{
  /* USER CODE BEGIN Task_Process */
  /* Infinite loop */
  for(;;)
  {
		Data_Collect();
		
		ControlLoop();
		
		Contol_Out();
    osDelay(200);//200ms±Õ»·Ò»´Î
  }
  /* USER CODE END Task_Process */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

