/*
 * lora_e5.c
 *
 *  Created on: Mar 3, 2026
 *      Author: agama
 */


#include "LoRa_E5_WAN.h"
#include <stdio.h>
#include <string.h>



/* ============================================================
   ==================== INITIALISATION =======================
   ============================================================ */

void LoRaE5_init(LoRaE5Class *obj, UART_HandleTypeDef *huart)
{
    if (!obj || !huart) return;

    obj->huart = huart;

    obj->lowpower_auto = false;
    obj->adaptative_DR = false;

    obj->bitRate = 9600;
    obj->txHead_time = 0.0f;
    obj->freq_band = 868.0f; // par défaut
    obj->txPower = 14;        // par défaut

    obj->SF_last = SF7;
    obj->BW_last = BW125;
    obj->FREQBAND_last = EU868;

    memset(obj->recv_buf, 0, sizeof(obj->recv_buf));
    memset(obj->cmd, 0, sizeof(obj->cmd));
    memset(obj->cmd_resp_ack, 0, sizeof(obj->cmd_resp_ack));
}

/* ============================================================
   ==================== UART READ BLOCKING ===================
   ============================================================ */

unsigned int LoRaE5_readBuffer(LoRaE5Class *obj, char* buffer, unsigned int length, unsigned int timeout_ms)
{
    if (!obj || !buffer || length == 0) return 0;

    uint32_t startTick = HAL_GetTick();
    uint32_t index = 0;

    while ((HAL_GetTick() - startTick) < timeout_ms && index < length)
    {
        uint8_t byte;
        if (HAL_UART_Receive(obj->huart, &byte, 1, 1) == HAL_OK)
        {
            buffer[index++] = byte;
        }
    }

    return index;
}

/* ============================================================
   ==================== SEND AT AND CHECK ====================
   ============================================================ */

unsigned int LoRaE5_at_send_check_response(LoRaE5Class *obj, char *p_cmd, char *p_ack, unsigned int timeout_ms, char *p_response)
{
    if (!obj || !p_cmd || !p_ack) return 0;

    memset(obj->recv_buf, 0, sizeof(obj->recv_buf));

    // Envoi de la commande AT
    if(HAL_UART_Transmit(obj->huart, (uint8_t*)p_cmd, strlen(p_cmd), timeout_ms) != HAL_OK)
        return 0;

    // Lecture de la réponse
    unsigned int len = LoRaE5_readBuffer(obj, obj->recv_buf, sizeof(obj->recv_buf)-1, timeout_ms);
    obj->recv_buf[len] = '\0';

    // Copier la réponse si demandé
    if(p_response)
        strncpy(p_response, obj->recv_buf, len+1);

    // Vérification de la présence de l'ACK
    if(strstr(obj->recv_buf, p_ack) != NULL)
        return 1; // succès

    return 0; // échec
}



/* ============================================================
   =================== IDENTIFIANTS ==========================
   ============================================================ */

unsigned int LoRaE5_getId(LoRaE5Class *obj, char *buffer, _deviceID id, unsigned int timeout)
{
    if (!obj || !buffer) return 0;

    char cmd[32] = {0};
    switch(id)
    {
        case DevAddr: sprintf(cmd, "AT+DevAddr\r\n"); break;
        case DevEui:  sprintf(cmd, "AT+DevEui\r\n"); break;
        case AppEui:  sprintf(cmd, "AT+AppEui\r\n"); break;
        default: return 0;
    }

    if (LoRaE5_at_send_check_response(obj, cmd, "OK", timeout, buffer))
        return strlen(buffer);
    else
        return 0;
}

unsigned int LoRaE5_setId(LoRaE5Class *obj, char *DevAddr, char *DevEUI, char *AppEUI)
{
    if (!obj) return 0;
    int res = 0;

    if(DevAddr)
    {
        sprintf(obj->cmd, "AT+DevAddr=%s\r\n", DevAddr);
        res += LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL);
    }
    if(DevEUI)
    {
        sprintf(obj->cmd, "AT+DevEui=%s\r\n", DevEUI);
        res += LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL);
    }
    if(AppEUI)
    {
        sprintf(obj->cmd, "AT+AppEui=%s\r\n", AppEUI);
        res += LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL);
    }
    return res;
}

unsigned int LoRaE5_setKey(LoRaE5Class *obj, char *NwkSKey, char *AppSKey, char *AppKey)
{
    if (!obj) return 0;
    int res = 0;

    if(NwkSKey)
    {
        sprintf(obj->cmd, "AT+NwkSKey=%s\r\n", NwkSKey);
        res += LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL);
    }
    if(AppSKey)
    {
        sprintf(obj->cmd, "AT+AppSKey=%s\r\n", AppSKey);
        res += LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL);
    }
    if(AppKey)
    {
        sprintf(obj->cmd, "AT+AppKey=%s\r\n", AppKey);
        res += LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL);
    }
    return res;
}

