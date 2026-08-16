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

/* 12-bit ADC/DAC maximum digital value */
#define ADC_DAC_MAX_VALUE   4095U

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


	    uint32_t adc_value = 0;
	    uint32_t adc_voltage_mv = 0;


	    /* Get ADC1 handle */
	    hal_adc_handle_t *hadc1 = mx_adc1_gethandle();

	    if (hadc1 == NULL)
	    {
	        return (-1);
	    }

	    /*
	     * Configure ADC1 Channel 1.
	     *
	     * This section can be removed if ADC1_IN1 has already
	     * been added to Regular Sequencer Rank 1 in CubeMX2.
	     */
	    hal_adc_channel_config_t adc_channel_config;

	    adc_channel_config.group          = HAL_ADC_GROUP_REGULAR;
	    adc_channel_config.sequencer_rank = 1;
	    adc_channel_config.sampling_time  = HAL_ADC_SAMPLING_TIME_289CYCLES;
	    adc_channel_config.input_mode     = HAL_ADC_IN_SINGLE_ENDED;

	    if (HAL_ADC_SetConfigChannel(hadc1,
	                                 HAL_ADC_CHANNEL_1,
	                                 &adc_channel_config) != HAL_OK)
	    {
	        return (-1);
	    }

	    /* Activate ADC1 */
	    if (HAL_ADC_Start(hadc1) != HAL_OK)
	    {
	        return (-1);
	    }

	    /* ADC self calibration */
	    if (HAL_ADC_Calibrate(hadc1) != HAL_OK)
	    {
	        return (-1);
	    }
	    printf("\r\nSTM32C542 ADC Voltage Test\r\n");

	    while (1) {

	        /* Start ADC regular conversion */
	        if (HAL_ADC_REG_StartConv(hadc1) == HAL_OK)
	        {
	            /* Wait for conversion complete */
	            if (HAL_ADC_REG_PollForConv(hadc1, 100) == HAL_OK)
	            {
	                /* Read ADC conversion data */
	                adc_value =
	                    (uint32_t)HAL_ADC_REG_ReadConversionData(hadc1);

	                /*
	                 * Convert ADC value to voltage.
	                 *
	                 * Voltage = ADC_Value ¡Á VREF / 4095
	                 */
	                adc_voltage_mv =
	                    (adc_value * DAC_VREF_MV) / ADC_DAC_MAX_VALUE;

	                printf("ADC Value: %4lu, Voltage: %4lu mV\r\n",
	                       (unsigned long)adc_value,
	                       (unsigned long)adc_voltage_mv);
	            }
	            else
	            {
	                printf("ADC conversion timeout!\r\n");
	            }
	        }

	        HAL_Delay(1000);

	    }
  }
} /* end main */

