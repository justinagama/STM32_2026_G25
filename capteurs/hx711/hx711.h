/*
 * hx711.h
 *
 *  Created on: Feb 13, 2026
 *      Author: علي
 */
#ifndef __HX711_H__
#define __HX711_H__

#include "stm32l4xx_hal.h" // Inclusion de la HAL pour manipuler les GPIO

/**
 * @brief Structure de données pour l'instance HX711
 * Permet de gérer plusieurs capteurs si besoin.
 */
typedef struct {
    GPIO_TypeDef* port_dat;  // Port GPIO pour la broche DATA (ex: GPIOA)
    uint16_t pin_dat;       // Numéro de broche DATA (ex: GPIO_PIN_0)
    GPIO_TypeDef* port_clk;  // Port GPIO pour la broche CLOCK (ex: GPIOA)
    uint16_t pin_clk;       // Numéro de broche CLOCK (ex: GPIO_PIN_1)
    long offset;            // Valeur de la tare (poids à vide)
    float coefficient;      // Facteur de conversion (poids connu / valeur brute)
} HX711_t;

/* Fonctions de configuration */
void HX711_Init(HX711_t *hx, GPIO_TypeDef* port_dat, uint16_t pin_dat, GPIO_TypeDef* port_clk, uint16_t pin_clk);
void HX711_Tare(HX711_t *hx, uint8_t samples);

/* Fonctions de lecture */
long HX711_Read(HX711_t *hx);
long HX711_ReadAverage(HX711_t *hx, uint8_t samples);
float HX711_GetWeight(HX711_t *hx, uint8_t samples);


#endif /* INC_HX711_H_ */
