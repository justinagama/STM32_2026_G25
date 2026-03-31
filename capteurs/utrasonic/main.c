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
#include "ultra.h"   // Inclusion de la bibliothèque personnalisée
#include "lib_lcd.h" // inclusion de la bibliotèque pour le lcd
#include <stdio.h>   // Pour utiliser sprintf
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
float distance_cm = 0;   // Variable pour stocker la distance
char msg_uart[50];    // Buffer pour l'envoi de texte vers le PC
char msg_lcd[16];
LCD_RGB_HandleTypeDef lcd;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void msgDist(char *msg, float distance); // <-- Ajoute cette ligne ici
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  Ultra_Init(&htim2);
  // 2. Initialisation du LCD (doit être APRES MX_I2C1_Init)
    LCD_RGB_Init(&lcd, &hi2c1, 16, 2);
    LCD_Clear(&lcd);
    LCD_SetCouleur(&lcd, COULEUR_BLEU);
    LCD_SetCursor(&lcd, 0, 0);
    LCD_EcrireTexte(&lcd, "Mesure en cours");

    // 3. Message de démarrage sur le PC

  HAL_Delay(1000);
    LCD_Clear(&lcd);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */
	  // 1. Mesurer la distance
	  /*
	      distance_cm = Ultra_ReadDistance();

	      // 2. Préparer le message pour le terminal série
	      // On transforme le float en chaîne de caractères
	      int len = sprintf(msg_uart, "Distance : %.2f cm\r\n", distance_cm);

	      // 3. Envoyer vers le PC via l'USB (UART2)
	      HAL_UART_Transmit(&huart2, (uint8_t*)msg_uart, len, 100);

	      // 3 bis lcd
	      	LCD_SetRGB(&lcd,10,100,100);
	      	LCD_SetCursor(&lcd,0,0);
	      	LCD_EcrireTexte(&lcd,"distance cm");
	        LCD_SetCursor(&lcd, 6,0);
    		//LCD_EcrireTexte(&lcd,msgT);


	      // 4. Pause de 200ms pour ne pas saturer le capteur
	          HAL_Delay(200);



	  // 1. Mesurer la distance
	      distance_cm = Ultra_ReadDistance();

	      // 2. Affichage Terminal Série (PC)
	      int len = sprintf(msg_uart, "Distance : %.2f cm\r\n", distance_cm);
	      HAL_UART_Transmit(&huart2, (uint8_t*)msg_uart, len, 100);

	      // 3. Affichage sur le LCD
	      // On change la couleur en fonction de la distance (optionnel mais sympa)
	      if (distance_cm < 10) {
	          LCD_SetCouleur(&lcd, COULEUR_ROUGE); // Alerte : trop proche
	      } else {
	          LCD_SetCouleur(&lcd, COULEUR_VERT);
	      }

	      // Préparer la ligne du LCD
	      sprintf(msg_lcd, "Dist: %.1f cm  ", distance_cm); // Les espaces à la fin effacent les anciens chiffres

	      LCD_SetCursor(&lcd, 0, 0);
	      LCD_EcrireTexte(&lcd, "GROVE ULTRASONIC");
	      LCD_SetCursor(&lcd, 0, 1);
	      LCD_EcrireTexte(&lcd, msg_lcd);

	      // 4. Pause
	          HAL_Delay(200);


	        // 1. Mesurer la distance
	        distance_cm = Ultra_ReadDistance();

	        // 2. Affichage Terminal Série (PC)
	        // Note : Si cela affiche des espaces vides, voir l'étape 3 ci-dessous
	        int len = sprintf(msg_uart, "Distance : %.2f cm\r\n", distance_cm);
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg_uart, len, 100);

	        // 3. Affichage sur le LCD
	        if (distance_cm < 1.0)
	            {
	                // CAS D'ERREUR : Objet trop proche ou absence d'écho
	                LCD_SetCouleur(&lcd, COULEUR_ROUGE);
	                LCD_EcrireTexte(&lcd, "ERREUR DISTANCE "); // Les espaces à la fin effacent l'ancien texte
	            }
	            else{
	        if (distance_cm < 5) {
	            LCD_SetCouleur(&lcd, COULEUR_ROUGE);
	        } else {
	            LCD_SetCouleur(&lcd, COULEUR_VERT);
	        }
	            }
	        // UTILISATION DE TA FONCTION MANUELLE ICI
	        msgDist(msg_lcd, distance_cm);

	        LCD_SetCursor(&lcd, 0, 0);
	        LCD_EcrireTexte(&lcd, "GROVE ULTRASONIC");
	        LCD_SetCursor(&lcd, 0, 1);
	        LCD_EcrireTexte(&lcd, msg_lcd); // Affiche "025.4 cm" par exemple

	        HAL_Delay(200);
	    }
*/
	  // 1. Mesure de la distance
	      distance_cm = Ultra_ReadDistance();

	      // 2. Envoi vers le PC pour debug
	      int len = sprintf(msg_uart, "Distance : %.2f cm\r\n", distance_cm);
	      HAL_UART_Transmit(&huart2, (uint8_t*)msg_uart, len, 100);

	      // 3. Logique d'affichage LCD avec gestion d'erreur
	      LCD_SetCursor(&lcd, 0, 1); // On se place sur la 2ème ligne

	      if (distance_cm < 2.5)
	      {
	          // CAS D'ERREUR : Objet trop proche ou absence d'écho
	          LCD_SetCouleur(&lcd, COULEUR_ROUGE);
	          LCD_EcrireTexte(&lcd, "ERREUR DISTANCE "); // Les espaces à la fin effacent l'ancien texte
	      }
	      else
	      {
	          // CAS NORMAL : Affichage de la distance
	          if (distance_cm < 10.0) {
	              LCD_SetCouleur(&lcd, COULEUR_ROUGE); // Alerte proximité
	          } else {
	              LCD_SetCouleur(&lcd, COULEUR_VERT);  // OK
	          }

	          // Conversion manuelle via ta fonction
	          msgDist(msg_lcd, distance_cm);
	          LCD_EcrireTexte(&lcd, msg_lcd);

	          //
	          LCD_SetCursor(&lcd, 0, 0);
	          LCD_EcrireTexte(&lcd, "GROVE ULTRASONIC");
	      }

	      HAL_Delay(200);
	    }
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */


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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
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

/* USER CODE BEGIN 4 */
void msgDist(char *msg, float distance)
{
    int d_int = (int)distance;
    int d_dec = (int)((distance - d_int) * 10);

    // Calcul propre des chiffres
             // Centaines
    msg[0] = ((d_int / 10) % 10) + '0';    // Dizaines
    msg[1] = (d_int % 10) + '0';           // Unités
    msg[2] = '.';
    msg[3] = (d_dec % 10) + '0';           // Dixièmes
    msg[4] = ' ';
    msg[5] = 'c';
    msg[6] = 'm';
    msg[7] = '\0';                         // Fin de chaîne
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
