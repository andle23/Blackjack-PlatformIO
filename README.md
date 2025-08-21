# Arduino Blackjack Game
A simple Blackjack game implemented on an Arduino using a 16x2 LCD and two push buttons. The player can hit with the left button or stand with the right button, and the dealer follows standard Blackjack rules.

## Features
- Deck implemented with `uint8_t` to be memory-efficient.
- Player input using Hit and Stand buttons.
- Dealer logic follows standard Blackjack rules (peeks on Ace or 10, draws until 17+).
- Custom LCD characters for card suits (♥ ♦ ♣ ♠).
- Handles Blackjack, busts, pushes, and scoring.
- Resets and starts new rounds automatically.

## Hardware Requirements
- Arduino Uno (or compatible)
- 16x2 LCD (16-pin)
- 220Ω resistor
- 2 push buttons
- Jumper wires and breadboard

## Wiring Notes
1. Connect the LCD and buttons according to the schematic.
2. All wires in the schematic are color-coded to make the connections easy to follow. The colors are for clarity and don’t need to match the physical wires you use.

## Circuit Schematic
<img width="3000" height="2159" alt="Image" src="https://github.com/user-attachments/assets/f767935b-b2cb-4ad8-b0e5-b609f8ad3203" />

## Blackjack Demo
- [Watch Demo on YouTube] https://youtu.be/1TzBKKulm90

## Software Setup

### If using PlatformIO:
1. Add the LiquidCrystal library to `platformio.ini`:
   ```ini
   lib_deps = fmalpartida/LiquidCrystal@^1.5.0
