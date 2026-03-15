#ifndef DHT11_H
#define DHT11_H

#include "main.h"

/**
 * @file dht11.h
 * @brief Driver pour le capteur de température et d'humidité DHT11.
 *
 * Ce module permet :
 *  - La génération de délais en microsecondes
 *  - L'envoi du signal de démarrage au DHT11
 *  - La lecture des bits et octets du capteur
 *  - La récupération de la température et de l'humidité
 *
 * Le driver utilise un timer configuré à 1 µs par tick.
 */


/**
 * @brief Génère un délai en microsecondes.
 *
 * Utilise un timer matériel configuré à 1 MHz
 * (1 tick = 1 microseconde).
 *
 * @param us Nombre de microsecondes à attendre.
 */
void Delay_us(uint16_t us);


/**
 * @brief Envoie le signal de démarrage au capteur DHT11.
 *
 * Configure la broche en sortie open-drain,
 * tire la ligne à 0 pendant 18 ms minimum,
 * puis la relâche et repasse en entrée.
 */
void DHT11_Start(void);


/**
 * @brief Vérifie la réponse du capteur après le signal de démarrage.
 *
 * Le DHT11 doit générer :
 *  - 80 µs LOW
 *  - 80 µs HIGH
 *
 * @retval 1 Si le capteur répond correctement.
 * @retval 0 En cas d'erreur ou absence de réponse.
 */
uint8_t DHT11_CheckResponse(void);


/**
 * @brief Lit un bit transmis par le DHT11.
 *
 * La durée de l'état HIGH permet de déterminer
 * si le bit est 0 ou 1.
 *
 * @retval 0 Bit logique 0.
 * @retval 1 Bit logique 1.
 */
uint8_t DHT11_ReadBit(void);


/**
 * @brief Lit un octet complet (8 bits) depuis le DHT11.
 *
 * Les bits sont lus du MSB vers le LSB.
 *
 * @retval Octet lu.
 */
uint8_t DHT11_ReadByte(void);


/**
 * @brief Lit les données complètes du capteur DHT11.
 *
 * Le capteur envoie 5 octets :
 *  - Humidité entière
 *  - Humidité décimale
 *  - Température entière
 *  - Température décimale
 *  - Checksum
 *
 * Seules les parties entières sont retournées
 * via les pointeurs fournis.
 *
 * @param humidity Pointeur recevant l'humidité (%).
 * @param temperature Pointeur recevant la température (°C).
 */
void DHT11_ReadData(uint8_t *humidity, uint8_t *temperature);


#endif

