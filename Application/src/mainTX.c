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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LoRa_E5_P2P.h"
#include "sht31.h"
#include "hx711.h"
#include <lib_lcd.h>
#include "dht11.h"
#include <stdio.h>
#include <string.h>
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

/* USER CODE BEGIN PV */
LCD_RGB_HandleTypeDef lcd;
SHT31_t               sensor;
HX711_t               maBalance;

char msgMBal[10];
char msgTout[10];
char msgHout[10];
char msgTin[10];
char msgHin[10];
char LoRa_msg[100];

LORA_Handle_t  lora;
LORA_Handle_t *lora_handles[] = { &lora };

uint8_t humidity    = 0;
uint8_t temperature = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
void formatTemp(char *msg, float temp);
void formatHumi(char *msg, float humi);
void formatPoids(char *msg, float p);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Configuration UART2 pour printf */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    char          Version_data[256];
    char          ID_data[256];
    LORA_Status_t st_flag;
  /* USER CODE END 1 */

  /* MCU Configuration -------------------------------------------------------*/
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM6_Init();
  MX_UART4_Init();

  /* USER CODE BEGIN 2 */

    /* --- Timer pour Delay_us() --- */
    HAL_TIM_Base_Start(&htim6);

    /* --- Initialisation LCD --- */
    LCD_RGB_Init(&lcd, &hi2c1, 16, 2);
    LCD_Clear(&lcd);
    LCD_SetCouleur(&lcd, COULEUR_BLEU);
    LCD_SetCursor(&lcd, 0, 0);
    LCD_EcrireTexte(&lcd, "Welcome");
    HAL_Delay(2000);
    LCD_Clear(&lcd);

    /*========================================================================*/
    /* --- Initialisation SHT31 --- */
    LCD_SetCouleur(&lcd, COULEUR_BLEU);
    LCD_EcrireTexte(&lcd, "SHT31 Loading...");

    if (SHT31_Init(&sensor, &hi2c1) != HAL_OK)
    {
        LCD_Clear(&lcd);
        LCD_SetCouleur(&lcd, COULEUR_ROUGE);
        LCD_EcrireTexte(&lcd, "SHT31 Error!");
        while (1);
    }
    HAL_Delay(1000);
    LCD_Clear(&lcd);

    /* --- Initialisation HX711 (DT=PB10, SCK=PA8) --- */
    HX711_Init(&maBalance, GPIOA, GPIO_PIN_8, GPIOB, GPIO_PIN_10);
    maBalance.coefficient = 420.5f;
    HX711_Tare(&maBalance, 10);   /* plateau vide obligatoire */

    /*========================================================================*/
    /* --- Initialisation LoRa --- */
    printf("=== LoRa P2P Started ===\r\n");

    if (LORA_Init(&lora, &huart4) != LORA_STATUS_OK)
    {
        printf("Erreur init LoRa\r\n");
        while (1);
    }
    printf("Le Module LoRa est bien initialisé\r\n");

    if (LORA_GetVersion(&lora, Version_data) == LORA_STATUS_OK)
        printf("Version du module : %s\r\n", Version_data);
    else
        printf("Erreur get_version\r\n");

    if (LORA_GetID(&lora, ID_data) == LORA_STATUS_OK)
        printf("ID du module :\r\n%s\r\n", ID_data);
    else
        printf("Erreur get_ID\r\n");

    if (LORA_TestAT(&lora) != LORA_STATUS_OK)
    {
        printf("Module LoRa non ok pour AT\r\n");
        while (1);
    }
    printf("Le Module LoRa TestAT réussi\r\n");

    if (LORA_P2P_SetMode(&lora) != LORA_STATUS_OK)
    {
        printf("Erreur mode P2P\r\n");
        while (1);
    }
    printf("Le choix du Mode : P2P\r\n");

    st_flag = LORA_P2P_Config(&lora);
    if (st_flag != LORA_STATUS_OK)
    {
        printf("Erreur configuration P2P\r\n");
        while (1);
    }
    printf("Configuration P2P terminée\r\n");

  /* USER CODE END 2 */

  /* Infinite loop -----------------------------------------------------------*/
  /* USER CODE BEGIN WHILE */
  while (1)   /* FIX CRITIQUE : boucle principale restaurée */
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
      printf("=========== Lecture DHT11, SHT31, HX711  ============\r\n");
        /*----------------------------------------------------------------------
         * 1. Lecture & affichage SHT31 (capteur extérieur)
         *--------------------------------------------------------------------*/
        if (SHT31_ReadTempHum(&sensor))
        {
            formatTemp(msgTout, sensor.temperature);
            formatHumi(msgHout, sensor.humidity);

            LCD_SetCouleur(&lcd, (sensor.temperature > 30.0f) ? COULEUR_ROUGE : COULEUR_VERT);

            /* Ligne 0 : température extérieure */
            LCD_SetCursor(&lcd, 0, 0);
            LCD_EcrireTexte(&lcd, "Tout:");
            LCD_SetCursor(&lcd, 5, 0);
            LCD_EcrireTexte(&lcd, msgTout);

            /* Ligne 1 : humidité extérieure */
            LCD_SetCursor(&lcd, 0, 1);
            LCD_EcrireTexte(&lcd, "Hout:");
            LCD_SetCursor(&lcd, 5, 1);
            LCD_EcrireTexte(&lcd, msgHout);
        }
        else
        {
            LCD_SetCursor(&lcd, 0, 0);
            LCD_EcrireTexte(&lcd, "SHT31 Err!      ");
        }

        HAL_Delay(3000);
        LCD_Clear(&lcd);

        /*----------------------------------------------------------------------
         * 2. Lecture DHT11 (capteur intérieur)
         *--------------------------------------------------------------------*/

        DHT11_ReadData(&humidity, &temperature);

        formatTemp(msgTin, (float)temperature);
        formatHumi(msgHin, (float)humidity);

        /* LED indicateur */
        if (humidity > 0 || temperature > 0)
            HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        else
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

        /* Ligne 0 : température intérieure */
        LCD_SetCursor(&lcd, 0, 0);
        LCD_EcrireTexte(&lcd, "Tin:");
        LCD_SetCursor(&lcd, 4, 0);
        LCD_EcrireTexte(&lcd, msgTin);

        /* Ligne 1 : humidité intérieure */
        LCD_SetCursor(&lcd, 0, 1);
        LCD_EcrireTexte(&lcd, "Hin:");
        LCD_SetCursor(&lcd, 4, 1);
        LCD_EcrireTexte(&lcd, msgHin);

        HAL_Delay(3000);
        LCD_Clear(&lcd);

        /*----------------------------------------------------------------------
         * 3. Lecture & affichage HX711 (balance)
         *--------------------------------------------------------------------*/
        float poids = HX711_GetWeight(&maBalance, 5);

        LCD_SetCouleur(&lcd, (poids > 500.0f) ? COULEUR_ROUGE : COULEUR_VERT);

        LCD_SetCursor(&lcd, 0, 0);
        LCD_EcrireTexte(&lcd, "Poids actuel:   ");

        formatPoids(msgMBal, poids);
        LCD_SetCursor(&lcd, 0, 1);
        LCD_EcrireTexte(&lcd, msgMBal);

        HAL_Delay(3000);
        LCD_Clear(&lcd);

        /*----------------------------------------------------------------------
         * 4. Envoi LoRa
         *--------------------------------------------------------------------*/
        snprintf(LoRa_msg, sizeof(LoRa_msg),"Tout:%s, Hout:%s, Tin:%s, Hin:%s, P:%s",msgTout, msgHout, msgTin, msgHin, msgMBal);


        printf("\r\n");
        printf("++++++++++++++++++++\r\n");
        printf("+                  +\r\n");
        printf("+      LoRa TX     +\r\n");
        printf("+                  +\r\n");
        printf("++++++++++++++++++++\r\n");
        printf("\r\n");
        printf("\r\n");
        printf("LoRa TX  [%s]\r\n", LoRa_msg);
        printf("\r\n");
        if (LORA_P2P_SendString(&lora, LoRa_msg) == LORA_STATUS_OK)
        {
        	printf("Paquet envoyé\r\n");
        }
        else
        {
        	printf("Erreur envoi\r\n");
        }

        HAL_Delay(5000);

  }   /* fin while(1) */
  /* USER CODE END 3 */

}   /* FIX CRITIQUE : accolade de fermeture de main() restaurée */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
        Error_Handler();

    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = 1;
    RCC_OscInitStruct.PLL.PLLN            = 10;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        Error_Handler();

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                     | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
        Error_Handler();
}

/* USER CODE BEGIN 4 */

void formatTemp(char *msg, float temp)
{
    int t_int = (int)temp;
    int t_dec = (int)((temp - t_int) * 10);
    if (t_dec < 0) t_dec *= -1;
    sprintf(msg, "%d.%d C  ", t_int, t_dec);
}

void formatHumi(char *msg, float humi)
{
    int h_int = (int)humi;
    int h_dec = (int)((humi - h_int) * 10);
    if (h_dec < 0) h_dec *= -1;
    sprintf(msg, "%d.%d %%  ", h_int, h_dec);
}

void formatPoids(char *msg, float p)
{
    char  *signe = (p < 0) ? "-" : " ";
    float  p_abs = (p < 0) ? -p : p;
    int    p_int = (int)p_abs;
    int    p_dec = (int)((p_abs - p_int) * 10);
    sprintf(msg, "%s%d.%d g ", signe, p_int, p_dec);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    __disable_irq();
    while (1) {}
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    printf("Wrong parameters value: file %s on line %lu\r\n", file, (unsigned long)line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
