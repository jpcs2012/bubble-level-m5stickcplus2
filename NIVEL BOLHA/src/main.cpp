#include <M5Unified.h>
#include <math.h>

float accX, accY, accZ;
float offsetPitch = 0.0, offsetRoll = 0.0;

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  M5.Lcd.setRotation(1);   // Horizontal
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("Press BtnA para calibrar");
}

void loop() {
  M5.update();
  M5.Imu.getAccel(&accX, &accY, &accZ);

  // inverter sinais para alinhar com display
  float X = -accY;
  float Y = -accX;

  // Calcular Pitch e Roll em graus
  float pitch = atan2(accX, sqrt(accY * accY + accZ * accZ)) * 180.0 / M_PI;
  float roll  = atan2(accY, accZ) * 180.0 / M_PI;

  // Calibrar: guardar valores atuais como zero
  if (M5.BtnA.wasPressed()) {
    offsetPitch = pitch;
    offsetRoll  = roll;
    M5.Lcd.setTextColor(GREEN, BLACK);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.println("Calibrado!            ");
  }

  // Aplicar calibração
  pitch -= offsetPitch;
  roll  -= offsetRoll;

  // Arredondar a 0,5 grau
  pitch = round(pitch * 2.0) / 2.0;
  roll  = round(roll * 2.0) / 2.0;

  // Limpar área principal
  M5.Lcd.fillRect(0, 30, 240, 100, BLACK);

  // Calcular posição da bolha
  int x = map(X * 100, -100, 100, 20, 220);
  int y = map(Y * 100, -100, 100, 40, 120);

  // Moldura verde
  M5.Lcd.drawRect(20, 40, 200, 80, GREEN);

  // Cruz vermelha
  M5.Lcd.drawLine(120, 40, 120, 120, RED);
  M5.Lcd.drawLine(20, 80, 220, 80, RED);

  // Condição para "nivelado"
  bool nivelado = (fabs(pitch) <= 0.5 && fabs(roll) <= 0.5);

  // Bolha: verde se nivelado, amarela se não
  M5.Lcd.fillCircle(x, y, 8, nivelado ? GREEN : YELLOW);

  // Mostrar Pitch e Roll no topo
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(10, 5);
  M5.Lcd.printf("P: %.1f  R: %.1f   ", pitch, roll);

  delay(100);
}
