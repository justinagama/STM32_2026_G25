# STM32 HX711 Load Cell Driver

Driver embarqué en C pour le convertisseur analogique-numérique **HX711**, utilisé avec des capteurs de force (load cells), compatible **STM32 (HAL)**.

---

## Présentation

Ce projet implémente un driver permettant d’interfacer facilement le module **HX711** avec un microcontrôleur STM32.

Le HX711 est un **ADC 24 bits haute précision** utilisé pour la mesure de poids via des jauges de contrainte.

Le driver gère :

* la communication GPIO bit-banging avec le HX711
* la lecture des données 24 bits
* la gestion du signe (complément à 2)
* le moyennage des mesures
* la tare (offset)
* la conversion en poids réel



---

## ⚙️ Prérequis

* Microcontrôleur STM32
* HAL STM32 configuré (GPIO)
* STM32CubeIDE (recommandé)
* Module HX711
* Capteur de force (load cell)

---

## 🔌 Connexion matérielle

| HX711 | STM32       |
| ----- | ----------- |
| VCC   | 3.3V / 5V   |
| GND   | GND         |
| DT    | GPIO Input  |
| SCK   | GPIO Output |

---

## 📦 Installation

1. Ajouter les fichiers au projet :

```
hx711.c
hx711.h
```

2. Inclure le driver :

```c
#include "hx711.h"
```

---

## 🛠️ Utilisation

### 🔹 Initialisation

```c
HX711_t hx;

HX711_Init(&hx, GPIOA, GPIO_PIN_0, GPIOA, GPIO_PIN_1);
```

---

### 🔹 Lecture brute

```c
long value = HX711_Read(&hx);
```

---

### 🔹 Moyennage

```c
long avg = HX711_ReadAverage(&hx, 10);
```

---

### 🔹 Tare (mise à zéro)

```c
HX711_Tare(&hx, 10);
```

---

### 🔹 Lecture du poids

```c
float weight = HX711_GetWeight(&hx, 10);
```

---

## Calibration

Le coefficient doit être ajusté expérimentalement :

```c
hx.coefficient = 1000.0f; // exemple
```

Méthode :

1. Lire la valeur brute avec un poids connu
2. Calculer :

```text
coefficient = valeur_brute / poids_reel
```

---

## Fonctionnalités

* ✅ Lecture 24 bits (ADC haute résolution)
* ✅ Gestion du signe (complément à 2)
* ✅ Moyennage configurable
* ✅ Fonction tare (offset automatique)
* ✅ Conversion en unité physique
* ✅ Compatible STM32 HAL

---

## ⚠️ Limitations

* Bit-banging GPIO → dépend du timing CPU
* Utilise un délai précis basé sur **DWT**
* Pas de gestion d’erreurs avancée (timeout simple)
* Pas de gestion basse consommation

---

## ⏱️ Timing critique

Le driver utilise le compteur **DWT (Data Watchpoint and Trace)** pour générer des délais en microsecondes :

```c
delay_us(1);
```

⚠️ Nécessite :

* activation du DWT
* horloge système correctement configurée


---

## 🧪 Exemple complet

```c
HX711_t hx;

HX711_Init(&hx, GPIOA, GPIO_PIN_0, GPIOA, GPIO_PIN_1);

// Calibration
hx.coefficient = 1000.0f;

// Tare
HX711_Tare(&hx, 10);

while (1)
{
    float poids = HX711_GetWeight(&hx, 10);
    HAL_Delay(500);
}
```
## 📚 Documentation et Liens
* 📄 [Mode d'emploi Joy-It SEN-HX711-01 (7 pages)](https://joy-it.net/en/products/SEN-HX711-01)
* ⚙️ [Datasheet du composant HX711](https://www.mouser.com/datasheet/2/813/hx711_english-1022875.pdf)
