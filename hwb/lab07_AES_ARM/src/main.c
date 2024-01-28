// Jiri Bucek 2018, bucekj@fit.cvut.cz
// Using parts from the UART_TwoBoards_ComPolling example, (C) COPYRIGHT 2016 STMicroelectronics


#include "stm32f0xx.h"
#include "stm32f0xx_nucleo.h"
#include "nucleo_serial.h"
#include "aestest.cpp"
			


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED2 on */
  BSP_LED_On(LED2);
  while(1)
  {
    /* Error if LED2 is slowly blinking (1 sec. period) */
    BSP_LED_Toggle(LED2);
    HAL_Delay(1000);
  }
}



/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSI/2)
  *            SYSCLK(Hz)                     = 48000000
  *            HCLK(Hz)                       = 48000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 8000000
  *            PREDIV                         = 1
  *            PLLMUL                         = 12
  *            Flash Latency(WS)              = 1
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* No HSE Oscillator on Nucleo, Activate PLL with HSI/2 as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1)!= HAL_OK)
  {
    Error_Handler();
  }
}


UART_HandleTypeDef UartHandle;
//* huart;

int main(void)
{

	  /* STM32F0xx HAL library initialization:
	       - Configure the Flash prefetch
	       - Systick timer is configured by default as source of time base, but user
	         can eventually implement his proper time base source (a general purpose
	         timer for example or other time source), keeping in mind that Time base
	         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
	         handled in milliseconds basis.
	       - Low Level Initialization
	     */
	  HAL_Init();

	  /* Configure LED2 */
	  BSP_LED_Init(LED2);

	  /* Configure the system clock to 48 MHz */
	  SystemClock_Config();

	  /* init serial communication (UART2, see nucleo_serial.h) */
	  /* from this point on, printf() should work. */
	  serial_init();
	  printf("---------------\n");
	  printf("Number 5 is alive! %lu\n", HAL_GetTick());
#if defined(DEBUG)
	  printf("Source compiled with Debug settings.\n");
#endif

      int retry = 10000;

      printf("run AES basic %d x times\n", retry);
      long unsigned int startTime = HAL_GetTick();
	  int last = mainA(retry);
      long unsigned int endTime = HAL_GetTick();
	  printf("Basic AES finished in %lu ms\n", endTime - startTime);
	  printf("byte[0] of the result: %x\n", last);

      printf("run AES TBOX %d x times\n", retry);
      startTime = HAL_GetTick();
	  last = mainT(retry);
      endTime = HAL_GetTick();
	  printf("TBOX AES finished in %lu ms\n", endTime - startTime);
	  printf("byte[0] of the result: %x\n", last);
	  printf("++++++++++++++++\n");
	  /* Turn LED2 on */
	  BSP_LED_On(LED2);
	  while(1)
	  {
	    /* LED2 is quickly blinking  */
	    BSP_LED_Toggle(LED2);
	    HAL_Delay(100);
	  }
}
