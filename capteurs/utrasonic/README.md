#  STM32 Ultrasonic Distance Sensor & RGB LCD

Projet embarqué basé sur STM32 permettant la mesure de distance en temps réel à l’aide d’un capteur ultrason Grove Ultrasonic Ranger, avec affichage sur un LCD RGB I2C et sortie debug via UART.

##  Description

Ce système mesure la distance entre le capteur et un obstacle, puis affiche l’information sur deux interfaces :

-  Afficheur LCD RGB
  - Affichage de la distance en temps réel
  - Rétroéclairage dynamique selon la distance (mode sécurité)

- 🔌 Console Série (UART)
  - Envoi des mesures vers un PC
  - Debug et monitoring en temps réel

##  Configuration Matérielle

| Composant                | Interface | Broche STM32 (exemple) |
|-------------------------|----------|------------------------|
| Grove Ultrasonic Ranger | SIG      | Défini dans ultra.h    |
| LCD RGB Grove           | I2C1 SDA | PB7                    |
| LCD RGB Grove           | I2C1 SCL | PB6                    |
| Console PC              | UART2    | PA2 (TX) / PA3 (RX)    |

##  Configuration STM32CubeMX

- TIM2 : résolution 1 µs (1 MHz après prescaler)
- I2C1 : Standard ou Fast Mode
- USART2 : 115200, 8N1

##  Logique du Code

Boucle principale avec rafraîchissement toutes les 200 ms.

### Gestion des couleurs

- < 2.5 cm : ERREUR → Rouge
- 2.5 - 10 cm : Alerte → Rouge
- > 10 cm : OK → Vert

### Optimisation

Fonction msgDist() pour formater les données sans stdio (gain mémoire).

##  Exemple

GROVE ULTRASONIC  
25.4 cm

## 📂 Structure

Core/
├── Src/
│   ├── main.c
│   ├── ultra.c
│   ├── lib_lcd.c
├── Inc/
│   ├── ultra.h
│   ├── lib_lcd.h

##  Installation

1. Copier les fichiers drivers dans votre projet
2. Configurer TIM2, I2C1 et USART2
3. Compiler avec STM32CubeIDE
4. Flasher la carte
5. Ouvrir un terminal série (115200)

##  Fonctionnalités

- Mesure distance temps réel
- Affichage LCD RGB dynamique
- Sortie UART debug
- Gestion erreurs

##  Projet

STM32L4 - 2026
