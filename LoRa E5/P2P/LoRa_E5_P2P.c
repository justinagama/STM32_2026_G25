/*
 * LoRa_E5_P2P.c
 *
 *  Created on: Mar 3, 2026
 *      Author: agama
 */

#include "LoRa_E5_P2P.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* ===================== */
/*   FONCTIONS PRIVÉES   */
/* ===================== */

/**
 * @brief Envoie une commande brute sur le module LoRa via UART
 * @param hlora Pointeur vers le handle LoRa
 * @param cmd Chaîne de commande à envoyer
 * @return Statut de l'opération
 */
LORA_Status_t LORA_SendRaw(LORA_Handle_t *hlora, const char *cmd)
{
    if (!hlora || !cmd || !hlora->huart)
    {
        return LORA_STATUS_INVALID_PARAM;
    }

    if (HAL_UART_Transmit(hlora->huart, (uint8_t *)cmd, strlen(cmd), LORA_DEFAULT_TIMEOUT) != HAL_OK)
    {
    	return LORA_STATUS_ERROR;
    }

    return LORA_STATUS_OK;
}

/**
 * @brief Attend la réponse d'une commande LoRa
 * @param hlora Pointeur vers le handle LoRa
 * @param expected Chaîne attendue comme réponse
 * @param timeout Durée maximum d'attente en ms
 * @return Statut de l'opération
 */
static LORA_Status_t LORA_WaitForResponse(LORA_Handle_t *hlora, const char *expected, uint32_t timeout)
{
    if (!hlora || !expected)
        return LORA_STATUS_INVALID_PARAM;

    uint32_t start = HAL_GetTick();
    char local_copy[LORA_RX_BUFFER_SIZE];

    while ((HAL_GetTick() - start) < timeout)
    {
        __disable_irq();
        strncpy(local_copy, hlora->rx_buffer, LORA_RX_BUFFER_SIZE - 1);
        __enable_irq();

        local_copy[LORA_RX_BUFFER_SIZE - 1] = '\0';

        if (strstr(local_copy, expected) != NULL)
        {
        	return LORA_STATUS_OK;
        }

       HAL_Delay(1);
    }

    return LORA_STATUS_TIMEOUT;
}

/**
 * @brief Envoie une commande et attend un ACK
 * @param hlora Pointeur vers le handle LoRa
 * @param cmd Commande à envoyer
 * @param ack Chaîne ACK attendue
 * @param timeout Durée maximum d'attente en ms
 * @return Statut de l'opération
 */
static LORA_Status_t LORA_SendCommand(LORA_Handle_t *hlora, const char *cmd, const char *ack, uint32_t timeout)
{
    if (!hlora || !cmd || !ack)
    {
    	return LORA_STATUS_INVALID_PARAM;
    }

    __disable_irq();
    memset(hlora->rx_buffer, 0, LORA_RX_BUFFER_SIZE);
    hlora->rx_index = 0;
    __enable_irq();

    if (LORA_SendRaw(hlora, cmd) != LORA_STATUS_OK)
    {
    	 return LORA_STATUS_ERROR;
    }

    return LORA_WaitForResponse(hlora, ack, timeout);
}

/*------------------------------------------------------------*/
/*              CALLBACK UART RX IT                          */
/*------------------------------------------------------------*/

/**
 * @brief Callback d'interruption UART pour la réception
 * @param huart Pointeur vers le handle UART concerné
// */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // On récupère le handle via le huart passé
    extern LORA_Handle_t *lora_handles[]; // tableau de pointeurs vers tous les LORA_Handle_t
    for (int i = 0; i < NUM_LORA_MODULES; i++)
    {
        LORA_Handle_t *hlora = lora_handles[i];

        if (hlora && hlora->huart == huart)
        {
            // Gestion réception
            hlora->rx_buffer[hlora->rx_index++] = hlora->rx_byte;

            if (hlora->rx_index >= (LORA_RX_BUFFER_SIZE - 1))
            {
            	hlora->rx_index = 0;
            }

            hlora->rx_buffer[hlora->rx_index] = '\0';

            HAL_UART_Receive_IT(hlora->huart, (uint8_t*)&hlora->rx_byte, 1);

            break;
        }
    }
}

