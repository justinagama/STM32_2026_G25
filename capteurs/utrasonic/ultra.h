/*
 * ultra.h
 *
 *  Created on: Feb 12, 2026
 *      Author: علي
 */

#ifndef INC_ULTRA_H_
#define INC_ULTRA_H_

#include "stm32l4xx_hal.h"

// Définitions des broches (à adapter selon votre config CubeMX)
#define ULTRA_PORT GPIOA
#define ULTRA_PIN  GPIO_PIN_8
#define ULTRA_TIMER htim2

// Prototypes
void Ultra_Init(TIM_HandleTypeDef *htim);
float Ultra_ReadDistance(void);

#endif /* INC_ULTRA_H_ */


