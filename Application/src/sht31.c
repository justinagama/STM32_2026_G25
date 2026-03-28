/*
 * sht31.c
 *
 *  Created on: Mar 25, 2026
 *      Author: علي
 */
#include "sht31.h"

// Fonction interne pour vérifier le CRC (Polynomial: 0x31)
static uint8_t SHT31_CheckCRC(uint8_t *data, uint8_t len) {
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) crc = (crc << 1) ^ 0x31;
            else crc <<= 1;
        }
    }
    return crc;
}

uint8_t SHT31_Init(SHT31_t *dev, I2C_HandleTypeDef *hi2c) {
    dev->hi2c = hi2c;
    dev->temperature = 0;
    dev->humidity = 0;

    uint8_t cmd[2] = { (SHT31_SOFT_RESET >> 8), (SHT31_SOFT_RESET & 0xFF) };
    return HAL_I2C_Master_Transmit(dev->hi2c, SHT31_ADDR, cmd, 2, 100);
}

uint8_t SHT31_ReadTempHum(SHT31_t *dev) {
    uint8_t cmd[2] = { (SHT31_MEAS_HIGH_REP >> 8), (SHT31_MEAS_HIGH_REP & 0xFF) };
    uint8_t data[6];

    // Envoyer la commande de mesure
    if (HAL_I2C_Master_Transmit(dev->hi2c, SHT31_ADDR, cmd, 2, 100) != HAL_OK) return 0;

    HAL_Delay(20); // Temps de conversion

    // Lire 6 octets
    if (HAL_I2C_Master_Receive(dev->hi2c, SHT31_ADDR, data, 6, 100) != HAL_OK) return 0;

    // Vérification CRC pour la température et l'humidité
    if (SHT31_CheckCRC(&data[0], 2) != data[2]) return 0;
    if (SHT31_CheckCRC(&data[3], 2) != data[5]) return 0;

    // Conversion en valeurs physiques
    uint16_t rawTemp = (data[0] << 8) | data[1];
    uint16_t rawHum = (data[3] << 8) | data[4];

    dev->temperature = -45.0f + 175.0f * ((float)rawTemp / 65535.0f);
    dev->humidity = 100.0f * ((float)rawHum / 65535.0f);

    return 1; // Succès
}

