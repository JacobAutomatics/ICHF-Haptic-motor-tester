/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "AS5600.h"
#include "Butterworth_II_tustin.h"

#include "mpu6050.h" // tylko na potrzeby ramki komunikacyjnej

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define _18B_ANGLE_SCALLING_FACTOR 45.50556f // +/-180deg -> 0 - 16383
#define _18B_MOTOR_SCALLING_FACTOR 182.03333f // 0 - 90Hz -> 0 - 16383
#define _18B_ACCEL_SCALLING_FACTOR 2047.75f // +/- 8G -> 0 - 16383
#define _18B_OFFSET 8192
#define _JOYSTICK_SCALLING_FACTOR 0.9961f // 0 - 255 -> 0 - 254
#define _ROUNDING_CORRECTION 0.5f

void coding18b(MPU6050_t sensorStruct, uint8_t buttonsA,
		uint8_t buttonsB, float motorL, float motorR, uint8_t joystickX,
		uint8_t joystickY, uint8_t output[18]) {
	uint16_t uangleX;
	uint16_t uangleY;
	uint16_t umotorL;
	uint16_t umotorR;
	uint16_t uaccelX;
	uint16_t uaccelY;
	uint16_t uaccelZ;
	uint8_t ujoystickX;
	uint8_t ujoystickY;

	uangleX = (uint16_t)((sensorStruct.KalmanAngleX * _18B_ANGLE_SCALLING_FACTOR) + _18B_OFFSET + _ROUNDING_CORRECTION);
	if (uangleX > 16383) uangleX = 16383;
	uangleY = (uint16_t)((sensorStruct.KalmanAngleY * _18B_ANGLE_SCALLING_FACTOR) + _18B_OFFSET + _ROUNDING_CORRECTION);
	if (uangleY > 16383) uangleY = 16383;

	umotorL = (uint16_t)((motorL * _18B_MOTOR_SCALLING_FACTOR) + _ROUNDING_CORRECTION);
	if (umotorL > 16383) umotorL = 16383;
	umotorR = (uint16_t)((motorR * _18B_MOTOR_SCALLING_FACTOR) + _ROUNDING_CORRECTION);
	if (umotorR > 16383) umotorR = 16383;

	uaccelX = (uint16_t)((sensorStruct.Ax * _18B_ACCEL_SCALLING_FACTOR) + _18B_OFFSET + _ROUNDING_CORRECTION);
	if (uaccelX > 16383) uaccelX = 16383;
	uaccelY = (uint16_t)((sensorStruct.Ay * _18B_ACCEL_SCALLING_FACTOR) + _18B_OFFSET + _ROUNDING_CORRECTION);
	if (uaccelY > 16383) uaccelY = 16383;
	uaccelZ = (uint16_t)((sensorStruct.Az * _18B_ACCEL_SCALLING_FACTOR) + _18B_OFFSET + _ROUNDING_CORRECTION);
	if (uaccelZ > 16383) uaccelZ = 16383;

	ujoystickX = (uint8_t)(joystickX * _JOYSTICK_SCALLING_FACTOR);
	if (ujoystickX > 254) ujoystickX = 254;
	ujoystickY = (uint8_t)(joystickY * _JOYSTICK_SCALLING_FACTOR);
	if (ujoystickY > 254) ujoystickY = 254;

	output[0] = (uangleX >> 6) & 0b11111110;
	output[1] = uangleX & 0b01111111;
	output[2] = (uangleY >> 6) & 0b11111110;
	output[3] = uangleY & 0b01111111;
	output[4] = (umotorL >> 6) & 0b11111110;
	output[5] = umotorL & 0b01111111;
	output[6] = (umotorR >> 6) & 0b11111110;
	output[7] = umotorR & 0b01111111;
	output[8] = buttonsA & 0b11111011;
	output[9] = (buttonsB & 0b01111011) | (buttonsA & 0b100);
	output[10] = (uaccelX >> 6) & 0b11111110;
	output[11] = uaccelX & 0b01111111;
	output[12] = (uaccelY >> 6) & 0b11111110;
	output[13] = uaccelY & 0b01111111;
	output[14] = (uaccelZ >> 6) & 0b11111110;
	output[15] = uaccelZ & 0b01111111;
	output[16] = ujoystickX;
	output[17] = ujoystickY;
}

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

MPU6050_t MPU6050; // tylko na potrzeby ramki komunikacyjnej

typedef enum {
	STOP, // oczekiwanie na komunikat z komputera
	WORK
} USER_FLAG_state; // konkretny stan całej maszyny

typedef enum {
	FREE, // pomiar wolny, timer może go aktywować
	PROCESSING // pomiar w toku (aktywacja timera nic nie da)
} USER_FLAG_measurement; // flaga na potrzebę pomiaru


uint8_t input; // Bajt wejsciowy. Komunikacja z komputerem.
uint8_t inputsTab[3] = {0};
uint8_t inputNumber = 0;

