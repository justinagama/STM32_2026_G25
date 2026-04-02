# STM32 LCD RGB I2C Driver

Driver embarqué en C pour écran **LCD alphanumérique avec rétroéclairage RGB** via interface **I2C**, compatible **STM32 (HAL)**.

> Inspiré du projet : https://github.com/mcauser/Grove-LCD-RGB-Backlight

---

## Présentation

Ce projet propose une bibliothèque légère permettant de piloter un écran **LCD RGB type Grove** avec un microcontrôleur STM32.

Le driver implémente :

* l’initialisation complète du LCD
* l’affichage de texte (caractère et chaîne)
* le positionnement du curseur
* le contrôle du rétroéclairage RGB
* la détection automatique du contrôleur RGB (**standard ou V5**)

---

## Architecture logicielle

```text
Application utilisateur
        ↓
Driver LCD (lib_lcd)
        ↓
STM32 HAL (I2C)
        ↓
LCD RGB (bus I2C)
```

---

## Prérequis

* Microcontrôleur STM32
* HAL STM32 configuré (**I2C activé**)
* STM32CubeIDE (recommandé)
* Écran LCD RGB I2C compatible (type Grove)

---

## Connexion matérielle

| LCD | STM32     |
| --- | --------- |
| VCC | 3.3V / 5V |
| GND | GND       |
| SDA | I2C SDA   |
| SCL | I2C SCL   |

---

## Installation

1. Copier les fichiers dans votre projet :

```
lib_lcd.c
lib_lcd.h
```

2. Ajouter les fichiers au projet STM32CubeIDE

3. Inclure le driver :

```c
#include "lib_lcd.h"
```

---

## Utilisation

### 🔹 Initialisation

```c
LCD_RGB_HandleTypeDef lcd; // Instance du driver

LCD_RGB_Init(&lcd, &hi2c1, 16, 2);
```

---

### 🔹 Affichage de texte

```c
LCD_EcrireTexte(&lcd, "Hello World");
```

---

### 🔹 Positionnement du curseur

```c
LCD_SetCursor(&lcd, 0, 1);
LCD_EcrireTexte(&lcd, "STM32");
```

---

### 🔹 Effacement de l’écran

```c
LCD_Clear(&lcd);
```

---

### 🔹 Gestion des couleurs

#### Couleurs prédéfinies

```c
LCD_SetCouleur(&lcd, COULEUR_ROUGE);
LCD_SetCouleur(&lcd, COULEUR_VERT);
LCD_SetCouleur(&lcd, COULEUR_BLEU);
LCD_SetCouleur(&lcd, COULEUR_BLANC);
```

#### Couleur personnalisée

```c
LCD_SetRGB(&lcd, 255, 100, 50);
```

---

## Fonctionnalités

* ✅ Communication I2C via HAL STM32
* ✅ Support LCD 16x2 (extensible)
* ✅ Détection automatique du contrôleur RGB
* ✅ API simple et légère
* ✅ Compatible STM32L4 (portable sur autres séries STM32)

---


## 📄 Documentation

Le code est documenté avec **Doxygen** :

```bash
doxygen Doxyfile
```

---

## 🧪 Exemple complet

```c
LCD_RGB_HandleTypeDef lcd;

LCD_RGB_Init(&lcd, &hi2c1, 16, 2);

LCD_SetCouleur(&lcd, COULEUR_BLEU);

LCD_SetCursor(&lcd, 0, 0);
LCD_EcrireTexte(&lcd, "Hello");

LCD_SetCursor(&lcd, 0, 1);
LCD_EcrireTexte(&lcd, "STM32");
```

---


