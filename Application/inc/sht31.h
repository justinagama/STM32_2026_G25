/*
 * sht31.h
 *
 *  Created on: Mar 25, 2026
 *      Author: علي
 */
#ifndef SHT31_H_
#define SHT31_H_

#include "stm32l4xx_hal.h"

// Adresse I2C par défaut (0x44 << 1)
#define SHT31_ADDR 0x88

// Commandes
#define SHT31_MEAS_HIGH_REP 0x2C06
#define SHT31_SOFT_RESET    0x30A2

typedef struct {
    I2C_HandleTypeDef *hi2c;
    float temperature;
    float humidity;
} SHT31_t;

// Prototypes
uint8_t SHT31_Init(SHT31_t *dev, I2C_HandleTypeDef *hi2c);
uint8_t SHT31_ReadTempHum(SHT31_t *dev);

#endif /* SHT31_H_ */