/* ====================== */
/*    FONCTIONS PUBLIQUES */
/* ====================== */

/**
 * @brief Initialise le module LoRa
 * @param hlora Pointeur vers le handle LoRa
 * @param huart Pointeur vers le handle UART
 * @return Statut de l'opération
 */
LORA_Status_t LORA_Init(LORA_Handle_t *hlora, UART_HandleTypeDef *huart)
{
    if (!hlora || !huart)
    {
    	return LORA_STATUS_INVALID_PARAM;
    }

    memset(hlora, 0, sizeof(LORA_Handle_t));
    hlora->huart = huart;
    hlora->state = LORA_STATE_IDLE;
    hlora->mode  = LORA_MODE_P2P;
    hlora->rx_index = 0;

    HAL_UART_Receive_IT(huart, (uint8_t*)&hlora->rx_byte, 1);

    return LORA_STATUS_OK;
}

/**
 * @brief Teste la communication AT avec le module
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_TestAT(LORA_Handle_t *hlora)
{
    return LORA_SendCommand(hlora, CMD_AT, CMD_AT_ACK, LORA_DEFAULT_TIMEOUT);
}

/**
 * @brief Récupère la version du firmware du module
 * @param hlora Pointeur vers le handle LoRa
 * @param buffer Buffer pour stocker la version
 * @return Statut de l'opération
 */
LORA_Status_t LORA_GetVersion(LORA_Handle_t *hlora, char *buffer)
{
    if (!hlora || !buffer)
    {
    	return LORA_STATUS_INVALID_PARAM;
    }

    memset(hlora->rx_buffer, 0, LORA_RX_BUFFER_SIZE);

    if (LORA_SendCommand(hlora, CMD_VER, "\r\n", 1000) != LORA_STATUS_OK)
    {
    	return LORA_STATUS_ERROR;
    }

    if (strstr(hlora->rx_buffer, "+VER:") == NULL)
    {
    	return LORA_STATUS_ERROR;
    }

    snprintf(buffer, 256, "%s", hlora->rx_buffer);
    return LORA_STATUS_OK;
}

/**
 * @brief Récupère l'ID du module
 * @param hlora Pointeur vers le handle LoRa
 * @param buffer Buffer pour stocker l'ID
 * @return Statut de l'opération
 */
LORA_Status_t LORA_GetID(LORA_Handle_t *hlora, char *buffer)
{
    if (!hlora || !buffer)
    {
    	return LORA_STATUS_INVALID_PARAM;
    }

    memset(hlora->rx_buffer, 0, LORA_RX_BUFFER_SIZE);

    if (LORA_SendCommand(hlora, "AT+ID\r\n", "\r\n ", 2000) == LORA_STATUS_OK)
    {
        return LORA_STATUS_ERROR;
    }

    if (strstr(hlora->rx_buffer, "+ID:") == NULL)
    {
        return LORA_STATUS_ERROR;
    }

    snprintf(buffer, 256, "%s", hlora->rx_buffer);

    return LORA_STATUS_OK;
}

/**
 * @brief Réinitialise le module LoRa
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_Reset(LORA_Handle_t *hlora)
{
    return LORA_SendRaw(hlora, CMD_RESET);
}

/**
 * @brief Réinitialisation usine du module LoRa
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_FactoryReset(LORA_Handle_t *hlora)
{
    return LORA_SendCommand(hlora, CMD_FDEFAULT, CMD_FDEFAULT_ACK, 3000);
}

/**
 * @brief Récupère le statut du module
 * @param hlora Pointeur vers le handle LoRa
 * @param buffer Buffer pour stocker le statut
 * @return Statut de l'opération
 */
