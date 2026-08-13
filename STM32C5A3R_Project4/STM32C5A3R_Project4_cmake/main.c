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
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype -----------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define TIM1_CLK_HZ        144000000UL

/*
 * 本实验固定 ARR = 9999
 * 这样一个 PWM 周期为 10000 个计数点，方便计算占空比
 */
#define TIM1_PWM_ARR       9999UL

/* Private function prototypes -----------------------------------------------*/
static hal_status_t TIM1_SetPSC_ARR(uint32_t psc, uint32_t arr);
static hal_status_t TIM1_SetFrequency(uint32_t freq_hz);
static hal_status_t TIM1_SetChannelDuty(hal_tim_channel_t channel,
                                        uint32_t arr,
                                        uint32_t duty_percent);
static hal_status_t TIM1_PWM_Start_CH1_CH3(void);

/**
  * @brief  使用 HAL2 自带函数修改 TIM1 PSC 和 ARR
  * @param  psc: TIM1 预分频值
  * @param  arr: TIM1 自动重装载值
  * @retval HAL status
  */
static hal_status_t TIM1_SetPSC_ARR(uint32_t psc, uint32_t arr)
{
    hal_tim_handle_t *htim1 = mx_tim1_gethandle();

    if (htim1 == NULL)
    {
        return HAL_ERROR;
    }

    /*
     * 使用 HAL2 自带函数修改 PSC 和 ARR
     * PSC 影响计数频率
     * ARR 影响 PWM 周期
     */
    if (HAL_TIM_SetPrescaler(htim1, psc) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (HAL_TIM_SetPeriod(htim1, arr) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /*
     * 修改参数后，将计数器清零，方便观察 PWM 输出变化
     */
    if (HAL_TIM_SetCounter(htim1, 0) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

/**
  * @brief  根据目标频率自动计算 PSC，并设置 TIM1 频率
  * @param  freq_hz: 目标 PWM 频率，单位 Hz
  * @retval HAL status
  */
static hal_status_t TIM1_SetFrequency(uint32_t freq_hz)
{
    uint32_t arr = TIM1_PWM_ARR;
    uint32_t psc;

    if (freq_hz == 0U)
    {
        return HAL_INVALID_PARAM;
    }

    /*
     * PWM 频率 = TIM1_CLK / ((PSC + 1) * (ARR + 1))
     *
     * 反推：
     * PSC + 1 = TIM1_CLK / (PWM频率 * (ARR + 1))
     */
    psc = TIM1_CLK_HZ / (freq_hz * (arr + 1U));

    if (psc == 0U)
    {
        psc = 1U;
    }

    psc = psc - 1U;

    return TIM1_SetPSC_ARR(psc, arr);
}

/**
  * @brief  设置 TIM1 某一个通道的占空比
  * @param  channel: HAL_TIM_CHANNEL_1 / HAL_TIM_CHANNEL_3 等
  * @param  arr: 当前 ARR 值
  * @param  duty_percent: 占空比，范围 0~100
  * @retval HAL status
  */
static hal_status_t TIM1_SetChannelDuty(hal_tim_channel_t channel,
                                        uint32_t arr,
                                        uint32_t duty_percent)
{
    hal_tim_handle_t *htim1 = mx_tim1_gethandle();
    uint32_t pulse;

    if (htim1 == NULL)
    {
        return HAL_ERROR;
    }

    if (duty_percent > 100U)
    {
        duty_percent = 100U;
    }

    /*
     * 占空比 = Pulse / (ARR + 1) * 100%
     *
     * 所以：
     * Pulse = (ARR + 1) * Duty / 100
     */
    pulse = ((arr + 1U) * duty_percent) / 100U;

    /*
     * 使用 HAL2 自带函数修改 CCRx / Pulse
     *
     * TIM1_CH1 -> CCR1
     * TIM1_CH3 -> CCR3
     */
    if (HAL_TIM_OC_SetCompareUnitPulse(
            htim1,
            hal_tim_oc_channel_to_compare_unit(channel),
            pulse) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

/**
  * @brief  启动 TIM1 CH1 和 CH3 PWM 输出
  * @retval HAL status
  */
static hal_status_t TIM1_PWM_Start_CH1_CH3(void)
{
    hal_tim_handle_t *htim1 = mx_tim1_gethandle();

    if (htim1 == NULL)
    {
        return HAL_ERROR;
    }

    /*
     * 启动 TIM1_CH1 输出
     */
    if (HAL_TIM_OC_StartChannel(htim1, HAL_TIM_CHANNEL_1) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /*
     * 启动 TIM1_CH3 输出
     */
    if (HAL_TIM_OC_StartChannel(htim1, HAL_TIM_CHANNEL_3) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /*
     * 启动 TIM1 计数器
     */
    if (HAL_TIM_Start(htim1) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}
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
     * TIM1_CH1 -> PA8
     * TIM1_CH3 -> PA5
     *
     * 当前配置：
     * TIM1_CLK = 144 MHz
     * ARR = 9999
     */

    /*
     * 设置 PWM 频率为 1Hz
     *
     * PWM频率 = 144MHz / ((PSC + 1) * (ARR + 1))
     * ARR = 9999
     * 当 freq = 1Hz 时，PSC = 14399
     */
    if (TIM1_SetFrequency(1) != HAL_OK)
    {
        printf("TIM1_SetFrequency failed\r\n");
    }

    /*
     * 设置多通道占空比
     *
     * CH1 = 25%
     * CH3 = 75%
     */
    if (TIM1_SetChannelDuty(HAL_TIM_CHANNEL_1, TIM1_PWM_ARR, 25) != HAL_OK)
    {
        printf("TIM1 CH1 duty set failed\r\n");
    }

    if (TIM1_SetChannelDuty(HAL_TIM_CHANNEL_3, TIM1_PWM_ARR, 75) != HAL_OK)
    {
        printf("TIM1 CH3 duty set failed\r\n");
    }

    /*
     * 启动 TIM1 CH1 和 CH3 PWM 输出
     */
    if (TIM1_PWM_Start_CH1_CH3() != HAL_OK)
    {
        printf("TIM1 PWM start failed\r\n");
    }
    HAL_Delay(5000);
    while (1)
    {
        printf("TIM1 PWM: 1000Hz, CH1=25%%, CH3=75%%\r\n");

        /*
         * 设置 TIM1 PWM 频率为 1000Hz
         *
         * TIM1_CLK = 144MHz
         * ARR = 999
         * PSC = 143
         *
         * PWM频率 = 144MHz / ((143 + 1) * (999 + 1))
         *         = 1000Hz
         */
        TIM1_SetFrequency(1000);

        /*
         * CH1 输出 25% 占空比
         * CH3 输出 75% 占空比
         */
        TIM1_SetChannelDuty(HAL_TIM_CHANNEL_1, TIM1_PWM_ARR, 25);
        TIM1_SetChannelDuty(HAL_TIM_CHANNEL_3, TIM1_PWM_ARR, 75);

        HAL_Delay(3000);
//
//        printf("TIM1 PWM: 1000Hz, CH1=50%%, CH3=50%%\r\n");
//
//        TIM1_SetFrequency(1000);
//        TIM1_SetChannelDuty(HAL_TIM_CHANNEL_1, TIM1_PWM_ARR, 50);
//        TIM1_SetChannelDuty(HAL_TIM_CHANNEL_3, TIM1_PWM_ARR, 50);
//
//        HAL_Delay(3000);
//
//        printf("TIM1 PWM: 1000Hz, CH1=75%%, CH3=25%%\r\n");
//
//        TIM1_SetFrequency(1000);
//        TIM1_SetChannelDuty(HAL_TIM_CHANNEL_1, TIM1_PWM_ARR, 75);
//        TIM1_SetChannelDuty(HAL_TIM_CHANNEL_3, TIM1_PWM_ARR, 25);
//
//        HAL_Delay(3000);
    }
  }
} /* end main */
