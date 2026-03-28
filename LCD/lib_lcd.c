
/**
 * @file lib_lcd.c
 * @brief Driver LCD RGB I2C pour STM32 (compatible Grove / LCD RGB)
 * @author 
 * @date 2026
 *
 * @details
 * Ce driver permet de piloter un écran LCD alphanumérique avec rétroéclairage RGB
 * via une interface I2C. Il supporte deux types de drivers RGB (version classique et V5).
 *
 * Fonctionnalités :
 * - Initialisation LCD
 * - Affichage texte
 * - Positionnement curseur
 * - Gestion rétroéclairage RGB
 */

#include <lib_lcd.h>
#include <string.h>

/* ================= FONCTIONS INTERNES ================= */

/**
 * @brief Envoie des données brutes au LCD via I2C
 * @param lcd Pointeur vers la structure LCD
 * @param data Buffer de données à envoyer
 * @param taille Taille du buffer
 * @retval None
 */
void LCD_I2C_Send(LCD_RGB_HandleTypeDef *lcd, uint8_t *data, uint8_t taille)
{
    HAL_I2C_Master_Transmit(lcd->hi2c, LCD_ADRESSE, data, taille, 100);
}

/**
 * @brief Écrit une valeur dans un registre du contrôleur RGB
 * @param lcd Pointeur vers la structure LCD
 * @param reg Adresse du registre
 * @param val Valeur à écrire
 * @retval None
 */
void RGB_SetRegistre(LCD_RGB_HandleTypeDef *lcd, uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    HAL_I2C_Master_Transmit(lcd->hi2c, lcd->adresse_rgb, buf, 2, 100);
}

/* ================= INITIALISATION ================= */

/**
 * @brief Initialise le LCD RGB
 * @param lcd Structure de configuration LCD
 * @param hi2c Handle I2C utilisé
 * @param colonnes Nombre de colonnes (non utilisé directement)
 * @param lignes Nombre de lignes
 * @retval None
 *
 * @details
 * - Initialise le LCD en mode 2 lignes
 * - Configure le mode d’affichage
 * - Détecte automatiquement le driver RGB
 * - Initialise le rétroéclairage
 */
void LCD_RGB_Init(LCD_RGB_HandleTypeDef *lcd, I2C_HandleTypeDef *hi2c,
                  uint8_t colonnes, uint8_t lignes)
{
    lcd->hi2c = hi2c;
    lcd->nb_lignes = lignes;
    lcd->fonction_affichage = LCD_2_LIGNES | LCD_5x8_DOTS;

    HAL_Delay(50);

    LCD_Commande(lcd, LCD_FUNCTIONSET | lcd->fonction_affichage);
    HAL_Delay(5);
    LCD_Commande(lcd, LCD_FUNCTIONSET | lcd->fonction_affichage);
    HAL_Delay(1);
    LCD_Commande(lcd, LCD_FUNCTIONSET | lcd->fonction_affichage);

    lcd->controle_affichage = LCD_DISPLAY_ON;
    LCD_Commande(lcd, LCD_DISPLAYCTRL | lcd->controle_affichage);

    LCD_Clear(lcd);

    lcd->mode_affichage = LCD_ENTRY_LEFT | LCD_ENTRY_SHIFTDEC;
    LCD_Commande(lcd, LCD_ENTRYMODE | lcd->mode_affichage);

    /* Détection automatique du driver RGB */
    if (HAL_I2C_IsDeviceReady(hi2c, RGB_ADRESSE_V5, 3, 100) == HAL_OK)
    {
        lcd->adresse_rgb = RGB_ADRESSE_V5;

        RGB_SetRegistre(lcd, 0x00, 0x07);
        HAL_Delay(1);
        RGB_SetRegistre(lcd, 0x04, 0x15);
    }
    else
    {
        lcd->adresse_rgb = RGB_ADRESSE;

        RGB_SetRegistre(lcd, REG_MODE1, 0x00);
        RGB_SetRegistre(lcd, REG_SORTIE, 0xFF);
        RGB_SetRegistre(lcd, REG_MODE2, 0x20);
    }

    LCD_SetCouleur(lcd, COULEUR_BLANC);
}

