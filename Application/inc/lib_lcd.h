/**
 * @file lib_lcd.h
 * @brief Driver LCD RGB via I2C pour STM32
 *
 * @details
 * Ce fichier contient les définitions, macros et prototypes nécessaires
 * pour piloter un écran LCD RGB via interface I2C.
 */

#ifndef LIB_LCD_H_
#define LIB_LCD_H_

#include "stm32l4xx_hal.h"
#include <stdint.h>

/* ================= ADRESSES I2C ================= */
/**
 * @brief Adresse I2C du LCD (7 bits décalée pour HAL)
 */
#define LCD_ADRESSE        (0x3E << 1)

/**
 * @brief Adresse I2C du contrôleur RGB (version standard)
 */
#define RGB_ADRESSE        (0x62 << 1)

/**
 * @brief Adresse I2C du contrôleur RGB (version V5)
 */
#define RGB_ADRESSE_V5     (0x30 << 1)

/* ================= COULEURS ================= */
/**
 * @brief Couleur blanche
 */
#define COULEUR_BLANC      0

/**
 * @brief Couleur rouge
 */
#define COULEUR_ROUGE      1

/**
 * @brief Couleur verte
 */
#define COULEUR_VERT       2

/**
 * @brief Couleur bleue
 */
#define COULEUR_BLEU       3

/* ================= REGISTRES RGB ================= */
/**
 * @brief Registre MODE1 du contrôleur RGB
 */
#define REG_MODE1          0x00

/**
 * @brief Registre MODE2 du contrôleur RGB
 */
#define REG_MODE2          0x01

/**
 * @brief Registre de configuration des sorties
 */
#define REG_SORTIE         0x08

/**
 * @brief Registre intensité rouge
 */
#define REG_ROUGE          0x04

/**
 * @brief Registre intensité verte
 */
#define REG_VERT           0x03

/**
 * @brief Registre intensité bleue
 */
#define REG_BLEU           0x02

/* ================= COMMANDES LCD ================= */
/**
 * @brief Efface l'affichage
 */
#define LCD_CLEAR          0x01

/**
 * @brief Retour à la position initiale
 */
#define LCD_HOME           0x02

/**
 * @brief Configuration du mode d'entrée
 */
#define LCD_ENTRYMODE      0x04

/**
 * @brief Contrôle de l'affichage
 */
#define LCD_DISPLAYCTRL    0x08

/**
 * @brief Déplacement curseur/affichage
 */
#define LCD_CURSORSHIFT    0x10

/**
 * @brief Configuration des fonctions LCD
 */
#define LCD_FUNCTIONSET    0x20

/**
 * @brief Accès à la mémoire CGRAM
 */
#define LCD_SETCGRAM       0x40

/**
 * @brief Accès à la mémoire DDRAM
 */
#define LCD_SETDDRAM       0x80

/* ================= FLAGS LCD ================= */
/**
 * @brief Active l'affichage
 */
#define LCD_DISPLAY_ON     0x04

/**
 * @brief Active le curseur
 */
#define LCD_CURSOR_ON      0x02

/**
 * @brief Active le clignotement du curseur
 */
#define LCD_BLINK_ON       0x01

/**
 * @brief Mode écriture de gauche à droite
 */
#define LCD_ENTRY_LEFT     0x02

/**
 * @brief Pas de décalage de l'affichage
 */
#define LCD_ENTRY_SHIFTDEC 0x00

/**
 * @brief Mode 2 lignes
 */
#define LCD_2_LIGNES       0x08

/**
 * @brief Police 5x8
 */
#define LCD_5x8_DOTS       0x00

/* ================= STRUCTURE LCD ================= */
/**
 * @brief Structure de gestion du LCD RGB
 */
typedef struct
{
    I2C_HandleTypeDef *hi2c;   /**< Handle du périphérique I2C */
    uint8_t fonction_affichage; /**< Configuration des fonctions LCD */
    uint8_t controle_affichage; /**< État de l'affichage */
    uint8_t mode_affichage;     /**< Mode d'entrée */
    uint8_t nb_lignes;          /**< Nombre de lignes */
    uint16_t adresse_rgb;       /**< Adresse du contrôleur RGB */
} LCD_RGB_HandleTypeDef;

/* ================= PROTOTYPES ================= */

/* Initialisation */
/**
 * @brief Initialise le LCD RGB
 * @param lcd Structure LCD
 * @param hi2c Handle I2C
 * @param colonnes Nombre de colonnes
 * @param lignes Nombre de lignes
 * @retval None
 */
void LCD_RGB_Init(LCD_RGB_HandleTypeDef *lcd, I2C_HandleTypeDef *hi2c,
                  uint8_t colonnes, uint8_t lignes);

/* Commandes LCD */
/**
 * @brief Efface l'écran
 */
void LCD_Clear(LCD_RGB_HandleTypeDef *lcd);

/**
 * @brief Retourne le curseur à la position initiale
 */
void LCD_Home(LCD_RGB_HandleTypeDef *lcd);

/**
 * @brief Positionne le curseur
 * @param col Colonne
 * @param ligne Ligne
 */
void LCD_SetCursor(LCD_RGB_HandleTypeDef *lcd, uint8_t col, uint8_t ligne);

/**
 * @brief Envoie une commande au LCD
 */
void LCD_Commande(LCD_RGB_HandleTypeDef *lcd, uint8_t cmd);

/**
 * @brief Écrit un caractère
 */
void LCD_EcrireCaractere(LCD_RGB_HandleTypeDef *lcd, uint8_t data);

/**
 * @brief Écrit une chaîne de caractères
 */
void LCD_EcrireTexte(LCD_RGB_HandleTypeDef *lcd, char *texte);

/* Rétroéclairage RGB */
/**
 * @brief Définit une couleur RGB personnalisée
 */
void LCD_SetRGB(LCD_RGB_HandleTypeDef *lcd, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Définit une couleur prédéfinie
 */
void LCD_SetCouleur(LCD_RGB_HandleTypeDef *lcd, uint8_t couleur);

/* Bas niveau */
/**
 * @brief Envoie des données via I2C
 */
void LCD_I2C_Send(LCD_RGB_HandleTypeDef *lcd, uint8_t *data, uint8_t taille);

/**
 * @brief Écrit dans un registre RGB
 */
void RGB_SetRegistre(LCD_RGB_HandleTypeDef *lcd, uint8_t reg, uint8_t val);

#endif