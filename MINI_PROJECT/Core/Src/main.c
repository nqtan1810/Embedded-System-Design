/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "cmsis_os.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "usbd_desc.h"
#include "stm32f429i_discovery_sdram.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_gyroscope.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi5;

osThreadId Task01Handle;
osThreadId Task02Handle;
osMailQId myMailQueueHandle;
/* USER CODE BEGIN PV */

	typedef struct
	{
	  int16_t 	point;
	  int16_t 	a;
	  int16_t 	b;
	  float 	R;
	  int8_t 	game_over_flag;
	} QUEUE_DATA_STRUCT;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI5_Init(void);
void StartTask01(void const * argument);
void StartTask02(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI5_Init();
  /* USER CODE BEGIN 2 */
  //MX_USB_DEVICE_Init();
  BSP_GYRO_Init();
  BSP_SDRAM_Init();
  BSP_LCD_Init();//init LCD
  //set the layer buffer address into SDRAM
  BSP_LCD_LayerDefaultInit(1, SDRAM_DEVICE_ADDR);
  BSP_LCD_SelectLayer(1);//select on which layer we write
  BSP_LCD_DisplayOn();//turn on LCD
  BSP_LCD_Clear(LCD_COLOR_BLUE);//clear the LCD on blue color
  BSP_LCD_SetBackColor(LCD_COLOR_BLUE);//set text background color
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);//set text color


  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of myMailQueue */
  osMailQDef(myMailQueue, 16, QUEUE_DATA_STRUCT);
  myMailQueueHandle = osMailCreate(osMailQ(myMailQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Task01 */
  osThreadDef(Task01, StartTask01, osPriorityNormal, 0, 512);
  Task01Handle = osThreadCreate(osThread(Task01), NULL);

  /* definition and creation of Task02 */
  osThreadDef(Task02, StartTask02, osPriorityNormal, 0, 512);
  Task02Handle = osThreadCreate(osThread(Task02), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PG13 PG14 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartTask01 */
/**
  * @brief  Function implementing the Task01 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTask01 */
void StartTask01(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 5 */

  int16_t 	point;
  int16_t 	a;
  int16_t 	b;
  float 	R;
  int8_t 	game_over_flag;
  uint8_t 	msg_buf[14] = "Hello Player\n\r";
  uint8_t 	msg_point[3];
  uint8_t 	msg_score[7] = "Score: ";
  uint8_t 	msg_game_over[10] = "GAME OVER!";
  uint8_t 	TxBuffer[6];
  int16_t	total_point = 0;
  uint8_t 	perfect_hit[10] = "PERFECT!";

  //osSignalWait(0x1, 10000);
  osSignalSet(Task02Handle, 0x1);
  CDC_Transmit_HS(msg_buf, strlen((char *)msg_buf));

  /* Infinite loop */
  for(;;)
  {
	osEvent event = osMailGet(myMailQueueHandle, osWaitForever); //Get mail
	QUEUE_DATA_STRUCT *queue_rx = event.value.p;
	point 			= queue_rx->point;
	a				= queue_rx->a;
	b				= queue_rx->b;
	R				= queue_rx->R;
	game_over_flag 	= queue_rx->game_over_flag;

	if(point > 1){
		BSP_LCD_DisplayStringAt(10, 300, perfect_hit, CENTER_MODE);	// display perfect hit
	}

	osMailFree(myMailQueueHandle, queue_rx); //Free mail queue memory

	sprintf(TxBuffer, "%.1f", R);
	TxBuffer[4] = '\n';
	TxBuffer[5] = '\r';
	CDC_Transmit_HS(TxBuffer, strlen((char *)TxBuffer));  // send R to Virtual COM Port

	// calculate total point
	total_point += point;

	// display coding here
	sprintf(msg_point, "%d", total_point);
	BSP_LCD_DisplayStringAt(0, 0, msg_score, LEFT_MODE);	// display point
	BSP_LCD_DisplayStringAt(100, 0, msg_point, LEFT_MODE);
	BSP_LCD_FillCircle(a, b, R);	// draw filled circle
	if(game_over_flag){
		BSP_LCD_DisplayStringAt(10, 300, msg_game_over, CENTER_MODE);
	}

	// for debug


    osDelay(15);

    //clear to see what does change in LCD
    BSP_LCD_Clear(LCD_COLOR_BLUE);//clear the LCD on blue color

  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the Task02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */

	float gyro_data[3];
	int16_t 	point = 0;
	int16_t 	a = 120;
    int16_t 	b = 160;
    float 		R = 15;		//  R_MIN <= R <= R_MAX
    int8_t 		game_over_flag = 0;
    float		rate_gyr_x;
    float		rate_gyr_y;
    //float		rate_gyr_z;
    float 		gyro_angle_x = 0;
    float 		gyro_angle_y = 0;
    //float 		gyro_angle_z = 0;
    float 		pre_gyro_angle_x = 0;
	float 		pre_gyro_angle_y = 0;
	//float 		pre_gyro_angle_z = 0;
	float 		delt_angle_x = 0;
	float 		delt_angle_y = 0;
	//float 		delt_angle_z = 0;
    float		loop_period = 0.01;
    float 		k = 1.01;
    float 		R_MAX = 50;
    float 		R_MIN = 15;
    float 		R_HIT_LEVEL01 = 20;
    float 		R_HIT_LEVEL02 = 25;
    float 		R_HIT_LEVEL03 = 30;
    float		v_x = 40;
    float 		v_y = 40;
    int16_t THRESHOLD_HIGH = 90;
    int16_t THRESHOLD_LOW = -90;
	float 		WIDTH = 239;
	float 		HEIGHT = 319;
	//uint8_t 	perfect_hit[10] = "PERFECT!";

	osSignalWait(0x1, osWaitForever); //Wait for signal.

  for(;;)
  {
	BSP_GYRO_GetXYZ(gyro_data);		// get raw data from gyroscope

	// xu li chuyen doi gyroscope raw data sang angle
	rate_gyr_x = (float) gyro_data[0] * L3GD20_SENSITIVITY_500DPS * 0.001;
	rate_gyr_y = (float) gyro_data[1] * L3GD20_SENSITIVITY_500DPS * 0.001;
	//rate_gyr_z = (float) gyro_data[2] * L3GD20_SENSITIVITY_500DPS * 0.001;

	gyro_angle_x += rate_gyr_x * loop_period;
	gyro_angle_y += rate_gyr_y * loop_period;
	//gyro_angle_z += rate_gyr_z * loop_period;

	gyro_angle_x = (int)gyro_angle_x % 180;
	gyro_angle_y = (int)gyro_angle_y % 180;
	//gyro_angle_z = (int)gyro_angle_z % 180;

	delt_angle_x = gyro_angle_x - pre_gyro_angle_x;
	delt_angle_y = gyro_angle_y - pre_gyro_angle_y;
	//delt_angle_z = gyro_angle_z - pre_gyro_angle_z;

	pre_gyro_angle_x = gyro_angle_x;
	pre_gyro_angle_y = gyro_angle_y;
	//pre_gyro_angle_z = gyro_angle_z;

	// xu li game tai day: xac dinh point, a, b, R(dua vao he so k va gyroscope data), game_over_flag, blink led
	if ((delt_angle_x <= THRESHOLD_LOW || delt_angle_x >= THRESHOLD_HIGH
		 || delt_angle_y >= THRESHOLD_HIGH || delt_angle_y <= THRESHOLD_LOW) && game_over_flag == 0 && k < 1){
		//BSP_LCD_FillTriangle(120, 90, 150, 320, 290, 290);	// duoi
		if(R < R_HIT_LEVEL01){
			k = 1.02;
			point = 2;
		}
		else if(R < R_HIT_LEVEL02){
			k = 1.015;
			point = 1;
		}
		else if(R < R_HIT_LEVEL03){
			k = 1.01;
			point = 1;
		}

		if(k > 1){
			//point = 1;
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
		}

		////////////////////////////////////////// tinh toan huong bong khi cham vot
		if (delt_angle_x >= THRESHOLD_HIGH){	// cham mat tren
			if (v_y < 0){
				v_y = -v_y  ;
			}
		}

		if (delt_angle_x <= THRESHOLD_LOW){		// cham mat duoi
			if (v_y > 0){
				v_y = -v_y;
			}
		}

		if (delt_angle_y >= THRESHOLD_HIGH){	// cham mat trai
			if(v_x < 0){
				v_x = -v_x;
			}
		}

		if (delt_angle_y <= THRESHOLD_LOW){		// cham mat phai
			if(v_x > 0){
				v_x = -v_x;
			}
		}

		delt_angle_x = 0;
		delt_angle_y = 0;
	}

	if(a <= R || a >= WIDTH-R){		// cham LEFT-RIGHT
		v_x = -v_x;
	}

	if(b <= R || b >= HEIGHT-R){		// cham TOP-BOTTOM
		v_y = -v_y;
	}

	a += (int) (v_x) * 0.025;	// tinh toa do (a~x, b~y)
	b += (int) (v_y) * 0.025;

	if(a <= R){		// cham LEFT-RIGHT
		a = R+1;
	}

	if(b <= R){		// cham TOP-BOTTOM
		b = R+1;
	}

	if(a >= WIDTH-R){		// cham LEFT-RIGHT
		a = WIDTH-R;
	}

	if(b >= HEIGHT-R){		// cham TOP-BOTTOM
		b = HEIGHT-R;
	}

	// kiem tra game over va khoan gioi han cua R
	if(R < R_MIN){
		game_over_flag = 1;
		k = 1;
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
	}

	if(R >= R_MAX){
		k = 0.99;
	}

	R *= k;

	QUEUE_DATA_STRUCT *queue_tx;		// declare object to send to queue
	do
	{
		queue_tx = (QUEUE_DATA_STRUCT *)osMailAlloc(myMailQueueHandle, osWaitForever); //Allocate memory slot
	} while (NULL == queue_tx);

	queue_tx->point 		 = point;	// Packaging data
	queue_tx->a 			 = a;
	queue_tx->b 			 = b;
	queue_tx->R 			 = R;
	queue_tx->game_over_flag = game_over_flag;

	osMailPut(myMailQueueHandle, queue_tx); //Put data into mail queue.

	// for debug
	point = 0;
    osDelay(1);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
  }
  /* USER CODE END StartTask02 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
