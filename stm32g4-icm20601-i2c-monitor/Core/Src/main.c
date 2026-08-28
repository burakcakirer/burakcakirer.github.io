/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Hem ICM-20601 hem de SH1107 OLED Birlikte Kararlı Sürüm
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "icm20601.h" // Sensör kütüphanemiz
#include "SH1107.h"   // OLED kütüphanemiz
#include <stdio.h>    // sprintf için
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
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef hlpuart1;

/* USER CODE BEGIN PV */
ICM20601_Data_t imu_data;       // Sensörden gelen ham veriler
ICM20601_Data_t filtered_data;  // Derleyicinin aradığı, filtrelenmiş temiz veri yapısı
uint8_t init_status = 0;
char txt_buffer[32];

// Hareketli Ortalama Filtresi (Moving Average) Hafıza Dizileri


//#define FILTER_SIZE  10    // ESKI FILTER SIZE

#define FILTER_SIZE  30


int16_t ax_buffer[FILTER_SIZE] = {0};
int16_t ay_buffer[FILTER_SIZE] = {0};
int16_t az_buffer[FILTER_SIZE] = {0};

int16_t gx_buffer[FILTER_SIZE] = {0};
int16_t gy_buffer[FILTER_SIZE] = {0};
int16_t gz_buffer[FILTER_SIZE] = {0};

uint8_t filter_index = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */

void Apply_Moving_Average_Filter(ICM20601_Data_t *raw, ICM20601_Data_t *filtered);



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
  MX_LPUART1_UART_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  // 1. OLED ekranın dâhili şarj pompalarının dolması için 500ms kesinlikle şart!
  HAL_Delay(500);

  // 2. Önce OLED ekranı güvenle ilklendiriyoruz
  SH1107_Init();
  SH1107_ClearBuffer();
  SH1107_Update();

  // 3. I2C hattının kendine gelmesi ve durulması için kısa bir es
  HAL_Delay(50);

  // 4. �?imdi ICM-20601 sensörümüzü kararlı hat üzerinden uyandırıyoruz
  init_status = ICM20601_Init(&hi2c1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
      if (init_status == 1) {
          // Sensörden ham verileri oku
          ICM20601_Read_All(&hi2c1, &imu_data);

          // Okunan ham verileri filtreye sok ve filtered_data yapısını güncelle
          Apply_Moving_Average_Filter(&imu_data, &filtered_data);
      }

      // Ekran buffer'ını dâhili RAM'de temizle
      SH1107_ClearBuffer();

      // =================================================================
      // 1. ÜST BA�?LIK VE TASARIM
      // =================================================================
      SH1107_DrawString(22, 4, "=== IMU MONITOR ===", 1);
      SH1107_DrawLine(0, 15, 128, 15, 1);

      // =================================================================
      // 2. IVMEÖLÇER (ACCELEROMETER) BÖLÜMÜ - FİLTRELİ
      // =================================================================
      SH1107_DrawString(5, 20, "ACCELEROMETER", 1);

      sprintf(txt_buffer, "X: %d", filtered_data.accel_x);
      SH1107_DrawString(15, 32, txt_buffer, 1);

      sprintf(txt_buffer, "Y: %d", filtered_data.accel_y);
      SH1107_DrawString(15, 42, txt_buffer, 1);

      sprintf(txt_buffer, "Z: %d", filtered_data.accel_z);
      SH1107_DrawString(15, 52, txt_buffer, 1);

      // Orta Kesim Ayrım Çizgisi
      SH1107_DrawLine(0, 64, 128, 64, 1);

      // =================================================================
      // 3. JİROSKOP (GYROSCOPE) BÖLÜMÜ - FİLTRELİ
      // =================================================================
      SH1107_DrawString(5, 68, "GYROSCOPE", 1);

      sprintf(txt_buffer, "X: %d", filtered_data.gyro_x);
      SH1107_DrawString(15, 80, txt_buffer, 1);

      sprintf(txt_buffer, "Y: %d", filtered_data.gyro_y);
      SH1107_DrawString(15, 90, txt_buffer, 1);

      sprintf(txt_buffer, "Z: %d", filtered_data.gyro_z);
      SH1107_DrawString(15, 100, txt_buffer, 1);

      // Alt Ayrım Çizgisi
      SH1107_DrawLine(0, 112, 128, 112, 1);

      // =================================================================
      // 4. TEMPERATURE (SICAKLIK) BÖLÜMÜ
      // =================================================================
      int32_t temp_int = (int32_t)filtered_data.temperature;
      int32_t temp_frac = (int32_t)((filtered_data.temperature - (float)temp_int) * 100.0f);
      if(temp_frac < 0) temp_frac = -temp_frac;

      sprintf(txt_buffer, "Temperature: %ld.%02ld C", temp_int, temp_frac);
      SH1107_DrawString(5, 117, txt_buffer, 1);

      // =================================================================
      // 5. EKRANI GÜNCELLE VE TETİKLE
      // =================================================================
      SH1107_Update();

      // Durum LED'ini kırpıştır ve bekle


//      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
//      HAL_Delay(100);



      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
          HAL_Delay(250);




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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */


//static void MX_I2C1_Init(void)
//{
//
//  /* USER CODE BEGIN I2C1_Init 0 */
//
//  /* USER CODE END I2C1_Init 0 */
//
//  /* USER CODE BEGIN I2C1_Init 1 */
//
//  /* USER CODE END I2C1_Init 1 */
//  hi2c1.Instance = I2C1;
//  hi2c1.Init.Timing = 0x30A0A7FB;
//  hi2c1.Init.OwnAddress1 = 0;
//  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
//  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
//  hi2c1.Init.OwnAddress2 = 0;
//  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
//  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
//  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
//  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure Analogue filter
//  */
//  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure Digital filter
//  */
//  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN I2C1_Init 2 */
//
//  /* USER CODE END I2C1_Init 2 */
//
//}



static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x30A0A7FB;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  /* USER CODE BEGIN I2C1_Init 2 */
  // 100 kHz Standard Mode için tam %50 Duty Cycle zamanlama sabiti
  hi2c1.Init.Timing = 0x30A09393;
  /* USER CODE END I2C1_Init 2 */

  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }

}




