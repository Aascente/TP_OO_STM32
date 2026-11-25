/**
  ******************************************************************************
  * @file    BSP/CM4/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program for Cortex-M4 (Dedicated Display & Rendering Core)
  ******************************************************************************
  */

#include "main.h"

#define HSEM_ID_0 (0U) /* HW semaphore 0: boot sync from CM7 */

void CM4_Renderer_Init(void);
void CM4_Renderer_Loop(void);
static void CM4_MPU_Config(void);

int main(void)
{
  /* HW semaphore Clock enable */
  __HAL_RCC_HSEM_CLK_ENABLE();

  /* Activate HSEM notification for Cortex-M4 */
  HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

  /*
    Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7
    to finish clock, SDRAM, LCD, and shared SRAM4 setup.
  */
  HAL_PWREx_ClearPendingEvent();
  HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);

  /* Clear HSEM flag */
  __HAL_HSEM_CLEAR_FLAG(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

  /* Update SystemCoreClock after CM7 configured PLL1 (200 MHz on CM4) */
  SystemCoreClockUpdate();

  /* Configure CM4 MPU for SDRAM (0xD0000000) & shared SRAM4 (0x38000000) */
  CM4_MPU_Config();

  /* Initialize Cortex-M4 HAL (SysTick at 1ms) */
  HAL_Init();

  /* Initialize Display Renderer on CM4 */
  CM4_Renderer_Init();

  /* Render loop driven by CM7 frameSeq updates in D3 SRAM4 */
  while (1)
  {
    CM4_Renderer_Loop();
  }
}

static void CM4_MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  HAL_MPU_Disable();

  /* External SDRAM (32MB @ 0xD0000000): Normal non-cacheable bufferable for fast LCD pixel writes */
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress      = 0xD0000000U;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Shared D3 SRAM4 (64KB @ 0x38000000): Normal non-cacheable shareable */
  MPU_InitStruct.BaseAddress      = 0x38000000U;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_64KB;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  while (1) {}
}
#endif
