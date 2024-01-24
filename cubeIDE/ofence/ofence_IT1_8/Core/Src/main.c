/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define BUFF_SIZE   (8)
#define CHAR_CR     (0x0d)
#define TRUE        (1)
#define FALSE       (0)

uint8_t IMUrcvBuffer[BUFF_SIZE]; /* 受信バッファ */
uint8_t IMUdata[BUFF_SIZE]; /* 送信バッファ */

uint8_t IRrcvBuffer[BUFF_SIZE]; /* 受信バッファ */
uint8_t IRdata[BUFF_SIZE]; /* 送信バッファ */

uint8_t LinercvBuffer[BUFF_SIZE]; /* 受信バッファ */
uint8_t Linedata[BUFF_SIZE]; /* 送信バッファ */

uint8_t CamrcvBuffer[BUFF_SIZE]; /* 受信バッファ */
uint8_t Camdata[10]; /* 送信バッファ */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == &huart3) {
		memmove(&IMUdata[0], &IMUdata[1], sizeof(IMUdata[0]) * (7 - 0));
		IMUdata[7] = (int) IMUrcvBuffer[0];
		HAL_UART_Receive_IT(&huart3, IMUrcvBuffer, 1);
	}
	if (huart == &huart5) {
		memmove(&Linedata[0], &Linedata[1], sizeof(Linedata[0]) * (7 - 0));
		Linedata[7] = (int) LinercvBuffer[0];
		HAL_UART_Receive_IT(&huart5, LinercvBuffer, 1);
	}
	if (huart == &huart1) {
		memmove(&IRdata[0], &IRdata[1], sizeof(IRdata[0]) * (7 - 0));
		IRdata[7] = (int) IRrcvBuffer[0];
		HAL_UART_Receive_IT(&huart1, IRrcvBuffer, 1);
	}
	if (huart == &huart4) {
		memmove(&Camdata[0], &Camdata[1], sizeof(Camdata[0]) * (9 - 0));
		Camdata[9] = (int) CamrcvBuffer[0];
		HAL_UART_Receive_IT(&huart4, CamrcvBuffer, 1);
	}
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */
	int IMUnumber[2];
	int Gyrodegree = 0;
	int Gyrodegreepast;

	int IRnumber[2];
	int IRorigin;
	int IRAngle;
	int IRAngle_sum;
	int IRAngle_ave = 0;
	int IRAngleall[20] = { 0 };
	int IRdr;
	int IRdrsub;
	int IRdrPast;
	int Angle;

	int Linenumber[2];
	int Lineorigin;
	int LineAngle;
	int LinePower;

	int Camnumber[4] = { 0, 0, 0, 0 };
	int attack_angle = 0;
	int attack_distance;
	int defence_angle;
	int defence_distance;

	int mode;
	int count;
	char str[5];
	float Kp = 1.25; //3.06 2.26   //PID設??  ?  ?
	float Kd = 0.2; //0.58
	float Ki = 0.000000; //5.03
	int Gyro_power = 0;
	int GyroE;
	int GyroE_1;
	int gyrosum;
	float gyroP;
	float gyroI;
	float gyroD;

	double Line_rate[4];
	double V[4]; //??  ?  ? ??
	double kesu[4][3] = { 1, 1, 0.00, 1, -1, 0.00, -1, -1, 0.00, -1, 1, 0.00, };
	double yoso[3];
	float pi = 3.14; //3.141592
	float rad;
	double Vx, Vy, L;
	double Vxline, Vyline, Lline;
	int derectionpower1, derectionpower2, derectionpower3, derectionpower4;
	int Allp1;
	int Allp2;
	int Allp3;
	int Allp4;
	int allpower;

	int ball_radius = 4;
	int machine_radis = 11;
	int plus = 5;
	float balldr;
	float AB;
	int BC = ball_radius + machine_radis + plus;
	float Angle_pls;

