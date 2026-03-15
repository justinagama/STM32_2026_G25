#ifndef LIB_LCD_H_
#define LIB_LCD_H_


#include "stm32l4xx_hal.h"
#include <stdint.h>

/* ================= ADRESSES I2C ================= */
/* Adresses 7 bits (HAL = adresse << 1) */
#define LCD_ADRESSE        (0x3E << 1)
#define RGB_ADRESSE        (0x62 << 1)
#define RGB_ADRESSE_V5     (0x30 << 1)

/* ================= COULEURS ================= */
#define COULEUR_BLANC      0
#define COULEUR_ROUGE      1
#define COULEUR_VERT       2
#define COULEUR_BLEU       3

/* ================= REGISTRES RGB ================= */
#define REG_MODE1          0x00
#define REG_MODE2          0x01
#define REG_SORTIE         0x08

#define REG_ROUGE          0x04
#define REG_VERT           0x03
#define REG_BLEU           0x02

/* ================= COMMANDES LCD ================= */
#define LCD_CLEAR          0x01
#define LCD_HOME           0x02
#define LCD_ENTRYMODE      0x04
#define LCD_DISPLAYCTRL    0x08
#define LCD_CURSORSHIFT    0x10
#define LCD_FUNCTIONSET    0x20
#define LCD_SETCGRAM       0x40
#define LCD_SETDDRAM       0x80

/* ================= FLAGS LCD ================= */
#define LCD_DISPLAY_ON     0x04
#define LCD_CURSOR_ON      0x02
#define LCD_BLINK_ON       0x01

#define LCD_ENTRY_LEFT     0x02
#define LCD_ENTRY_SHIFTDEC 0x00

#define LCD_2_LIGNES       0x08
#define LCD_5x8_DOTS       0x00

/* ================= STRUCTURE LCD ================= */
typedef struct
{
    I2C_HandleTypeDef *hi2c;
    uint8_t fonction_affichage;
    uint8_t controle_affichage;
    uint8_t mode_affichage;
    uint8_t nb_lignes;
    uint16_t adresse_rgb;
} LCD_RGB_HandleTypeDef;

/* ================= PROTOTYPES ================= */

/* Initialisation */
void LCD_RGB_Init(LCD_RGB_HandleTypeDef *lcd, I2C_HandleTypeDef *hi2c,
                  uint8_t colonnes, uint8_t lignes);

/* Commandes LCD */
void LCD_Clear(LCD_RGB_HandleTypeDef *lcd);
void LCD_Home(LCD_RGB_HandleTypeDef *lcd);
void LCD_SetCursor(LCD_RGB_HandleTypeDef *lcd, uint8_t col, uint8_t ligne);
void LCD_Commande(LCD_RGB_HandleTypeDef *lcd, uint8_t cmd);
void LCD_EcrireCaractere(LCD_RGB_HandleTypeDef *lcd, uint8_t data);
void LCD_EcrireTexte(LCD_RGB_HandleTypeDef *lcd, char *texte);

/* Rétroéclairage RGB */
void LCD_SetRGB(LCD_RGB_HandleTypeDef *lcd, uint8_t r, uint8_t g, uint8_t b);
void LCD_SetCouleur(LCD_RGB_HandleTypeDef *lcd, uint8_t couleur);

/* Bas niveau */
void LCD_I2C_Send(LCD_RGB_HandleTypeDef *lcd, uint8_t *data, uint8_t taille);
void RGB_SetRegistre(LCD_RGB_HandleTypeDef *lcd, uint8_t reg, uint8_t val);

#endif
