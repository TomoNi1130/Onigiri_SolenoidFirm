/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define BASE_CANID 128
#define SOLENOID_PINS (A_0_Pin | A_1_Pin | A_2_Pin | A_3_Pin | A_4_Pin | A_5_Pin | A_6_Pin | A_7_Pin)
#define ERROR_BLINK_COUNT 3U
#define ERROR_BLINK_DELAY_CYCLES 800000UL

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config (void);
static void MX_GPIO_Init (void);
static void MX_CAN_Init (void);
static void MX_USART1_UART_Init (void);

/* USER CODE BEGIN PFP */

void CAN_Start (CAN_HandleTypeDef *hcan);
uint8_t CAN_Send (CAN_HandleTypeDef *hcan, uint32_t id, uint8_t *data, uint8_t len);
static void Error_SetSafeOutputs (void);
static void Error_BlinkAndReset (void);
static void Error_Delay (void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static uint8_t input_state = 0b00000000;
static uint8_t canid = BASE_CANID;

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main (void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init ();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config ();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init ();
  MX_CAN_Init ();
  MX_USART1_UART_Init ();
  /* USER CODE BEGIN 2 */

  CAN_Start (&hcan);

  canid = BASE_CANID + !HAL_GPIO_ReadPin (GPIOB, IDset_Pin);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // ソレノイドの制御
    uint8_t sol_state = input_state;
    for (int i = 0; i < 8; i++) {
      HAL_GPIO_WritePin (GPIOA, GPIO_PIN_0 << i, (sol_state >> i) & 1);
    }

    // CANのステータスLEDの制御
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config (void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler ();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler ();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig (&PeriphClkInit) != HAL_OK) {
    Error_Handler ();
  }
}

/**
 * @brief CAN Initialization Function
 * @param None
 * @retval None
 */
static void MX_CAN_Init (void) {
  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN;
  hcan.Init.Prescaler = 8;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_8TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = ENABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = ENABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init (&hcan) != HAL_OK) {
    Error_Handler ();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init (void) {
  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init (&huart1) != HAL_OK) {
    Error_Handler ();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init (void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE ();
  __HAL_RCC_GPIOB_CLK_ENABLE ();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin (GPIOA, A_0_Pin | A_1_Pin | A_2_Pin | A_3_Pin | A_4_Pin | A_5_Pin | A_6_Pin | A_7_Pin | CAN_State_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : A_0_Pin A_1_Pin A_2_Pin A_3_Pin A_4_Pin A_5_Pin A_6_Pin A_7_Pin CAN_State_Pin */
  GPIO_InitStruct.Pin = A_0_Pin | A_1_Pin | A_2_Pin | A_3_Pin | A_4_Pin | A_5_Pin | A_6_Pin | A_7_Pin | CAN_State_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init (GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : IDset_Pin */
  GPIO_InitStruct.Pin = IDset_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init (IDset_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void CAN_Start (CAN_HandleTypeDef *hcan) {
  CAN_FilterTypeDef can_filter = {0};
  can_filter.FilterMode = CAN_FILTERMODE_IDMASK;
  can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
  can_filter.FilterIdHigh = 0x0000;
  can_filter.FilterIdLow = 0x0000;
  can_filter.FilterMaskIdHigh = 0x0000;
  can_filter.FilterMaskIdLow = 0x0000;
  can_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  can_filter.FilterActivation = ENABLE;
  can_filter.SlaveStartFilterBank = 14;

  if (hcan->Instance == CAN) {
    can_filter.FilterBank = 0;
  } else {
    can_filter.FilterBank = 14;
  }

  if (HAL_CAN_ConfigFilter (hcan, &can_filter) != HAL_OK) {
    Error_Handler ();
  }

  if (HAL_CAN_Start (hcan) != HAL_OK) {
    Error_Handler ();
  }

  if (HAL_CAN_ActivateNotification (hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
    Error_Handler ();
  }
}

uint8_t CAN_Send (CAN_HandleTypeDef *hcan, uint32_t id, uint8_t *data, uint8_t len) {
  CAN_TxHeaderTypeDef tx_header;
  uint32_t tx_mailbox;

  tx_header.StdId = id & 0x7FFU; /* 11bit標準ID */
  tx_header.ExtId = 0;
  tx_header.IDE = CAN_ID_STD;
  tx_header.RTR = CAN_RTR_DATA;
  tx_header.DLC = len;
  tx_header.TransmitGlobalTime = DISABLE;

  if (HAL_CAN_AddTxMessage (hcan, &tx_header, data, &tx_mailbox) != HAL_OK) {
    return 1;
  }
  return 0;
}

void HAL_CAN_RxFifo0MsgPendingCallback (CAN_HandleTypeDef *hcan) {  // CANのコールバック
  CAN_RxHeaderTypeDef rx_header = {0};
  uint8_t rx_data[8] = {0};

  if (hcan->Instance != CAN) return;
  if (HAL_CAN_GetRxMessage (hcan, CAN_RX_FIFO0, &rx_header, rx_data) != HAL_OK) return;

  if (rx_header.StdId != canid) return;
  input_state = rx_data[0];
}

int __io_putchar (int ch) {
  uint8_t c = (uint8_t)ch;
  (void)HAL_UART_Transmit (&huart1, &c, 1, 10);
  return ch;
}

static void Error_SetSafeOutputs (void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE ();

  GPIO_InitStruct.Pin = SOLENOID_PINS | CAN_State_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init (GPIOA, &GPIO_InitStruct);

  HAL_GPIO_WritePin (GPIOA, SOLENOID_PINS | CAN_State_Pin, GPIO_PIN_RESET);
}

static void Error_BlinkAndReset (void) {
  for (uint32_t i = 0; i < ERROR_BLINK_COUNT; i++) {
    HAL_GPIO_WritePin (CAN_State_GPIO_Port, CAN_State_Pin, GPIO_PIN_SET);
    Error_Delay ();
    HAL_GPIO_WritePin (CAN_State_GPIO_Port, CAN_State_Pin, GPIO_PIN_RESET);
    Error_Delay ();
  }

  NVIC_SystemReset ();
}

static void Error_Delay (void) {
  for (volatile uint32_t i = 0; i < ERROR_BLINK_DELAY_CYCLES; i++) {
    __NOP ();
  }
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler (void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq ();
  Error_SetSafeOutputs ();
  Error_BlinkAndReset ();

  while (1) {
    NVIC_SystemReset ();
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed (uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
