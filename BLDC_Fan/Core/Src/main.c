/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "mcu_api.h"
#include "wifi.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//最小速度时，输出的PWM占空比
#define MIN_SPEED	10

//最大速度时，输出的PWM占空比
//最大速度输出的PWM占空比应该为100，我这里因为演示设置较低
#define MAX_SPEED	35

//关机输出占空比
#define	OFF_SPEED	5

//改变风扇转向输出的PWM值，在BLDC开发板中输出PWM在1%~2.5%之间改变电机转向
#define DIRECTION_CHANGE_PWM 15

//睡眠模式下，风速改变时间
#define SLEEP_TIME 700
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//电扇状态结构体，全局变量
fan_status_t gs_fan_status = {
		.OnOff 						= FALSE,
		.e_fan_mode 			= nature,
		.speed 						= 10,
		.e_fan_direction 	= forward
	};

//上一次风扇速度
unsigned long last_fan_speed = 0;

//风速sleep模式下，改变风速计数值和风速改变标志
unsigned long fen_count = SLEEP_TIME;
_Bool sleep_speed_flag = TRUE;	
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
	
void setup(void);
void connect_tuya(void);
void set_fan_speed(unsigned long speed);
void check_mode(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GUNC_
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch,FILE *f)
#endif  /* __GUNC_ */

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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	setup();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		wifi_uart_service();
		connect_tuya();
		
		if (gs_fan_status.OnOff == TRUE) { //开机
			check_mode();
		} else {
			set_fan_speed(0);
		}
		
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
PUTCHAR_PROTOTYPE{
	HAL_UART_Transmit(&huart3, (uint8_t*)&ch, 1, 0xFFFF);
	return ch;
}

/**
 * @brief  在进入while循环前，需要进行初始化的
 * @param[in] Null
 * @return Null
 * @note
 */
void setup(void)
{
	//优先输出频率为1000HZ，占空比为5%，使电机处于关机状态
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (OFF_SPEED * 10));
	
	//打开与涂鸦三明治 Wi-Fi MCU 通信板（E3S）通信的UART1接收中断
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	
	//完成wifi协议初始化
	wifi_protocol_init();
}

/**
 * @brief  GPIOC3引脚拉低300ms后，进入SMART模式,LED提示联网状态
 * @param[in] Null
 * @return Null
 * @note
 */
void connect_tuya(void)
{
	//判断PC3是否拉低
	if (HAL_GPIO_ReadPin(WIFI_KEY_GPIO_Port, WIFI_KEY_Pin) == GPIO_PIN_RESET) {
		HAL_Delay(300);
		if (HAL_GPIO_ReadPin(WIFI_KEY_GPIO_Port, WIFI_KEY_Pin) == GPIO_PIN_RESET) {
			mcu_set_wifi_mode(0);
			printf("begin connect wifi\r\n");
		}
	}
	
	//获取当前连接状态，显示LED提示
	switch(mcu_get_wifi_work_state())
	{
				case SMART_CONFIG_STATE:	//SMART配网模式，快闪
						printf("smart config\r\n");
						HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
						HAL_Delay(250);
				break;
				case AP_STATE:	//AP配网模式，慢闪
						printf("AP config\r\n");
						HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
						HAL_Delay(500);
				break;
				case WIFI_NOT_CONNECTED: //常暗
						printf("connect wifi\r\n");
						HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
				break;
				case WIFI_CONNECTED://常亮，连接到WIFI
						printf("connect success\r\n");
				case WIFI_CONN_CLOUD://常亮，连接到WIFI和云平台
						HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
				break;
				default:
						HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
				break;
	}
}


/**
 * @brief  设置目标速度，输出对应PWM
 * @param[in] {speed} 要设置的电机速度值 0-100; 
 * @return Null
 * @note
 */
void set_fan_speed(unsigned long speed)
{
	//输入为0，关闭电机。
	if (speed == 0) {
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (OFF_SPEED * 10));
		last_fan_speed = OFF_SPEED;
		return;
	}
	
	//判断输入值是否超出最大，最小值
	if (speed < MIN_SPEED) {
		//转速低于最小值，转速设置为最小值
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (MIN_SPEED * 10));
		last_fan_speed = MIN_SPEED;
	} else if (speed > MAX_SPEED) {
		//转速高于最大值，转速设置为最大值
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (MAX_SPEED * 10));
		last_fan_speed = MAX_SPEED;
	} else {
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (speed * 10));
		last_fan_speed = speed;
	}
	
	return;
}

/**
 * @brief  检查风扇运行模式
 * @param[in] Null
 * @return Null
 * @note
 */
void check_mode(void) 
{
	//睡眠模式，风一会最小，一会为设置风速
	if (gs_fan_status.e_fan_mode == sleep) {
		if ((sleep_speed_flag == TRUE) && (fen_count >= SLEEP_TIME)) {
			
			set_fan_speed(gs_fan_status.speed);
			
			//计数清零，改变风速
			fen_count = 0;
			sleep_speed_flag = FALSE;
		} else if((sleep_speed_flag == FALSE) && (fen_count >= SLEEP_TIME)) {
			
			set_fan_speed(MIN_SPEED);

			//计数清零，改变风速
			fen_count = 0;
			sleep_speed_flag = TRUE;
		}
		
		fen_count++;
		HAL_Delay(10);
	} else {
		//正常模式，风速为设定速度
		if (last_fan_speed != gs_fan_status.speed) { //速度发生改变
				set_fan_speed(gs_fan_status.speed);
		}
	}
}


/**
 * @brief  改变一次电机方向
 * @param[in] Null
 * @return Null
 * @note
 */
void change_fan_direction(void)
{
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, DIRECTION_CHANGE_PWM);
	HAL_Delay(1500);
	last_fan_speed = 0;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