LORA_Status_t LORA_GetStatus(LORA_Handle_t *hlora, char *buffer)
{
    if (!hlora || !buffer)
    {
    	return LORA_STATUS_INVALID_PARAM;
    }

    if (LORA_SendCommand(hlora, CMD_STATUS, CMD_STATUS_ACK, LORA_DEFAULT_TIMEOUT) == LORA_STATUS_OK)
    {
        strcpy(buffer, hlora->rx_buffer);
        return LORA_STATUS_OK;
    }
    return LORA_STATUS_ERROR;
}

/* ===================== */
/*         P2P           */
/* ===================== */

/**
 * @brief Configure le module en mode P2P
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_SetMode(LORA_Handle_t *hlora)
{
    return LORA_SendCommand(hlora, CMD_TEST, CMD_TEST_ACK, LORA_DEFAULT_TIMEOUT);
}

/**
 * @brief Applique la configuration P2P
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_Config(LORA_Handle_t *hlora)
{
    return LORA_SendCommand(hlora, CMD_config, CMD_config_ACK,LORA_DEFAULT_TIMEOUT);
}

/**
 * @brief Démarre la réception P2P
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_StartRX(LORA_Handle_t *hlora)
{
    LORA_Status_t status = LORA_P2P_SetMode(hlora);
    if (status != LORA_STATUS_OK)
    {
    	return status;
    }

    status = LORA_SendCommand(hlora, CMD_RECV, CMD_RECV_ACK, LORA_DEFAULT_TIMEOUT);
    if (status == LORA_STATUS_OK)
    {
    	hlora->state = LORA_STATE_RX;
    }

    return status;
}

/**
 * @brief Arrête toute activité P2P
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_Stop(LORA_Handle_t *hlora)
{
    if (!hlora)
    {
    	return LORA_STATUS_INVALID_PARAM;
    }
    hlora->state = LORA_STATE_IDLE;
    return LORA_SendRaw(hlora, CMD_SLEEP);
}

/**
 * @brief Envoie une chaîne de caractères via P2P
 * @param hlora Pointeur vers le handle LoRa
 * @param data Chaîne à envoyer
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_SendString(LORA_Handle_t *hlora, const char *data)
{
    if (!hlora || !data)
    {
    	return LORA_STATUS_INVALID_PARAM;
    }
    if (strlen(data) > 200)
    {
    	return LORA_STATUS_INVALID_PARAM;
    }

    int len = snprintf(hlora->tx_buffer, LORA_TX_BUFFER_SIZE, "%s,\"%s\"\r\n", CMD_SEND, data);
    if (len <= 0 || len >= LORA_TX_BUFFER_SIZE)
    {
    	return LORA_STATUS_ERROR;
    }

    LORA_Status_t status = LORA_SendCommand(hlora, hlora->tx_buffer, CMD_SEND_ACK, LORA_DEFAULT_TIMEOUT);
    if (status == LORA_STATUS_OK)
    {
    	hlora->state = LORA_STATE_TX;
    }

    return status;
}

/**
 * @brief Envoie un tableau de données hexadécimal via P2P
 * @param hlora Pointeur vers le handle LoRa
 * @param data Données à envoyer
 * @param length Longueur des données
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_SendHex(LORA_Handle_t *hlora, const uint8_t *data, uint16_t length)
{
    if (!hlora || !data)
    {
    	return LORA_STATUS_INVALID_PARAM;
    }
    if ((length * 2 + 30) >= LORA_TX_BUFFER_SIZE)
    {
    	return LORA_STATUS_INVALID_PARAM;
    }

    char hex[LORA_TX_BUFFER_SIZE] = {0};
    for (uint16_t i = 0; i < length; i++)
    {
    	sprintf(&hex[i*2], "%02X", data[i]);
    }

    int len = snprintf(hlora->tx_buffer, LORA_TX_BUFFER_SIZE, "%s,\"%s\"\r\n", CMD_SEND, hex);
    if (len <= 0 || len >= LORA_TX_BUFFER_SIZE)
    {
    	return LORA_STATUS_ERROR;
    }

    return LORA_SendCommand(hlora, hlora->tx_buffer, CMD_SEND_ACK, LORA_DEFAULT_TIMEOUT);
}

/**
 * @brief Vérifie si des données P2P sont disponibles
 * @param hlora Pointeur vers le handle LoRa
 * @return true si des données sont disponibles, false sinon
 */
