/*
 * hx711.c
 *
 *  Created on: Feb 13, 2026
 *      Author: علي
 */


#include "hx711.h"

// Fonction interne pour un délai microseconde précis (DWT)
static void delay_us(uint32_t us) {
    uint32_t startTick = DWT->CYCCNT;
    uint32_t delayTicks = us * (SystemCoreClock / 1000000);
    while (DWT->CYCCNT - startTick < delayTicks);
}

void HX711_Init(HX711_t *hx, GPIO_TypeDef* port_dat, uint16_t pin_dat, GPIO_TypeDef* port_clk, uint16_t pin_clk) {
    hx->port_dat = port_dat;
    hx->pin_dat = pin_dat;
    hx->port_clk = port_clk;
    hx->pin_clk = pin_clk;
    hx->offset = 0;
    hx->coefficient = 1.0f;

    // Activation du compteur DWT pour les délais us
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    HAL_GPIO_WritePin(hx->port_clk, hx->pin_clk, GPIO_PIN_RESET);
}

long HX711_Read(HX711_t *hx) {
    long data = 0;

    // Attendre que la donnée soit prête (DT à 0)
    // Timeout de sécurité pour éviter de bloquer le programme
    uint32_t timeout = 1000000;
    while (HAL_GPIO_ReadPin(hx->port_dat, hx->pin_dat) == GPIO_PIN_SET && timeout > 0) {
        timeout--;
    }

    for (int i = 0; i < 24; i++) {
        HAL_GPIO_WritePin(hx->port_clk, hx->pin_clk, GPIO_PIN_SET);
        delay_us(1);
        data = data << 1;
        HAL_GPIO_WritePin(hx->port_clk, hx->pin_clk, GPIO_PIN_RESET);
        delay_us(1);

        if (HAL_GPIO_ReadPin(hx->port_dat, hx->pin_dat)) {
            data++;
        }
    }

    // 25ème impulsion pour fixer le Gain à 128 (Channel A)
    HAL_GPIO_WritePin(hx->port_clk, hx->pin_clk, GPIO_PIN_SET);
    delay_us(1);
    HAL_GPIO_WritePin(hx->port_clk, hx->pin_clk, GPIO_PIN_RESET);
    delay_us(1);

    // Extension du signe pour un entier 24 bits
    if (data & 0x800000) {
        data |= 0xFF000000;
    }

    return data;
}

long HX711_ReadAverage(HX711_t *hx, uint8_t samples) {
    long sum = 0;
    for (uint8_t i = 0; i < samples; i++) {
        sum += HX711_Read(hx);
    }
    return sum / samples;
}

void HX711_Tare(HX711_t *hx, uint8_t samples)
{
    hx->offset = HX711_ReadAverage(hx, samples);
}

float HX711_GetWeight(HX711_t *hx, uint8_t samples)
{
    long raw = HX711_ReadAverage(hx, samples);
    return (float)(raw - hx->offset) / hx->coefficient;
}
