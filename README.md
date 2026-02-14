# ESP32 E-Paper Display (Master’s Thesis Project)

This repository contains the ESP32 firmware developed during my Master’s thesis project for an Electronic Shelf Label (ESL) prototype.  
The system focuses on reliable e-paper display updates with low power usage and robust SPI-based display control. It was designed as a code-centric prototype to evaluate display performance and end-to-end data flow in an ESL-like setup.

## What this project does

- Drives a 2.7" e-paper display using SPI on an ESP32
- Handles display initialization, refresh/update routines, and basic display rendering
- Designed for repeated, stable updates

## Hardware (typical setup)

- ESP32 development board (or custom ESP32 PCB)
- 2.7" e-paper display module (SPI)
- Power supply (5V input + regulator to 3.3V if needed)
- Jumper wires/test points (depending on setup)

Note: Exact pin mapping depends on your board and display module.

## Firmware overview

The firmware is structured around:
- SPI communication to the display
- Display driver / low-level routines
- Application logic for updating the screen content
- Basic reliability handling (init/refresh cycles)

## Build & flash (ESP-IDF)

### Prerequisites
- ESP-IDF installed and configured
- USB drivers for your ESP32 board