/* ============================================================
   ======================= RADIO =============================
   ============================================================ */

unsigned int LoRaE5_setFrequencyBand(LoRaE5Class *obj, _physical_type_t physicalType)
{
    if (!obj) return 0;

    const char* band_str[] = {"EU868","US915","AU915","AS923","CN470","IN865","RU864"};
    sprintf(obj->cmd, "AT+FREQBAND=%s\r\n", band_str[physicalType]);

    if (LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL))
    {
        obj->FREQBAND_last = physicalType;
        return 1;
    }
    return 0;
}

unsigned int LoRaE5_setDataRate(LoRaE5Class *obj, _data_rate_t dataRate, _physical_type_t physicalType)
{
    if (!obj) return 0;
    sprintf(obj->cmd, "AT+DR=%u\r\n", dataRate);
    return LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL);
}

unsigned int LoRaE5_setSpreadFactor(LoRaE5Class *obj, _spreading_factor_t SF, _band_width_t BW, _physical_type_t physicalType)
{
    if (!obj) return 0;
    sprintf(obj->cmd, "AT+SF=%u BW=%u\r\n", SF, BW);
    if (LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL))
    {
        obj->SF_last = SF;
        obj->BW_last = BW;
        return 1;
    }
    return 0;
}

unsigned int LoRaE5_setAdaptiveDataRate(LoRaE5Class *obj, bool command)
{
    if (!obj) return 0;
    sprintf(obj->cmd, "AT+ADR=%u\r\n", command ? 1 : 0);
    if (LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL))
    {
        obj->adaptative_DR = command;
        return 1;
    }
    return 0;
}

unsigned int LoRaE5_setPower(LoRaE5Class *obj, short power)
{
    if (!obj) return 0;
    sprintf(obj->cmd, "AT+POWER=%d\r\n", power);
    if (LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL))
    {
        obj->txPower = power;
        return 1;
    }
    return 0;
}

unsigned int LoRaE5_setPort(LoRaE5Class *obj, unsigned char port)
{
    if (!obj) return 0;
    sprintf(obj->cmd, "AT+PORT=%u\r\n", port);
    return LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL);
}

/* ============================================================
   ==================== TRANSMISSION =========================
   ============================================================ */

unsigned int LoRaE5_transferPacket(LoRaE5Class *obj, unsigned char *buffer, unsigned char length, unsigned int timeout)
{
    if (!obj || !buffer || length == 0) return 0;

    sprintf(obj->cmd, "AT+SEND=%u,", length);
    HAL_UART_Transmit(obj->huart, (uint8_t*)obj->cmd, strlen(obj->cmd), timeout);

    HAL_UART_Transmit(obj->huart, buffer, length, timeout);

    return LoRaE5_at_send_check_response(obj, "\r\n", "OK", timeout, NULL);
}

unsigned int LoRaE5_transferPacketWithConfirmed(LoRaE5Class *obj,  unsigned char *buffer, unsigned char length, unsigned int timeout)
{
    if (!obj || !buffer || length == 0) return 0;

    sprintf(obj->cmd, "AT+CSEND=%u,", length);
    HAL_UART_Transmit(obj->huart, (uint8_t*)obj->cmd, strlen(obj->cmd), timeout);

    HAL_UART_Transmit(obj->huart, buffer, length, timeout);

    return LoRaE5_at_send_check_response(obj, "\r\n", "OK", timeout, NULL);
}

/* ============================================================
   ====================== JOIN ==============================
   ============================================================ */

unsigned int LoRaE5_setDeviceMode(LoRaE5Class *obj, _device_mode_t mode)
{
    if (!obj) return 0;
    sprintf(obj->cmd, "AT+MODE=%u\r\n", mode);
    return LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL);
}

unsigned int LoRaE5_setOTAAJoin(LoRaE5Class *obj, _otaa_join_cmd_t command, unsigned int timeout)
{
    if (!obj) return 0;
    sprintf(obj->cmd, "AT+JOIN=%u\r\n", command);
    return LoRaE5_at_send_check_response(obj, obj->cmd, "OK", timeout, NULL);
}

/* ============================================================
   ==================== LOW POWER ===========================
   ============================================================ */

unsigned int LoRaE5_setDeviceLowPower(LoRaE5Class *obj, unsigned int time_to_wakeup)
{
    if (!obj) return 0;
    sprintf(obj->cmd, "AT+SLEEP=%u\r\n", time_to_wakeup);
    return LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL);
}

unsigned int LoRaE5_setDeviceWakeUp(LoRaE5Class *obj)
{
    if (!obj) return 0;
    sprintf(obj->cmd, "AT+WAKE\r\n");
    return LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL);
}

unsigned int LoRaE5_setDeviceReset(LoRaE5Class *obj)
{
    if (!obj) return 0;
    sprintf(obj->cmd, "AT+RESET\r\n");
    return LoRaE5_at_send_check_response(obj, obj->cmd, "OK", 2000, NULL);
}
