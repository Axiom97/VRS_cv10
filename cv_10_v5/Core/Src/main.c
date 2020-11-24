/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>


void SystemClock_Config(void);

void proccesDmaData(const uint8_t* data, uint16_t len);

void setDutyCycle(uint8_t D);


static uint8_t start_flag=0;
uint8_t index_buffera=0;
uint8_t buffer[35];

uint8_t mode=1; // if 1, mode is in auto mode. If 0, its manual
uint8_t intensity=99;




int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim3);

  USART2_RegisterCallback(proccesDmaData);


  while (1)
  {

  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0)
  {
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }
  LL_SetSystemCoreClock(8000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

void proccesDmaData(const uint8_t* data, uint16_t len){

	static char vzor_Manual[]="manual", vzor_Auto[]="auto", vzor_PWM[]="PWM";
	uint8_t count=0;
	char value_of_intensity[3];

	for(uint8_t i = 0; i<len; i++){

		if(start_flag){
			if (*(data+i) == '$'){
				start_flag=0;
				index_buffera=0;
				if(!strcmp(buffer,vzor_Manual)){
					mode=0;
				}
				else if(!strcmp(buffer,vzor_Auto)){
					mode=1;
				}
				else if(!mode) {
					for(uint8_t j=0; j<3; j++){
						if(buffer[j]==vzor_PWM[j]){
							count++;
						}
					}
					if(count==3){
						value_of_intensity[0]=buffer[3];
						value_of_intensity[1]=buffer[4];
						intensity=atoi(value_of_intensity);
					}
				}
				memset(buffer,'\000',sizeof(buffer));
			}
			else{
				buffer[index_buffera++]=*(data+i);
			}

			if(index_buffera>34){
				index_buffera=0;
				start_flag=0;
				memset(buffer,'\000',sizeof(buffer));
			}
		}
		else if (*(data+i) == '$'){
			start_flag=1;
		}
	}
}

void setDutyCycle(uint8_t D){
	TIM2->CCR1 = D;
}



void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
