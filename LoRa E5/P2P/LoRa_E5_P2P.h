/*
 * LoRa_E5_P2P.h
 *
 *  Created on: Mar 3, 2026
 *      Author: agama
 */

/**
 * @file    lora_e5.h
 * @author  Justin AGAMA
 * @brief   Driver STM32 HAL pour le module Grove LoRa-E5 (P2P uniquement)
 * @version 1.1.0
 * @date    2026
 * 
 * @details
 * Cette bibliothèque permet de piloter le module Grove LoRa-E5
 * via des commandes AT sur une interface UART.
 *
 * Version actuelle orientée mode P2P.
 * 
 */

#ifndef LORA_E5_H
#define LORA_E5_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/* ========================= */
/*        CONSTANTES         */
/* ========================= */

/** @brief Taille du buffer de réception LoRa */
#define LORA_RX_BUFFER_SIZE    256U
/** @brief Taille du buffer d'émission LoRa */
#define LORA_TX_BUFFER_SIZE    256U
/** @brief Timeout par défaut pour les commandes LoRa (ms) */
#define LORA_DEFAULT_TIMEOUT   1000U
/** @brief Nombre de modules LoRa utilisés */
#define NUM_LORA_MODULES 1

/** @brief Commandes AT pour le module LoRa */
#define CMD_AT              "AT\r\n"
#define CMD_AT_ACK          "+AT: OK"
#define CMD_TEST            "AT+MODE=TEST\r\n"
#define CMD_TEST_ACK        "+MODE: TEST"
#define CMD_RECV            "AT+ TEST= RXLRPKT\r\n"
#define CMD_RECV_ACK        "+TEST: RXLRPKT"
#define RECV_ACK            "+TEST: RX"
#define CMD_SEND            "AT+TEST=TXLRSTR"
#define CMD_SEND_ACK        "+TEST: TXLRSTR"
#define CMD_SLEEP           "AT+LOWPOWER\r\n"
#define CMD_VER             "AT+VER\r\n"
#define CMD_VER_ACK         "+VER:"
#define CMD_RESET           "AT+RESET\r\n"
#define CMD_FDEFAULT        "AT+FDEFAULT\r\n"
#define CMD_FDEFAULT_ACK    "+FDEFAULT: OK"
#define CMD_ID              "AT+ID\r\n"
#define CMD_ID_ACK          "+ID:"
#define CMD_STATUS          "AT+STATUS?\r\n"
#define CMD_STATUS_ACK      "+STATUS:"
#define CMD_config          "AT+TEST=RFCFG,868.3,SF7,125,8,8,20,ON,OFF,OFF\r\n"
#define CMD_config_ACK      "+TEST: RFCFG F:868300000, SF7, BW125K, TXPR:8, RXPR:8, POW:20dBm, CRC:ON, IQ:OFF, NET:OFF"

/* ========================= */
/*        TYPES ENUM         */
/* ========================= */

/**
 * @brief Statut général des fonctions LoRa
 */
typedef enum
{
    LORA_STATUS_OK = 0,           /**< Opération réussie */
    LORA_STATUS_ERROR,            /**< Erreur générique */
    LORA_STATUS_TIMEOUT,          /**< Timeout dépassé */
    LORA_STATUS_BUSY,             /**< Module occupé */
    LORA_STATUS_INVALID_PARAM     /**< Paramètre invalide */
} LORA_Status_t;

/**
 * @brief État actuel du module LoRa
 */
typedef enum
{
    LORA_STATE_IDLE = 0,  /**< Module en attente */
    LORA_STATE_RX,        /**< Module en réception */
    LORA_STATE_TX,        /**< Module en émission */
    LORA_STATE_SLEEP,     /**< Module en veille */
    LORA_STATE_ERROR      /**< Module en erreur */
} LORA_State_t;

/**
 * @brief Mode de fonctionnement du module LoRa
 */
typedef enum
{
    LORA_MODE_P2P = 0,   /**< Mode point-à-point */
    LORA_MODE_LORAWAN    /**< Mode LoRaWAN */
} LORA_Mode_t;

/* ========================= */
/*         STRUCTS           */
/* ========================= */

/**
 * @brief Structure d'un paquet P2P LoRa reçu
 */
typedef struct
{
    uint8_t  payload[LORA_RX_BUFFER_SIZE]; /**< Données reçues */
    uint16_t length;                       /**< Longueur des données */
    int16_t  rssi;                         /**< Niveau RSSI */
    int8_t   snr;                          /**< Rapport signal/bruit */
} LORA_P2P_Packet_t;

/**
 * @brief Structure de gestion du module LoRa
 */
