/*
 * lora_e5.h
 *
 *  Created on: Mar 3, 2026
 *      Author: agama
 */

#ifndef LORAE5_H
#define LORAE5_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/* ============================================================
   ENUMS (IDENTIQUES AU SEEED)
   ============================================================ */

typedef enum { DevAddr=0, DevEui, AppEui } _deviceID;

typedef enum {
    BR_9600   = 9600,
    BR_38400  = 38400,
    BR_115200 = 115200
} _baudrate_bps_supported;

typedef enum { CLASS_A = 0, CLASS_C } _class_type_t;

typedef enum {
    EU868,
    US915,
    AU915,
    AS923,
    CN470,
    IN865,
    RU864
} _physical_type_t;

typedef enum { LWABP = 0, LWOTAA, TEST } _device_mode_t;

typedef enum { JOIN = 0, FORCE } _otaa_join_cmd_t;

typedef enum {
    SF12=12, SF11=11, SF10=10,
    SF9=9, SF8=8, SF7=7
} _spreading_factor_t;

typedef enum {
    BW125=125,
    BW250=250,
    BW500=500
} _band_width_t;

typedef enum {
    DR0=0, DR1, DR2, DR3, DR4,
    DR5, DR6, DR7, DR8, DR9,
    DR10, DR11, DR12, DR13, DR14, DR15
} _data_rate_t;

/* ============================================================
   STRUCT PRINCIPALE
   ============================================================ */

typedef struct {

    UART_HandleTypeDef *huart;   // UART STM32 HAL

    bool lowpower_auto;
    bool adaptative_DR;

    unsigned int bitRate;
    float txHead_time;
    float freq_band;
    short txPower;

    _spreading_factor_t SF_last;
    _band_width_t BW_last;
    _physical_type_t FREQBAND_last;

    char recv_buf[512];
    char cmd[556];
    char cmd_resp_ack[64];

} LoRaE5Class;


/* ============================================================
   INITIALISATION
   ============================================================ */

void LoRaE5_init(LoRaE5Class *obj, UART_HandleTypeDef *huart);


/* ============================================================
   UART / COMMANDES AT
   ============================================================ */

unsigned int LoRaE5_readBuffer(LoRaE5Class *obj, char* buffer, unsigned int length, unsigned int timeout_ms);

unsigned int LoRaE5_at_send_check_response(LoRaE5Class *obj, char *p_cmd, char *p_ack, unsigned int timeout_ms, char *p_response);


/* ============================================================
   IDENTIFIANTS
   ============================================================ */

unsigned int LoRaE5_getId(LoRaE5Class *obj, char *buffer, _deviceID id, unsigned int timeout);

unsigned int LoRaE5_setId(LoRaE5Class *obj, char *DevAddr, char *DevEUI, char *AppEUI);

unsigned int LoRaE5_setKey(LoRaE5Class *obj, char *NwkSKey, char *AppSKey, char *AppKey);


/* ============================================================
   CONFIGURATION RADIO
   ============================================================ */

unsigned int LoRaE5_setFrequencyBand(LoRaE5Class *obj, _physical_type_t physicalType);

unsigned int LoRaE5_setDataRate(LoRaE5Class *obj, _data_rate_t dataRate, _physical_type_t physicalType);

unsigned int LoRaE5_setSpreadFactor(LoRaE5Class *obj, _spreading_factor_t SF, _band_width_t BW, _physical_type_t physicalType);

unsigned int LoRaE5_setAdaptiveDataRate(LoRaE5Class *obj, bool command);

unsigned int LoRaE5_setPower(LoRaE5Class *obj, short power);

unsigned int LoRaE5_setPort(LoRaE5Class *obj, unsigned char port);


/* ============================================================
   TRANSMISSION
   ============================================================ */

unsigned int LoRaE5_transferPacket(LoRaE5Class *obj, unsigned char *buffer, unsigned char length, unsigned int timeout);

unsigned int LoRaE5_transferPacketWithConfirmed(LoRaE5Class *obj,unsigned char *buffer, unsigned char length, unsigned int timeout);


/* ============================================================
   JOIN / MODE
   ============================================================ */

unsigned int LoRaE5_setDeviceMode(LoRaE5Class *obj, _device_mode_t mode);

unsigned int LoRaE5_setOTAAJoin(LoRaE5Class *obj,_otaa_join_cmd_t command, unsigned int timeout);


/* ============================================================
   LOW POWER / SYSTEM
   ============================================================ */

unsigned int LoRaE5_setDeviceLowPower(LoRaE5Class *obj, unsigned int time_to_wakeup);

unsigned int LoRaE5_setDeviceWakeUp(LoRaE5Class *obj);

unsigned int LoRaE5_setDeviceReset(LoRaE5Class *obj);


#ifdef __cplusplus
}
#endif

#endif /* LORAE5_H */