/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */




//static void MX_I2C2_Init(void)
//{
//
//  /* USER CODE BEGIN I2C2_Init 0 */
//
//  /* USER CODE END I2C2_Init 0 */
//
//  /* USER CODE BEGIN I2C2_Init 1 */
//
//  /* USER CODE END I2C2_Init 1 */
//  hi2c2.Instance = I2C2;
//  hi2c2.Init.Timing = 0x30A0A7FB;
//  hi2c2.Init.OwnAddress1 = 0;
//  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
//  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
//  hi2c2.Init.OwnAddress2 = 0;
//  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
//  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
//  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
//  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure Analogue filter
//  */
//  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure Digital filter
//  */
//  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN I2C2_Init 2 */
//
//  /* USER CODE END I2C2_Init 2 */
//
//}




static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x30A0A7FB;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  /* USER CODE BEGIN I2C2_Init 2 */
  // İkinci hat için de tam %50 Duty Cycle zamanlama sabiti
  hi2c2.Init.Timing = 0x30A09393;
  /* USER CODE END I2C2_Init 2 */

  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }

}









/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

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
void Apply_Moving_Average_Filter(ICM20601_Data_t *raw, ICM20601_Data_t *filtered) {
    // 1. Yeni ham verileri tampon dizilere yerleştir
    ax_buffer[filter_index] = raw->accel_x;
    ay_buffer[filter_index] = raw->accel_y;
    az_buffer[filter_index] = raw->accel_z;

    gx_buffer[filter_index] = raw->gyro_x;
    gy_buffer[filter_index] = raw->gyro_y;
    gz_buffer[filter_index] = raw->gyro_z;

    // İndeksi ilerlet, dizi sınırına gelince başa dön
    filter_index++;
    if (filter_index >= FILTER_SIZE) {
        filter_index = 0;
    }

    // 2. Sum the configured moving-average window
    int32_t sum_ax = 0, sum_ay = 0, sum_az = 0;
    int32_t sum_gx = 0, sum_gy = 0, sum_gz = 0;

    for (uint8_t i = 0; i < FILTER_SIZE; i++) {
        sum_ax += ax_buffer[i];
        sum_ay += ay_buffer[i];
        sum_az += az_buffer[i];

        sum_gx += gx_buffer[i];
        sum_gy += gy_buffer[i];
        sum_gz += gz_buffer[i];
    }

    // 3. Ortalama değerleri filtrelenmiş struct'a yaz
    filtered->accel_x = (int16_t)(sum_ax / FILTER_SIZE);
    filtered->accel_y = (int16_t)(sum_ay / FILTER_SIZE);
    filtered->accel_z = (int16_t)(sum_az / FILTER_SIZE);

    filtered->gyro_x  = (int16_t)(sum_gx / FILTER_SIZE);
    filtered->gyro_y  = (int16_t)(sum_gy / FILTER_SIZE);
    filtered->gyro_z  = (int16_t)(sum_gz / FILTER_SIZE);

    // Sıcaklık doğrudan aktarılıyor
    filtered->temperature = raw->temperature;
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