typedef struct
{
    UART_HandleTypeDef *huart;             					/**< Handle UART associé */
    volatile uint8_t   rx_byte;            					/**< Octet reçu en interruption */
    volatile uint8_t   rx_flag;            					/**< Indique si un octet est reçu */
    char               rx_buffer[LORA_RX_BUFFER_SIZE]; 		/**< Buffer de réception */
    uint16_t           rx_index;           					/**< Index courant du buffer RX */
    char               tx_buffer[LORA_TX_BUFFER_SIZE]; 		/**< Buffer d'émission */
    LORA_State_t       state;              					/**< État du module */
    LORA_Mode_t        mode;               					/**< Mode LoRa (P2P / LoRaWAN) */
    LORA_P2P_Packet_t  last_packet;        					/**< Dernier paquet reçu */
} LORA_Handle_t;

/* ========================= */
/*       FONCTIONS PUBLIQUES */
/* ========================= */


LORA_Status_t LORA_SendRaw(LORA_Handle_t *hlora, const char *cmd);

/**
 * @brief Initialise le module LoRa
 * @param hlora Pointeur vers le handle LoRa
 * @param huart Pointeur vers le handle UART
 * @return Statut de l'opération
 */
LORA_Status_t LORA_Init(LORA_Handle_t *hlora, UART_HandleTypeDef *huart);

/**
 * @brief Teste la communication AT avec le module
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_TestAT(LORA_Handle_t *hlora);

/**
 * @brief Récupère la version du firmware du module
 * @param hlora Pointeur vers le handle LoRa
 * @param buffer Buffer pour stocker la version
 * @return Statut de l'opération
 */
LORA_Status_t LORA_GetVersion(LORA_Handle_t *hlora,char *buffer);

/**
 * @brief Réinitialise le module LoRa
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_Reset(LORA_Handle_t *hlora);

/**
 * @brief Réinitialisation usine du module LoRa
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_FactoryReset(LORA_Handle_t *hlora);

/**
 * @brief Récupère l'ID du module
 * @param hlora Pointeur vers le handle LoRa
 * @param buffer Buffer pour stocker l'ID
 * @return Statut de l'opération
 */
LORA_Status_t LORA_GetID(LORA_Handle_t *hlora, char *buffer);

/**
 * @brief Récupère le statut du module
 * @param hlora Pointeur vers le handle LoRa
 * @param buffer Buffer pour stocker le statut
 * @return Statut de l'opération
 */
LORA_Status_t LORA_GetStatus(LORA_Handle_t *hlora, char *buffer);

/**
 * @brief Configure le module en mode P2P
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_SetMode(LORA_Handle_t *hlora);

/**
 * @brief Applique la configuration P2P
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_Config(LORA_Handle_t *hlora);

/**
 * @brief Démarre la réception P2P
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_StartRX(LORA_Handle_t *hlora);

/**
 * @brief Arrête toute activité P2P
 * @param hlora Pointeur vers le handle LoRa
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_Stop(LORA_Handle_t *hlora);

/**
 * @brief Envoie une chaîne de caractères via P2P
 * @param hlora Pointeur vers le handle LoRa
 * @param data Chaîne à envoyer
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_SendString(LORA_Handle_t *hlora, const char *data);

/**
 * @brief Envoie un tableau de données hexadécimal via P2P
 * @param hlora Pointeur vers le handle LoRa
 * @param data Données à envoyer
 * @param length Longueur des données
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_SendHex(LORA_Handle_t *hlora, const uint8_t *data, uint16_t length);

/**
 * @brief Vérifie si des données P2P sont disponibles
 * @param hlora Pointeur vers le handle LoRa
 * @return true si des données sont disponibles, false sinon
 */
bool LORA_P2P_Available(LORA_Handle_t *hlora);

/**
 * @brief Lit un paquet P2P reçu
 * @param hlora Pointeur vers le handle LoRa
 * @param packet Pointeur vers la structure du paquet
 * @return Statut de l'opération
 */
LORA_Status_t LORA_P2P_Read(LORA_Handle_t *hlora, LORA_P2P_Packet_t *packet);

/**
 * @brief Récupère le RSSI du dernier paquet reçu
 * @param hlora Pointeur vers le handle LoRa
 * @return Valeur RSSI
 */
int16_t LORA_P2P_GetRSSI(LORA_Handle_t *hlora);

/**
 * @brief Récupère le SNR du dernier paquet reçu
 * @param hlora Pointeur vers le handle LoRa
 * @return Valeur SNR
 */
int8_t LORA_P2P_GetSNR(LORA_Handle_t *hlora);

#ifdef __cplusplus
}
#endif

#endif /* LORA_E5_H */