//	float a = 0.8;
//	float b = 0.004;
//	int c = 180;
//	float d = 0.0040;
	float a = 0.9;
	float b = 0.007;
	int c = 190;
	float d = 0.01;
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
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_UART4_Init();
	MX_UART5_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */
	void getIMU() {
		for (int i = 5; i >= 0; i--) {
			if (IMUdata[i] == 72) {
				IMUnumber[0] = IMUdata[i + 1];
				IMUnumber[1] = IMUdata[i + 2];
				break;
			}
		}
		Gyrodegree = IMUnumber[1] * 256 + IMUnumber[0];
		if (360 < Gyrodegree || 0 > Gyrodegree)
			Gyrodegree = Gyrodegreepast;
		Gyrodegreepast = Gyrodegree;
		if (Gyrodegree > 180) {
			Gyrodegree = Gyrodegree - 360;
		}
	}
	void getIR() {
		for (int i = 5; i >= 0; i--) {
			if (IRdata[i] == 72) {
				IRnumber[0] = IRdata[i + 1];
				IRnumber[1] = IRdata[i + 2];
				break;
			}
		}
		IRorigin = IRnumber[1] * 256 + IRnumber[0];
		IRAngle = IRorigin * 0.01;
		IRdr = IRorigin % 100;
		IRdr = abs(IRdr);

		if (IRAngle >= 470) {
			IRAngle = IRAngle - 655;
		}
		IRAngle = IRAngle * -1;
		if (IRAngle_ave > IRAngle) {
			IRAngle_ave -= 2;
		} else if (IRAngle_ave < IRAngle) {
			IRAngle_ave += 2;
		}

	}
	int wrap_around(void) {
		if (abs(IRAngle_ave) < 35) {
			Angle = IRAngle_ave;
		} else {
			balldr = IRdr * -1 + 85;
			AB = machine_radis + balldr;
			Angle_pls = atan(BC / AB);
			Angle_pls = Angle_pls * 57.2958;
			if (IRAngle < 0) {
				Angle = IRAngle - Angle_pls;
			} else {
				Angle = IRAngle + Angle_pls;
			}
		}

		if (Angle > 180) {
			Angle -= 360;
		} else if (Angle < -180) {
			Angle += 360;
		}
		Angle = Angle;
		return Angle;
	}
	void getLine() {
		for (int i = 5; i >= 0; i--) {
			if (Linedata[i] == 255) {
				Linenumber[0] = Linedata[i + 1];
				Linenumber[1] = Linedata[i + 2];
				break;
			}
		}
		Lineorigin = Linenumber[1] * 256 + Linenumber[0];
		LineAngle = Lineorigin * 0.01;
		LinePower = Lineorigin % 100;
		LinePower = abs(LinePower);
		if (LineAngle >= 470) {
			LineAngle = LineAngle - 650;
		}
	}
	void Improveattitude(int Angle) {
		getIMU();
		Gyro_power = 0;
		GyroE_1 = GyroE;
		GyroE = Angle - Gyrodegree;
		gyrosum += GyroE;
		gyroP = Kp * GyroE;
		gyroI = Ki * gyrosum;
		gyroD = Kd * (GyroE - GyroE_1);
		Gyro_power = gyroP + gyroI + gyroD;
		GyroE = Angle - Gyrodegree;
		Gyro_power = GyroE;
	}
	void Improveattitude_target(int Angle, int basic) {
		getIMU();
		Gyro_power = 0;
		GyroE_1 = GyroE;
		GyroE = Angle - basic;
		gyrosum += GyroE;
		gyroP = Kp * GyroE;
		gyroI = Ki * gyrosum;
		gyroD = Kd * (GyroE - GyroE_1);
		Gyro_power = gyroP + gyroI + gyroD;
	}
	void getcam() {
		for (int i = 5; i >= 0; i--) {
			if (Camdata[i] == 255) {
				Camnumber[0] = Camdata[i + 1];
				Camnumber[1] = Camdata[i + 2];
				Camnumber[2] = Camdata[i + 3];
				Camnumber[3] = Camdata[i + 4];
				break;
			}
		}
		attack_angle = Camnumber[0];
		attack_distance = Camnumber[1];
		defence_angle = Camnumber[2];
		defence_distance = Camnumber[3];
		if ((attack_angle != 200) && (attack_angle > 90)) {
			attack_angle = (attack_angle - 90) * -1;
		} else if (attack_angle == 200) {
			attack_angle = 0;
		}
		if ((defence_angle != 200) && (defence_angle < 90)) {
			defence_angle = (defence_angle + 90) * -1;
		} else if (defence_angle == 200) {
			defence_angle = 0;
		}
		if (attack_distance == 0) {
			attack_distance = 200;
		}
		if (defence_distance == 0) {
			defence_distance = 200;
		}
	}
	void Motor_Speed(int Port, int Power) {
		if (Power > 95) {
			Power = 95;
		} else if (Power < -95) {
			Power = -95;
		}
		Power = Power + 100;
		if (Port == 1) {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, Power);
		} else if (Port == 2) {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, Power);
		} else if (Port == 3) {
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, Power);
		} else if (Port == 4) {
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, Power);
		}
	}
	void LineMove() {
		getLine();
		Line_rate[0] = 0.00;
		Line_rate[1] = 0.00;
		Line_rate[2] = 0.00;
		Line_rate[3] = 0.00;
		rad = LineAngle * 0.017; //  ?/180の計算を1つにまとめた
		Vx = sin(rad);
		Vy = cos(rad);
		L = 0;
		yoso[0] = Vx;
		yoso[1] = Vy;
		yoso[2] = L;
		for (int i = 0; i <= 3; i++) {
			for (int j = 0; j <= 2; j++) {
				Line_rate[i] = Line_rate[i] + (yoso[j] * kesu[i][j]);
			}
		}
	}
	void Angle_Move(int degree, int power) { //進みたい角度とモーターの出
		V[0] = 0.00;
		V[1] = 0.00;
		V[2] = 0.00;
		V[3] = 0.00;
		rad = degree * 0.017;		// ??  ?  ?/180の計算を1つにまとめた
		Vx = sin(rad);
		Vy = cos(rad);
		L = 0.00;
		/*
		 if (LinePower != 0) {
		 rad = LineAngle * 0.017;
		 Vxline = sin(rad);
		 Vyline = cos(rad);
		 Lline = 0.00;
		 Vx = -Vxline;
		 Vy = -Vyline;
		 power = LinePower;
		 }*/
		yoso[0] = Vx;
		yoso[1] = Vy;
		yoso[2] = L;
		for (int i = 0; i <= 3; i++) {
			for (int j = 0; j <= 2; j++) {
				V[i] = V[i] + (yoso[j] * kesu[i][j]);
			}
		}
		derectionpower1 = -V[0] * power - Line_rate[0] * LinePower;
		derectionpower2 = -V[1] * power - Line_rate[1] * LinePower;
		derectionpower3 = -V[2] * power - Line_rate[2] * LinePower;
		derectionpower4 = -V[3] * power - Line_rate[3] * LinePower;
		Allp1 = derectionpower1 + Gyro_power;
		Allp2 = derectionpower2 + Gyro_power;
		Allp3 = derectionpower3 + Gyro_power;
		Allp4 = derectionpower4 + Gyro_power;
		Motor_Speed(1, Allp1);
		Motor_Speed(2, -Allp2);
		Motor_Speed(3, -Allp3);
		Motor_Speed(4, -Allp4);
	}
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); //
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	Motor_Speed(1, 0);
	Motor_Speed(2, 0);
	Motor_Speed(3, 0);
	Motor_Speed(4, 0);
	HAL_UART_Receive_IT(&huart3, &IMUrcvBuffer, 1);
	HAL_UART_Receive_IT(&huart1, &IRrcvBuffer, 1);
	HAL_UART_Receive_IT(&huart5, &LinercvBuffer, 1);
	HAL_UART_Receive_IT(&huart4, &CamrcvBuffer, 1);
	while (HAL_GPIO_ReadPin(ST_SW_GPIO_Port, ST_SW_Pin) == 0) {

	}
	HAL_Delay(200);
	while (1) {
		LineMove();
		if (LinePower != 0) {
			allpower = 0;
			Improveattitude(0);
			Angle_Move(0, 0);
			mode = 0;
		} else {
			allpower += 6; //昔と違って??????��?��??��?��???��?��??��?��????��?��??��?��???��?��??��?��?????��?��??��?��???��?��??��?��????��?��??��?��???��?��??��?��? ??  ??????��?��??��?��???��?��??��?��????��?��??��?��???��?��??��?��?????��?��??��?��???��?��??��?��????��?��??��?��???��?��??��?��? ?すぎるから加速を上げ??????��?��??��?��???��?��??��?��????��?��??��?��???��?��??��?��?????��?��??��?��???��?��??��?��????��?��??��?��???��?��??��?��? ?要がある?  ?
			if (allpower > 60) { //パワーが大きすぎると回り込みがきつくな?  ?(改?  ?次第ではど?  ?とでもな?  ?)
				allpower = 60;
			}
			getIR();
			if (IRdr == 0) {
				Improveattitude(0);
				Angle_Move(180, 10);
				mode = 1;
			} else {
				Improveattitude(0);
				Angle_Move(wrap_around(), allpower);
			}
			sprintf(str, "%d", IRdr);
			HAL_UART_Transmit(&huart2, &str, strlen(str), 1000);
			HAL_UART_Transmit(&huart2, (uint8_t*) "\n\r", 2, 1000);
			/* 受信した??????��?��??��?��???��?��??��?��????��?��??��?��???��?��??��?��?????��?��??��?��???��?��??��?��????��?��??��?��???��?��??��?��?容を�???????��?��??��?��???��?��??��?��????��?��??��?��???��?��??��?��?????��?��??��?��???��?��??��?��????��?��??��?��???��?��??��?��信 */
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
	void SystemClock_Config(void) {
		RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
		RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

		/** Configure the main internal regulator output voltage
		 */
		__HAL_RCC_PWR_CLK_ENABLE();
		__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

		/** Initializes the RCC Oscillators according to the specified parameters
		 * in the RCC_OscInitTypeDef structure.
		 */
		RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
		RCC_OscInitStruct.HSIState = RCC_HSI_ON;
		RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
		RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
		RCC_OscInitStruct.PLL.PLLM = 16;
		RCC_OscInitStruct.PLL.PLLN = 336;
		RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
		RCC_OscInitStruct.PLL.PLLQ = 2;
		RCC_OscInitStruct.PLL.PLLR = 2;
		if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
			Error_Handler();
		}

		/** Initializes the CPU, AHB and APB buses clocks
		 */
		RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
				| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
		RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2)
				!= HAL_OK) {
			Error_Handler();
		}
	}

	/**
	 * @brief TIM2 Initialization Function
	 * @param None
	 * @retval None
	 */
	static void MX_TIM2_Init(void) {

		/* USER CODE BEGIN TIM2_Init 0 */

		/* USER CODE END TIM2_Init 0 */

		TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
		TIM_MasterConfigTypeDef sMasterConfig = { 0 };
		TIM_OC_InitTypeDef sConfigOC = { 0 };

		/* USER CODE BEGIN TIM2_Init 1 */

		/* USER CODE END TIM2_Init 1 */
		htim2.Instance = TIM2;
		htim2.Init.Prescaler = 4;
		htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim2.Init.Period = 200;
		htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
			Error_Handler();
		}
		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
			Error_Handler();
		}
		if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
			Error_Handler();
		}
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
				!= HAL_OK) {
			Error_Handler();
		}
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = 0;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3)
				!= HAL_OK) {
			Error_Handler();
		}
		if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4)
				!= HAL_OK) {
			Error_Handler();
		}
		/* USER CODE BEGIN TIM2_Init 2 */

		/* USER CODE END TIM2_Init 2 */
		HAL_TIM_MspPostInit(&htim2);

	}

	/**
	 * @brief TIM3 Initialization Function
	 * @param None
	 * @retval None
	 */
	static void MX_TIM3_Init(void) {

		/* USER CODE BEGIN TIM3_Init 0 */

		/* USER CODE END TIM3_Init 0 */

		TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
		TIM_MasterConfigTypeDef sMasterConfig = { 0 };
		TIM_OC_InitTypeDef sConfigOC = { 0 };

		/* USER CODE BEGIN TIM3_Init 1 */

		/* USER CODE END TIM3_Init 1 */
		htim3.Instance = TIM3;
		htim3.Init.Prescaler = 4;
		htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim3.Init.Period = 200;
		htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
			Error_Handler();
		}
		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
			Error_Handler();
		}
		if (HAL_TIM_PWM_Init(&htim3) != HAL_OK) {
			Error_Handler();
		}
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
				!= HAL_OK) {
			Error_Handler();
		}
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = 0;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3)
				!= HAL_OK) {
			Error_Handler();
		}
		if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4)
				!= HAL_OK) {
			Error_Handler();
		}
		/* USER CODE BEGIN TIM3_Init 2 */

		/* USER CODE END TIM3_Init 2 */
		HAL_TIM_MspPostInit(&htim3);

	}

	/**
	 * @brief UART4 Initialization Function
	 * @param None
	 * @retval None
	 */
	static void MX_UART4_Init(void) {

		/* USER CODE BEGIN UART4_Init 0 */

		/* USER CODE END UART4_Init 0 */

		/* USER CODE BEGIN UART4_Init 1 */

		/* USER CODE END UART4_Init 1 */
		huart4.Instance = UART4;
		huart4.Init.BaudRate = 115200;
		huart4.Init.WordLength = UART_WORDLENGTH_8B;
		huart4.Init.StopBits = UART_STOPBITS_1;
		huart4.Init.Parity = UART_PARITY_NONE;
		huart4.Init.Mode = UART_MODE_TX_RX;
		huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart4.Init.OverSampling = UART_OVERSAMPLING_16;
		if (HAL_UART_Init(&huart4) != HAL_OK) {
			Error_Handler();
		}
		/* USER CODE BEGIN UART4_Init 2 */

		/* USER CODE END UART4_Init 2 */

	}

	/**
	 * @brief UART5 Initialization Function
	 * @param None
	 * @retval None
	 */
	static void MX_UART5_Init(void) {

		/* USER CODE BEGIN UART5_Init 0 */

		/* USER CODE END UART5_Init 0 */

		/* USER CODE BEGIN UART5_Init 1 */

		/* USER CODE END UART5_Init 1 */
		huart5.Instance = UART5;
		huart5.Init.BaudRate = 115200;
		huart5.Init.WordLength = UART_WORDLENGTH_8B;
		huart5.Init.StopBits = UART_STOPBITS_1;
		huart5.Init.Parity = UART_PARITY_NONE;
		huart5.Init.Mode = UART_MODE_TX_RX;
		huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart5.Init.OverSampling = UART_OVERSAMPLING_16;
		if (HAL_UART_Init(&huart5) != HAL_OK) {
			Error_Handler();
		}
		/* USER CODE BEGIN UART5_Init 2 */

		/* USER CODE END UART5_Init 2 */

	}

	/**
	 * @brief USART1 Initialization Function
	 * @param None
	 * @retval None
	 */
	static void MX_USART1_UART_Init(void) {

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
		if (HAL_UART_Init(&huart1) != HAL_OK) {
			Error_Handler();
		}
		/* USER CODE BEGIN USART1_Init 2 */

		/* USER CODE END USART1_Init 2 */

	}

	/**
	 * @brief USART2 Initialization Function
	 * @param None
	 * @retval None
	 */
	static void MX_USART2_UART_Init(void) {

		/* USER CODE BEGIN USART2_Init 0 */

		/* USER CODE END USART2_Init 0 */

		/* USER CODE BEGIN USART2_Init 1 */

		/* USER CODE END USART2_Init 1 */
		huart2.Instance = USART2;
		huart2.Init.BaudRate = 115200;
		huart2.Init.WordLength = UART_WORDLENGTH_8B;
		huart2.Init.StopBits = UART_STOPBITS_1;
		huart2.Init.Parity = UART_PARITY_NONE;
		huart2.Init.Mode = UART_MODE_TX_RX;
		huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart2.Init.OverSampling = UART_OVERSAMPLING_16;
		if (HAL_UART_Init(&huart2) != HAL_OK) {
			Error_Handler();
		}
		/* USER CODE BEGIN USART2_Init 2 */

		/* USER CODE END USART2_Init 2 */

	}

	/**
	 * @brief USART3 Initialization Function
	 * @param None
	 * @retval None
	 */
	static void MX_USART3_UART_Init(void) {

		/* USER CODE BEGIN USART3_Init 0 */

		/* USER CODE END USART3_Init 0 */

		/* USER CODE BEGIN USART3_Init 1 */

		/* USER CODE END USART3_Init 1 */
		huart3.Instance = USART3;
		huart3.Init.BaudRate = 115200;
		huart3.Init.WordLength = UART_WORDLENGTH_8B;
		huart3.Init.StopBits = UART_STOPBITS_1;
		huart3.Init.Parity = UART_PARITY_NONE;
		huart3.Init.Mode = UART_MODE_TX_RX;
		huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart3.Init.OverSampling = UART_OVERSAMPLING_16;
		if (HAL_UART_Init(&huart3) != HAL_OK) {
			Error_Handler();
		}
		/* USER CODE BEGIN USART3_Init 2 */

		/* USER CODE END USART3_Init 2 */

	}

	/**
	 * @brief GPIO Initialization Function
	 * @param None
	 * @retval None
	 */
	static void MX_GPIO_Init(void) {
		GPIO_InitTypeDef GPIO_InitStruct = { 0 };
		/* USER CODE BEGIN MX_GPIO_Init_1 */
		/* USER CODE END MX_GPIO_Init_1 */

		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOH_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();

		/*Configure GPIO pin Output Level */
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

		/*Configure GPIO pin : B1_Pin */
		GPIO_InitStruct.Pin = B1_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

		/*Configure GPIO pin : ST_SW_Pin */
		GPIO_InitStruct.Pin = ST_SW_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(ST_SW_GPIO_Port, &GPIO_InitStruct);

		/*Configure GPIO pin : LD2_Pin */
		GPIO_InitStruct.Pin = LD2_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

		/* USER CODE BEGIN MX_GPIO_Init_2 */
		/* USER CODE END MX_GPIO_Init_2 */
	}

	/* USER CODE BEGIN 4 */

	/* USER CODE END 4 */

	/**
	 * @brief  This function is executed in case of error occurrence.
	 * @retval None
	 */
	void Error_Handler(void) {
		/* USER CODE BEGIN Error_Handler_Debug */
		/* User can add his own implementation to report the HAL error return state */
		__disable_irq();
		while (1) {
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
