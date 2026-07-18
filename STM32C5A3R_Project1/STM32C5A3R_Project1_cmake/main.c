/**
  ******************************************************************************
  * file           : main.c
  * brief          : Main program body
  *                  Calls target system initialization then loop in main.
  ******************************************************************************
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype -----------------------------------------------*/

/**
  * brief:  The application entry point.
  * retval: none but we specify int to comply with C99 standard
  */
int main(void)
{
  /** System Init: this code placed in targets folder initializes your system.
    * It calls the initialization (and sets the initial configuration) of the peripherals.
    * You can use STM32CubeMX to generate and call this code or not in this project.
    * It also contains the HAL initialization and the initial clock configuration.
    */
  if (mx_system_init() != SYSTEM_OK)
  {
    return (-1);
  }
  else
  {
    /*
      * You can start your application code here
      */
	    while (1) {

	    	HAL_GPIO_TogglePin(HAL_GPIOB, HAL_GPIO_PIN_14);
	    	HAL_GPIO_TogglePin(HAL_GPIOB, HAL_GPIO_PIN_15);
	    	HAL_GPIO_TogglePin(HAL_GPIOA, HAL_GPIO_PIN_8);
	    	HAL_Delay(500);
	    	HAL_GPIO_TogglePin(HAL_GPIOB, HAL_GPIO_PIN_14);
	    	HAL_GPIO_TogglePin(HAL_GPIOB, HAL_GPIO_PIN_15);
	    	HAL_GPIO_TogglePin(HAL_GPIOA, HAL_GPIO_PIN_8);
	    	HAL_Delay(500);

	        HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, HAL_GPIO_PIN_SET);
	        HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, HAL_GPIO_PIN_SET);
	        HAL_GPIO_WritePin(LED3_PORT, LED3_PIN, HAL_GPIO_PIN_SET);
	        HAL_Delay(500);

	        HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, HAL_GPIO_PIN_RESET);
	        HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, HAL_GPIO_PIN_RESET);
	        HAL_GPIO_WritePin(LED3_PORT, LED3_PIN, HAL_GPIO_PIN_RESET);
	        HAL_Delay(500);

	    }
  }
} /* end main */

