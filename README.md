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