bool LORA_P2P_Available(LORA_Handle_t *hlora)
{
    if (!hlora)
        return false;

    char local_copy[LORA_RX_BUFFER_SIZE];

    __disable_irq();
    memcpy(local_copy, hlora->rx_buffer, LORA_RX_BUFFER_SIZE);
    __enable_irq();

    local_copy[LORA_RX_BUFFER_SIZE - 1] = '\0';

    /* attendre la vraie ligne RX */
    if (strstr(local_copy, "+TEST: RX \"") != NULL)
        return true;

    return false;
}
/**
 * @brief Lit un paquet P2P reçu
 * @param hlora Pointeur vers le handle LoRa
 * @param packet Pointeur vers la structure du paquet
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_Read(LORA_Handle_t *hlora, LORA_P2P_Packet_t *packet)
{
    if (!hlora || !packet)
        return LORA_STATUS_INVALID_PARAM;

    if (!strstr(hlora->rx_buffer, "+TEST: RX \""))
        return LORA_STATUS_ERROR;

    char local[LORA_RX_BUFFER_SIZE];

    //__disable_irq();
    strncpy(local, hlora->rx_buffer, LORA_RX_BUFFER_SIZE - 1);
    //__enable_irq();

    local[LORA_RX_BUFFER_SIZE - 1] = '\0';

    /* -------- RSSI / SNR -------- */
    char *len_line = strstr(local, "+TEST: LEN:");
    if (!len_line)
        return LORA_STATUS_ERROR;

    if (sscanf(len_line, "+TEST: LEN:%*d, RSSI:%hd, SNR:%hhd",
               &packet->rssi, &packet->snr) != 2)
        return LORA_STATUS_ERROR;

    /* -------- PAYLOAD -------- */
    char *rx_line = strstr(local, "+TEST: RX ");
    if (!rx_line)
        return LORA_STATUS_ERROR;

    char *first_quote = strchr(rx_line, '"');
    char *second_quote = strchr(first_quote + 1, '"');

    if (!first_quote || !second_quote)
        return LORA_STATUS_ERROR;

    char hex_payload[256];
    int hex_len = second_quote - (first_quote + 1);

    strncpy(hex_payload, first_quote + 1, hex_len);
    hex_payload[hex_len] = '\0';

    int len = strlen(hex_payload) / 2;

    for (int i = 0; i < len; i++)
    {
        char byte_str[3] = {0};
        byte_str[0] = hex_payload[i * 2];
        byte_str[1] = hex_payload[i * 2 + 1];

        packet->payload[i] = (uint8_t) strtol(byte_str, NULL, 16);
    }

    packet->payload[len] = '\0';
    packet->length = len;

    memset(hlora->rx_buffer, 0, LORA_RX_BUFFER_SIZE);

    return LORA_STATUS_OK;
}

/**
 * @brief Récupère le RSSI du dernier paquet reçu
 * @param hlora Pointeur vers le handle LoRa
 * @return Valeur RSSI
 */
int16_t LORA_P2P_GetRSSI(LORA_Handle_t *hlora)
{
    return hlora->last_packet.rssi;
}

/**
 * @brief Récupère le SNR du dernier paquet reçu
 * @param hlora Pointeur vers le handle LoRa
 * @return Valeur SNR
 */
int8_t  LORA_P2P_GetSNR(LORA_Handle_t *hlora)
{
    return hlora->last_packet.snr;
}
