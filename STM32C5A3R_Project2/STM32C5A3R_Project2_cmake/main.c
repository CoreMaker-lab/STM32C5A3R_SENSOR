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
#include "stm32c5xx_ll_flash.h"
#include <stdio.h>


/*
 * enable = 1：BOOT_SEL = 1，BOOT0 信号来自外部 BOOT0 引脚
 * enable = 0：BOOT_SEL = 0，BOOT0 信号来自内部 BOOT0 Option Bit
 */
static void Configure_BOOT_SEL(uint8_t enable)
{
    uint32_t target_boot_sel;
    uint32_t current_boot_sel;

    /*
     * STM32C5:
     * BOOT_SEL = 1: BOOT0 from BOOT0 pin
     * BOOT_SEL = 0: BOOT0 from BOOT0 option bit
     *
     * 当前头文件里有 LL_FLASH_OB_BOOT0_BOOTPIN，
     * 但没有 LL_FLASH_OB_BOOT0_BOOT0，所以 BOOT_SEL = 0 直接用 0U。
     */
    target_boot_sel = enable ? LL_FLASH_OB_BOOT0_BOOTPIN : 0U;

    current_boot_sel = LL_FLASH_OB_GetBoot0SourceSelection(FLASH);

    if (current_boot_sel == target_boot_sel)
    {
        printf("BOOT_SEL is already set to the desired value.\r\n");
        return;
    }

    /* 等待 Flash 空闲 */
    while ((FLASH->SR & FLASH_SR_BSY) != 0U)
    {
    }

    /*
     * 清除可能存在的 Option Byte 错误标志。
     * 如果不清除 OPTCHANGEERR，后面 OPTSTRT 可能无法启动。
     */
#ifdef FLASH_CCR_CLR_OPTCHANGEERR
    FLASH->CCR = FLASH_CCR_CLR_OPTCHANGEERR;
#endif

#ifdef FLASH_CCR_CLR_PGSERR
    FLASH->CCR = FLASH_CCR_CLR_PGSERR;
#endif

#ifdef FLASH_CCR_CLR_WRPERR
    FLASH->CCR = FLASH_CCR_CLR_WRPERR;
#endif

#ifdef FLASH_CCR_CLR_STRBERR
    FLASH->CCR = FLASH_CCR_CLR_STRBERR;
#endif

#ifdef FLASH_CCR_CLR_INCERR
    FLASH->CCR = FLASH_CCR_CLR_INCERR;
#endif

#ifdef FLASH_CCR_CLR_EOP
    FLASH->CCR = FLASH_CCR_CLR_EOP;
#endif

    /* 解锁 Flash */
    if (LL_FLASH_IsLocked(FLASH) != 0U)
    {
        LL_FLASH_SetUnlockKey(FLASH, LL_FLASH_KEY1);
        LL_FLASH_SetUnlockKey(FLASH, LL_FLASH_KEY2);
    }

    /* 解锁 Option Bytes */
    if (LL_FLASH_OB_IsLocked(FLASH) != 0U)
    {
        LL_FLASH_OB_SetUnlockKey(FLASH, LL_FLASH_OB_OPTKEY1);
        LL_FLASH_OB_SetUnlockKey(FLASH, LL_FLASH_OB_OPTKEY2);
    }

    if ((LL_FLASH_IsLocked(FLASH) != 0U) || (LL_FLASH_OB_IsLocked(FLASH) != 0U))
    {
        printf("Error: Failed to unlock FLASH or Option Bytes!\r\n");
        return;
    }

    /* 设置 BOOT_SEL */
    LL_FLASH_OB_SetBoot0SourceSelection(FLASH, target_boot_sel);

    /*
     * 启动 Option Bytes 修改。
     * RM0522 中说明：修改 FLASH_xxx_PRG 后，需要置位 OPTSTRT。
     */
    FLASH->OPTCR |= FLASH_OPTCR_OPTSTRT;

    /* 等待 Option Bytes 修改完成 */
    while ((FLASH->SR & FLASH_SR_BSY) != 0U)
    {
    }

    /* 检查是否发生 Option Byte 修改错误 */
    if ((FLASH->SR & FLASH_SR_OPTCHANGEERR) != 0U)
    {
        printf("Error: Option Bytes change failed!\r\n");

#ifdef FLASH_CCR_CLR_OPTCHANGEERR
        FLASH->CCR = FLASH_CCR_CLR_OPTCHANGEERR;
#endif
    }
    else
    {
        printf("BOOT_SEL configured successfully.\r\n");
    }

    /* 锁定 Option Bytes 和 Flash */
    LL_FLASH_OB_Lock(FLASH);
    LL_FLASH_Lock(FLASH);

    /*
     * 修改 Option Bytes 后建议复位，让配置重新加载。
     */
    NVIC_SystemReset();
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
      * You can start your application code here
      */
	  Configure_BOOT_SEL(1);
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

