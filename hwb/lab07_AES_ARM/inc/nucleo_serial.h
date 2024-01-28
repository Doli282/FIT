// Jiri Bucek 2018, bucekj@fit.cvut.cz
// Using parts from the UART_TwoBoards_ComPolling example, (C) COPYRIGHT 2016 STMicroelectronics

#if !defined(NUCLEO_SERIAL_H)
#define NUCLEO_SERIAL_H


#include "stm32f0xx.h"
#include "stm32f0xx_nucleo.h"

extern UART_HandleTypeDef *huartx;

/* User can use this section to tailor USARTx/UARTx instance used and associated
   resources */
/* Definition for USARTx clock resources */
#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_2
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF1_USART2
#define USARTx_RX_PIN                    GPIO_PIN_3
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF1_USART2

/* Size of Trasmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE

void HAL_UART_MspInit(UART_HandleTypeDef *huart);

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart);

UART_HandleTypeDef * serial_init(void);

#endif
