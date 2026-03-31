/*
 * ultra.c
 *
 *  Created on: Feb 12, 2026
 *      Author: علي
 */

#include "ultra.h"

static TIM_HandleTypeDef *uTimer;

void Ultra_Init(TIM_HandleTypeDef *htim) {
    uTimer = htim;
    HAL_TIM_Base_Start(uTimer);
}

static void Delay_us(uint32_t us) {
    __HAL_TIM_SET_COUNTER(uTimer, 0);
    while (__HAL_TIM_GET_COUNTER(uTimer) < us);
}

static void Set_Pin_Output(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ULTRA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(ULTRA_PORT, &GPIO_InitStruct);
}

static void Set_Pin_Input(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ULTRA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ULTRA_PORT, &GPIO_InitStruct);
}

float Ultra_ReadDistance(void) {
    uint32_t startTime = 0, stopTime = 0;
    uint32_t timeout = 100000; // Sécurité anti-blocage

    // 1. Envoi du signal Trigger
    Set_Pin_Output();
    HAL_GPIO_WritePin(ULTRA_PORT, ULTRA_PIN, GPIO_PIN_RESET);
    Delay_us(2);
    HAL_GPIO_WritePin(ULTRA_PORT, ULTRA_PIN, GPIO_PIN_SET);
    Delay_us(10);
    HAL_GPIO_WritePin(ULTRA_PORT, ULTRA_PIN, GPIO_PIN_RESET);

    // 2. Passage en mode réception
    Set_Pin_Input();

    // 3. Attente du début de l'écho (HIGH)
    while (HAL_GPIO_ReadPin(ULTRA_PORT, ULTRA_PIN) == GPIO_PIN_RESET && timeout--) ;
    startTime = __HAL_TIM_GET_COUNTER(uTimer);

    // 4. Attente de la fin de l'écho (LOW)
    timeout = 100000;
    while (HAL_GPIO_ReadPin(ULTRA_PORT, ULTRA_PIN) == GPIO_PIN_SET && timeout--) ;
    stopTime = __HAL_TIM_GET_COUNTER(uTimer);

    // 5. Calcul : Distance = (Temps * Vitesse du son) / 2
    // Vitesse du son = 0.0343 cm/us
    return (float)(stopTime - startTime) * 0.0343f / 2.0f;
}