receivedData USER_receivedDataStruct; // RAMKA DO ODBIERANIA DANYCH Z KOMPUTERA

AS5600_t AS5600;
Butterwoth_II_tustin_t Butterworth_II_tustin;
Butterwoth_II_tustin_t Butterworth_II_tustin2;

float speedForFilter1; // input na filtry
float speedForFilter2;

float speedFromFilter1;	// output z filtrów
float speedFromFilter2;

// zmienne potrzebne do ramki komunikacyjnej
float motorR;
float angleX;
float angleY;
float accelX;
float accelY;
float accelZ;
uint8_t buttonsA;
uint8_t buttonsB;
uint8_t joystickX;
uint8_t joystickY;
uint8_t outputTab[18];

uint8_t endByte = 0b11111111;
uint8_t exampleByte = 0b00000001;


USER_FLAG_measurement USER_FLAG_measurementVar;
USER_FLAG_state USER_FLAG_stateVar;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void setVibratingMotors() {
	if (USER_receivedDataStruct.mode > 0) {
		if (USER_receivedDataStruct.leftMotor > 0) {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, USER_receivedDataStruct.leftMotor);
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
		}
		else {
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
		}
	}
	else {
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	}
}

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
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

  AS5600_Init(&hi2c2, &AS5600);

  Butterworth_II_tustin_Init(&Butterworth_II_tustin, 14, 5);
  Butterworth_II_tustin_Init(&Butterworth_II_tustin2, 30, 5);

  HAL_UART_Receive_IT(&huart3, &input, 1);

  USER_FLAG_stateVar = STOP;
  USER_FLAG_measurementVar = FREE;


  // ************************************ DEBUUUUUG ***************************************
//  USER_FLAG_stateVar = WORK; // ODPALAAAAAM POMIAAAAR
//  HAL_TIM_Base_Start_IT(&htim2);
//
//  inputsTab[0] = 1;
//  inputsTab[1] = 0;
//  inputsTab[2] = 61;
//  USER_receivedDataStruct.mode = inputsTab[0];
//  USER_receivedDataStruct.leftMotor = inputsTab[1];
//  USER_receivedDataStruct.rightMotor = inputsTab[2];
//  setVibratingMotors();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  if (USER_FLAG_stateVar == WORK && USER_FLAG_measurementVar == PROCESSING) { // pojedynczy pomiar

		  // pomiar

		  AS5600_ReadAngle(&hi2c2, &AS5600);
	  	  AS5600_UpdateTurns(&AS5600);
	  	  AS5600_CalculateSpeed(&hi2c2, &AS5600);

	  	  // filtracja

	  	  speedForFilter1 = AS5600.speed;

	  	  // FILTROWANIE -> ODKOMENTOWAC
//	  	  Butterworth_II_tustin_Update(&Butterworth_II_tustin, speedForFilter1);
//	  	  speedFromFilter1 = Butterworth_II_tustin.y;

	  	  // BEZ FILTRACJI -> ODKOMENTOWAC
	  	  speedFromFilter1 = AS5600.speed;


	  	  motorR = 0;
		  buttonsA = 0;
		  buttonsB = 0;
		  joystickX = 0;
		  joystickY = 0;
		  angleX = 0;
		  angleY = 0;
		  accelX = 0;
		  accelY = 0;
		  accelZ = 0;
		  coding18b(MPU6050, buttonsA, buttonsB, speedFromFilter1, motorR, joystickX, joystickY, outputTab);

		  HAL_UART_Transmit(&huart3, outputTab, 18, 2);
	  	  HAL_UART_Transmit(&huart3, &endByte, 1, 1);

	  	  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, SET);

	  	  USER_FLAG_measurementVar = FREE; // Kasowanie flagi
	  }
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)	// ODBIERANIE RAMKI DANYCH
{
	if (input != 0b11111111 && inputNumber <= 2) {
		inputsTab[inputNumber] = input;
		inputNumber++;
	}
	else {
		if (input == 0b11111111 && inputNumber == 3) {
			for (uint8_t i = 0; i <= 2; i++) {
				if (inputsTab[i] != 0b11111111 && i == 2) {
					// POPRAWNY KOMUNIKAT ODEBRANY! DALSZE AKCJE >>>

					if(inputsTab[0] <= 2) {
						USER_FLAG_stateVar = WORK; // ODPALAAAAAM POMIAAAAR
						HAL_TIM_Base_Start_IT(&htim2);

						USER_receivedDataStruct.mode = inputsTab[0];
						USER_receivedDataStruct.leftMotor = inputsTab[1];
						USER_receivedDataStruct.rightMotor = inputsTab[2];
						//manageSystem();
						setVibratingMotors();
					}
					else break;
				}
			}
			inputNumber = 0;
		}
		else {
			inputNumber = 0;
		}
	}
	HAL_UART_Receive_IT(&huart3, &input, 1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2) {
			USER_FLAG_measurementVar = PROCESSING; // start pojedynczego pomiaru AS5600
	}

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
