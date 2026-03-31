#  STM32 SHT31 Temperature & Humidity Driver

Driver embarqué en C pour le capteur **SHT31** (température et humidité) via interface **I2C**, compatible **STM32 (HAL)**.

---

##  Présentation

Ce projet propose une bibliothèque légère permettant d’interfacer le capteur **SHT31** avec un microcontrôleur STM32.

Le SHT31 est un capteur numérique haute précision permettant de mesurer :

*  la température
*  l’humidité relative

Le driver gère :

* l’initialisation du capteur
* l’envoi de commandes I2C
* la lecture des données brutes
* la conversion en unités physiques (°C et %RH)

---


## Prérequis

* Microcontrôleur STM32
* HAL STM32 configuré (**I2C activé**)
* STM32CubeIDE (recommandé)
* Capteur SHT31

---

## Connexion matérielle

| SHT31 | STM32   |
| ----- | ------- |
| VCC   | 3.3V    |
| GND   | GND     |
| SDA   | I2C SDA |
| SCL   | I2C SCL |

---

## Installation

1. Ajouter les fichiers au projet :

```
sht31.c
sht31.h
```

2. Inclure le driver :

```c
#include "sht31.h"
```

---

## Utilisation

### 🔹 Initialisation

```c
SHT31_t sht;

SHT31_Init(&sht, &hi2c1);
```

---

### 🔹 Lecture température et humidité

```c
SHT31_ReadTempHum(&sht);

float temperature = sht.temperature;
float humidity = sht.humidity;
```

---

## Données retournées

Après appel de :

```c
SHT31_ReadTempHum(&sht);
```

Les valeurs sont disponibles dans la structure :

```c
sht.temperature; // en °C
sht.humidity;    // en %
```

---

## Configuration

* Adresse I2C par défaut : `0x44` (shiftée → `0x88` pour HAL)
* Mode de mesure utilisé :

  * **High Repeatability** (`SHT31_MEAS_HIGH_REP`)

---

## Fonctionnalités

* ✅ Communication I2C via HAL STM32
* ✅ Lecture température et humidité
* ✅ Conversion en unités physiques
* ✅ Interface simple et légère
* ✅ Compatible STM32L4 (portable)

---


## Exemple complet

```c
SHT31_t sht;

SHT31_Init(&sht, &hi2c1);

while (1)
{
    if (SHT31_ReadTempHum(&sht) == HAL_OK)
    {
        float temp = sht.temperature;
        float hum  = sht.humidity;
    }

    HAL_Delay(1000);
}
```
## 📚 Documentation
* 📄 [Datasheet SHT3x-DIS (PDF)](https://sensirion.com/media/documents/213E6A3B/63A5A569/Datasheet_SHT3x_DIS.pdf) — *Document complet : registres, timings et précision.*
