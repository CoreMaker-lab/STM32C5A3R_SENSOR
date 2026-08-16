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

#include "mx_usart1.h"
#include <stdio.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype -----------------------------------------------*/

int _write(int file, char *ptr, int len)
{
    hal_uart_handle_t *huart1 = mx_usart1_uart_gethandle();

    if (huart1 != NULL)
    {
        HAL_UART_Transmit(huart1, ptr, len, 1000);
    }

    return len;
}

/* DAC reference voltage, unit: mV */
#define DAC_VREF_MV        3300U

/* Target DAC output voltage, unit: mV */
#define DAC_OUTPUT_MV      1650U

/* 12-bit DAC maximum digital value */
#define DAC_MAX_VALUE      4095U

/* Calculate DAC digital value */
#define DAC_OUTPUT_VALUE   ((DAC_OUTPUT_MV * DAC_MAX_VALUE) / DAC_VREF_MV)

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

	    /* Get DAC1 handle */
	    hal_dac_handle_t *hdac1 = mx_dac1_gethandle();

	    if (hdac1 == NULL)
	    {
	        return (-1);
	    }

	    /*
	     * Set DAC Channel 1 data.
	     *
	     * VOUT = VREF+ ¡Á DOR / 4095
	     *
	     * VREF+ = 3.3 V
	     * VOUT  = 1.65 V
	     *
	     * DOR ¡Ö 2047
	     */
	    if (HAL_DAC_SetChannelData(hdac1,
	                               HAL_DAC_CHANNEL_1,
	                               DAC_OUTPUT_VALUE) != HAL_OK)
	    {
	        return (-1);
	    }

	    /* Start DAC1 Channel 1 */
	    if (HAL_DAC_StartChannel(hdac1,
	                             HAL_DAC_CHANNEL_1) != HAL_OK)
	    {
	        return (-1);
	    }

	    printf("STM32C542 DAC Test\r\n");
	    printf("DAC1_OUT1: PA4\r\n");
	    printf("DAC Output: %lu mV\r\n", (unsigned long)DAC_OUTPUT_MV);
	    printf("DAC Value : %lu\r\n", (unsigned long)DAC_OUTPUT_VALUE);


    while (1) {}
  }
} /* end main */

