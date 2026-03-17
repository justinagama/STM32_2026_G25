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

  //declaration des variable globaux
  LCD_RGB_HandleTypeDef lcd;
  char msgT[10];
  char msgH[10];
  char LoRa_msg[256];
  LORA_Handle_t lora;
  LORA_Handle_t* lora_handles[] = { &lora };
  uint8_t humidity = 0;
  uint8_t temperature = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void msgT_H(char *msg,uint8_t T_H_int,char Type);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


// configuration pour la communication UART2
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
	char Version_data[256];
	char ID_data[256];
	LORA_P2P_Packet_t packet;
	LORA_Status_t st_flag ;

//	config.frequency   = 868000000;  // en Hz
//	config.sf          = 7;
//	config.bandwidth   = 125000;      // en Hz
//	config.tx_pr       = 12;
//	config.rx_pr       = 15;
//	config.power       = 14;          // dBm
//	config.crc_on      = true;
//	config.iq_inverted = false;
//	config.public_network = false;

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
  MX_TIM6_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */

  // Démarrer le timer pour Delay_us()

 HAL_TIM_Base_Start(&htim6);

 // Initialisation du LCD
 LCD_RGB_Init(&lcd, &hi2c1, 16, 2);
 LCD_Clear(&lcd);
 LCD_SetCouleur(&lcd, COULEUR_BLEU);
 LCD_SetCursor(&lcd, 0, 0);
 LCD_EcrireTexte(&lcd, "Welcome");
 HAL_Delay(2000);
 LCD_Clear(&lcd);

 /*-------------------------------------------------------------------*/

 printf("=== LoRa P2P Started ===\r\n");

    /* Initialisation LoRa */
    if(LORA_Init(&lora, &huart4) != LORA_STATUS_OK)
    {
        printf("Erreur init LoRa\r\n");
        while(1);
    }
    else
    {
       printf("Le Module LoRa est bien initialisé\r\n");
    }


    if(LORA_GetVersion(&lora, Version_data) == LORA_STATUS_OK)
    {
 	   printf("Version du module : %s\r\n", Version_data);
    }
    else
    {
 	   printf("Erreur get_version\r\n");
    }

    if(LORA_GetID(&lora, ID_data) == LORA_STATUS_OK)
    {
 	   printf("ID du module :\r\n%s\r\n", ID_data);
    }
    else
    {
 	   printf("Erreur get_ID\r\n");
    }



    /* Test AT */
    if(LORA_TestAT(&lora) != LORA_STATUS_OK)
    {
       printf("Module LoRa non ok pour AT\r\n");
       while(1);
    }
    printf("Le Module LoRa TestAT réussi \r\n");



    if(LORA_P2P_SetMode(&lora) != LORA_STATUS_OK)
    {
        printf("Erreur mode P2P\r\n");
        while(1);
    }
    else
    {
 	   printf("Le choix du Mode : P2P \r\n");
    }
   // LORA_SendRaw(&lora,CMD_config);

    st_flag = LORA_P2P_Config(&lora);


    if( st_flag != LORA_STATUS_OK)
    {
        printf("Erreur configuration P2P\r\n");
        while(1);
    }
    else
    {
 	   printf("Configuration P2P terminée\r\n");
    }



     /* Démarrage de la réception continue */

     if(LORA_P2P_StartRX(&lora) != LORA_STATUS_OK)
     {
         printf("Erreur start RX\r\n");
         while(1);
     }
     else
     {
 	    printf("Démarrage de la transmision : RX\r\n");
     }




      HAL_Delay(1000);
	  printf("\r\n");
	  printf("++++++++++++++++++++\r\n");
	  printf("+                  +\r\n");
	  printf("+      LoRa RX     +\r\n");
	  printf("+                  +\r\n");
	  printf("++++++++++++++++++++\r\n");
	  printf("\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

	st_flag= LORA_P2P_Available(&lora);
	//printf("Flag_available : %d\r\n", st_flag);
	if(st_flag)
	{
	     st_flag = LORA_P2P_Read(&lora,&packet);

	  	      //printf("Flag : %d\r\n", st_flag);

	  	      if (st_flag != LORA_STATUS_OK)
	  	      {
	  	          HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	  	          //printf("Erreur de lecture des Trames LoRa\r\n");

	  	      }
	  	      else
	  	      {
	  	          HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
	  	          printf("\r\n");
	  	          printf("=================== RX ========================\r\n");
	  	          printf("Lecture des Trames reussies\r\n");

	  	          printf("Trame : ");
	  	          for(int i = 0; i < packet.length; i++)
	  	          {
	  	              printf("%c", packet.payload[i]);
	  	          }
	  	          printf("\r\n");

	  	          printf("RSSI : %d\r\n", packet.rssi);
	  	          printf("SNR  : %d\r\n", packet.snr);
	  	          memset(lora.rx_buffer,0,LORA_RX_BUFFER_SIZE);
	  	          LORA_P2P_StartRX(&lora);
	  	          printf("=================== RX ========================\r\n");
	  	          printf("\r\n");
	  	      }
	  }
	  else
	  {
		  printf("...");
	  }





	  	    // lecture des donnees capteur voir si je peux mettre une condition sur lecture capteur
//
//	  	    printf("=========== Lecture donnée capteur de temperature et humidité ============\r\n");
//	  	    DHT11_ReadData(&humidity, &temperature);
//
//	  	    // conversion des donnees capteur en ASCII
//	  	    msgT_H(msgT,temperature,'T'); // la fonction permet de convertire la valeur de la temperature en ASCII
//	  		  msgT_H(msgH,humidity,'H'); // la fonction permet de convertire la valeur de l'humidité en ASCII
//	  	    if (humidity > 0 || temperature > 0)
//	  		  {
//	  		     HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
//	  		  }
//	  		  else
//	  		  {
//	  		     HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
//	  		  }
//
//
//	  	      HAL_Delay(1000);   // Lecture toutes les 2 secondes
//	  		  LCD_SetCursor(&lcd, 0, 0);
//	  		  LCD_EcrireTexte(&lcd, "Temp:");
//	  		  LCD_SetCursor(&lcd, 6, 0);
//	  		  LCD_EcrireTexte(&lcd, msgT);
//	  		  LCD_SetCursor(&lcd, 0, 1);
//	  		  LCD_EcrireTexte(&lcd, "Hum:");
//	  		  LCD_SetCursor(&lcd, 6, 1);
//	  		  LCD_EcrireTexte(&lcd, msgH);
//
//	  	    snprintf(LoRa_msg, 100, "Temperature : %s || Humidity : %s",msgT, msgH);
//	  	    printf("LoRa TX  [%s]\r\n",LoRa_msg);
//	  	    /* Envoi d’un paquet */
//	  	    printf("\r\n");
//	  	    printf("++++++++++++++++++++\r\n");
//	  	    printf("+                  +\r\n");
//	  	    printf("+      LoRa TX     +\r\n");
//	  	    printf("+                  +\r\n");
//	  	    printf("++++++++++++++++++++\r\n");
//
//
//	  	   	if(LORA_P2P_SendString(&lora, LoRa_msg) == LORA_STATUS_OK)
//	  	    {
//	  	      printf("Paquet envoyé\r\n");
//	  	    }
//	  	    else
//	  	   	{
//	  	    	printf("Erreur envoi\r\n");
//	  	    }
//	  	   	HAL_Delay(1000);


//    /* USER CODE BEGIN 3 */
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
void msgT_H(char *msg,uint8_t T_H_int,char Type)
{
    if(T_H_int < 0)
    {
        msg[0]='-';
        T_H_int = -T_H_int;
    }
    else
    {
        msg[0]='+';
    }

    msg[1]=(T_H_int/10)+48;
    msg[2]=(T_H_int%10)+48;
    //msg[3]=',';
    //msg[4]=(T_H_dec/10)+48;
    //msg[5]=(T_H_dec%10)+48;

    switch(Type)
    {
        case 'T':
        case 't':
            msg[3]='C';
            break;

        case 'H':
        case 'h':
            msg[3]='%';
            break;

        default:
            msg[3]='?';   // sécurité
            break;
    }

//    msg[4]='\r';
//    msg[5]='\n';
//    msg[6]='\0';
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