/* ================= COMMANDES LCD ================= */

/**
 * @brief Envoie une commande au LCD
 * @param lcd Structure LCD
 * @param cmd Commande à envoyer
 * @retval None
 */
void LCD_Commande(LCD_RGB_HandleTypeDef *lcd, uint8_t cmd)
{
    uint8_t buf[2] = {0x80, cmd};
    LCD_I2C_Send(lcd, buf, 2);
}

/**
 * @brief Écrit un caractère sur le LCD
 * @param lcd Structure LCD
 * @param data Caractère à afficher
 * @retval None
 */
void LCD_EcrireCaractere(LCD_RGB_HandleTypeDef *lcd, uint8_t data)
{
    uint8_t buf[2] = {0x40, data};
    LCD_I2C_Send(lcd, buf, 2);
}

/**
 * @brief Écrit une chaîne de caractères sur le LCD
 * @param lcd Structure LCD
 * @param texte Chaîne à afficher
 * @retval None
 */
void LCD_EcrireTexte(LCD_RGB_HandleTypeDef *lcd, char *texte)
{
    while (*texte)
    {
        LCD_EcrireCaractere(lcd, *texte++);
    }
}

/**
 * @brief Efface l’écran LCD
 * @param lcd Structure LCD
 * @retval None
 */
void LCD_Clear(LCD_RGB_HandleTypeDef *lcd)
{
    LCD_Commande(lcd, LCD_CLEAR);
    HAL_Delay(2);
}

/**
 * @brief Replace le curseur à la position initiale
 * @param lcd Structure LCD
 * @retval None
 */
void LCD_Home(LCD_RGB_HandleTypeDef *lcd)
{
    LCD_Commande(lcd, LCD_HOME);
    HAL_Delay(2);
}

/**
 * @brief Positionne le curseur
 * @param lcd Structure LCD
 * @param col Colonne (0 à N)
 * @param ligne Ligne (0 ou 1)
 * @retval None
 */
void LCD_SetCursor(LCD_RGB_HandleTypeDef *lcd, uint8_t col, uint8_t ligne)
{
    uint8_t adresse = (ligne == 0) ? (0x80 + col) : (0xC0 + col);
    LCD_Commande(lcd, adresse);
}

/* ================= GESTION RGB ================= */

/**
 * @brief Définit la couleur RGB du rétroéclairage
 * @param lcd Structure LCD
 * @param r Intensité rouge (0-255)
 * @param g Intensité verte (0-255)
 * @param b Intensité bleue (0-255)
 * @retval None
 */
void LCD_SetRGB(LCD_RGB_HandleTypeDef *lcd, uint8_t r, uint8_t g, uint8_t b)
{
    if (lcd->adresse_rgb == RGB_ADRESSE_V5)
    {
        RGB_SetRegistre(lcd, 0x06, r);
        RGB_SetRegistre(lcd, 0x07, g);
        RGB_SetRegistre(lcd, 0x08, b);
    }
    else
    {
        RGB_SetRegistre(lcd, REG_ROUGE, r);
        RGB_SetRegistre(lcd, REG_VERT, g);
        RGB_SetRegistre(lcd, REG_BLEU, b);
    }
}

/**
 * @brief Définit une couleur prédéfinie
 * @param lcd Structure LCD
 * @param couleur Code couleur (COULEUR_*)
 * @retval None
 */
void LCD_SetCouleur(LCD_RGB_HandleTypeDef *lcd, uint8_t couleur)
{
    switch (couleur)
    {
        case COULEUR_ROUGE: LCD_SetRGB(lcd, 255, 0, 0); break;
        case COULEUR_VERT:  LCD_SetRGB(lcd, 0, 255, 0); break;
        case COULEUR_BLEU:  LCD_SetRGB(lcd, 0, 0, 255); break;
        default:            LCD_SetRGB(lcd, 255, 255, 255); break;
    }
}
