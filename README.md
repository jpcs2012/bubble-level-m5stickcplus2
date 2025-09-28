# Digital Bubble Level – M5StickC Plus2

This project turns the **M5StickC Plus2** into a digital bubble level using its built-in IMU (MPU6886).  
It provides both a graphical bubble display and numerical pitch/roll readings.

---

## ✨ Features
- Displays **Pitch and Roll** (rounded to 0.5°).
- Bubble turns **green** when the device is leveled (±0.5°).
- **Button A** performs zero calibration.
- Visual crosshair for precise alignment.

---

## 📦 Requirements
- [PlatformIO](https://platformio.org/)
- [`M5Unified`](https://registry.platformio.org/libraries/m5stack/M5Unified) library

---

## 🚀 Installation
Clone this repository and upload it to your device using PlatformIO:


git clone https://github.com/jpcs2012/bubble-level-m5stickcplus2.git
cd bubble-level-m5stickcplus2
pio run --target upload

# Nível de Bolha Digital para M5StickC Plus2

Projeto em PlatformIO/Arduino que transforma o **M5StickC Plus2** num nível de bolha digital:
- Mostra **Pitch e Roll** arredondados a 0,5°.
- Bolha no ecrã fica **verde** quando está nivelado (±0,5°).
- Botão **A** faz **calibração a zero**.

## 📦 Requisitos
- [PlatformIO](https://platformio.org/)
- Biblioteca [`M5Unified`](https://registry.platformio.org/libraries/m5stack/M5Unified)

## 🚀 Instalação

git clone https://github.com/teu-user/nivel-bolha-m5stickcplus2.git
cd nivel-bolha-m5stickcplus2
pio run --target upload
