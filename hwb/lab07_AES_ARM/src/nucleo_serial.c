// Jiri Bucek 2018, bucekj@fit.cvut.cz
// Using parts from the UART_TwoBoards_ComPolling example, (C) COPYRIGHT 2016 STMicroelectronics

#include "stm32f0xx.h"
#include "stm32f0xx_nucleo.h"
			
#include "nucleo_serial.h"

UART_HandleTypeDef UARTxHandle;
UART_HandleTypeDef *huartx = NULL;

UART_HandleTypeDef * serial_init(void)
{
	HAL_StatusTypeDef res;
	  /*##-1- Configure the UART peripheral ######################################*/
	  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	  /* UART configured as follows:
	      - Word Length = 8 Bits
	      - Stop Bit = One Stop bit
	      - Parity = None
	      - BaudRate = 9600 baud
	      - Hardware flow control disabled (RTS and CTS signals) */
	UARTxHandle.Instance        = USARTx;

	UARTxHandle.Init.BaudRate   = 921600;
	UARTxHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UARTxHandle.Init.StopBits   = UART_STOPBITS_1;
	UARTxHandle.Init.Parity     = UART_PARITY_NONE;
	UARTxHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UARTxHandle.Init.Mode       = UART_MODE_TX_RX;
	UARTxHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	res = HAL_UART_DeInit(&UARTxHandle);
	if(res != HAL_OK) return NULL;
	res = HAL_UART_Init(&UARTxHandle);
	if(res != HAL_OK) return NULL;
	huartx = &UARTxHandle;
	return huartx;
}


/**
  * @brief UART MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTx_TX_GPIO_CLK_ENABLE();
  USARTx_RX_GPIO_CLK_ENABLE();

  /* Enable USARTx clock */
  USARTx_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = USARTx_TX_AF;

  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = USARTx_RX_PIN;
  GPIO_InitStruct.Alternate = USARTx_RX_AF;

  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  USARTx_FORCE_RESET();
  USARTx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);
}





