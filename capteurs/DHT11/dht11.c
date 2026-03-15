#include "dht11.h"

/**
 * @brief Handle du timer utilisé pour les délais en microsecondes.
 *
 * Ce timer doit être configuré pour générer 1 tick = 1 µs.
 */
extern TIM_HandleTypeDef htim6;


/**
 * @brief Génère un délai en microsecondes.
 *
 * Utilise le timer TIM6 configuré à 1 MHz (1 tick = 1 µs).
 *
 * @param us Nombre de microsecondes à attendre.
 */
void Delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    while (__HAL_TIM_GET_COUNTER(&htim6) < us);
}


/**
 * @brief Envoie le signal de démarrage au capteur DHT11.
 *
 * Procédure :
 * 1. Configure la broche en sortie open-drain.
 * 2. Met la ligne à 0 pendant au moins 18 ms.
 * 3. Relâche la ligne.
 * 4. Reconfigure la broche en entrée pour attendre la réponse du capteur.
 */
void DHT11_Start(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /** Configuration PA9 en sortie open-drain */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /** Tirer la ligne à 0 pendant 18 ms */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_Delay(18);

    /** Relâcher la ligne */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
    Delay_us(30);

    /** Reconfigurer PA9 en entrée */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


/**
 * @brief Vérifie la réponse du capteur DHT11.
 *
 * Après le signal de démarrage, le DHT11 doit :
 * - Tirer la ligne à 0 (~80 µs)
 * - Puis la tirer à 1 (~80 µs)
 *
 * @retval 1 si le capteur répond correctement
 * @retval 0 sinon
 */
uint8_t DHT11_CheckResponse(void)
{
    uint8_t response = 0;

    Delay_us(40);

    if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9))
    {
        Delay_us(80);

        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9))
            response = 1;
    }

    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9));

    return response;
}


/**
 * @brief Lit un bit envoyé par le DHT11.
 *
 * Le DHT11 encode les bits ainsi :
 * - 0 : ~26-28 µs à l'état HIGH
 * - 1 : ~70 µs à l'état HIGH
 *
 * @retval 0 si le bit lu est 0
 * @retval 1 si le bit lu est 1
 */
uint8_t DHT11_ReadBit(void)
{
    while (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9));

    Delay_us(40);

    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9))
    {
        while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9));
        return 1;
    }
    else
    {
        return 0;
    }
}


/**
 * @brief Lit un octet (8 bits) depuis le DHT11.
 *
 * Les bits sont lus du MSB vers le LSB.
 *
 * @retval Octet lu depuis le capteur.
 */
uint8_t DHT11_ReadByte(void)
{
    uint8_t i, byte = 0;

    for (i = 0; i < 8; i++)
    {
        byte <<= 1;
        byte |= DHT11_ReadBit();
    }

    return byte;
}


/**
 * @brief Lit les données complètes du capteur DHT11.
 *
 * Le DHT11 envoie 5 octets :
 *  - Humidité entière
 *  - Humidité décimale
 *  - Température entière
 *  - Température décimale
 *  - Checksum
 *
 * Seules les parties entières sont retournées (DHT11 ne fournit
 * pas de décimales significatives).
 *
 * @param humidity Pointeur vers la variable recevant l'humidité (%).
 * @param temperature Pointeur vers la variable recevant la température (°C).
 */
void DHT11_ReadData(uint8_t *humidity, uint8_t *temperature)
{
    uint8_t hum_int, hum_dec, temp_int, temp_dec, checksum;

    DHT11_Start();

    if (DHT11_CheckResponse())
    {
        hum_int  = DHT11_ReadByte();
        hum_dec  = DHT11_ReadByte();
        temp_int = DHT11_ReadByte();
        temp_dec = DHT11_ReadByte();
        checksum = DHT11_ReadByte();

        if (checksum == (hum_int + hum_dec + temp_int + temp_dec))
        {
            *humidity = hum_int;
            *temperature = temp_int;
//            *humidity_dec =hum_dec;
//			*temperature_dec = temp_dec;
        }
    }
}
