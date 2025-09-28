/* main.cpp
   M5StickC Plus2 + ELM327 (Bluetooth by MAC) + MQTT + web logs
*/

#include <Arduino.h>
#include <M5StickCPlus2.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "BluetoothSerial.h"
#include <WebServer.h>

// ===== CONFIG WIFI =====
const char* ssid = "SILVA";
const char* password = "Silva2024.";

// ===== CONFIG MQTT =====
const char* mqtt_server = "pt.ultrabyte.cc";
const int   mqtt_port   = 1993;
const char* mqtt_user   = "jsilva";
const char* mqtt_pass   = "Joao1996.";

// ===== BLUETOOTH MAC =====
uint8_t obd_mac[6] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xBA};  // substitui pelo MAC real

// ===== GLOBALS =====
WiFiClient espClient;
PubSubClient client(espClient);
BluetoothSerial SerialBT;
WebServer server(80);

#define LOG_LINES 16
String logs[LOG_LINES];
int logIndex = 0;

// ===== LOGGING =====
void addLog(const String &s) {
  logs[logIndex] = s;
  logIndex = (logIndex + 1) % LOG_LINES;

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  int idx = logIndex;
  for (int i = 0; i < LOG_LINES; ++i) {
    const String &line = logs[idx];
    if (line.length() > 0) {
      M5.Lcd.println(line);
    }
    idx = (idx + 1) % LOG_LINES;
  }
}

void logPrint(const char* tag, const String &msg) {
  String line = String(tag) + " " + msg;
  addLog(line);
  Serial.println(line);
}

// ===== WIFI & MQTT =====
void setup_wifi() {
  logPrint("[WIFI]", "Conectando...");
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    M5.Lcd.print(".");
    if ((millis() - start) > 20000) {
      logPrint("[WIFI]", "Timeout (20s). Tentando reconectar...");
      start = millis();
    }
  }
  String ip = WiFi.localIP().toString();
  logPrint("[WIFI]", "Conectado: " + ip);

  // Mostrar IP no fundo do display
  M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setCursor(0, M5.Lcd.height() - 20);
  M5.Lcd.printf("IP: %s", ip.c_str());
}

void reconnect() {
  while (!client.connected()) {
    logPrint("[MQTT]", "Conectando...");
    if (client.connect("M5StickCPlus2", mqtt_user, mqtt_pass)) {
      logPrint("[MQTT]", "Conectado!");
      client.publish("car/test", "M5StickC Plus2 pronto!");
    } else {
      logPrint("[MQTT]", "Falha conectar, rc=" + String(client.state()));
      delay(3000);
    }
  }
}

// ===== ELM327 initialization =====
void initELM327() {
  delay(500);
  SerialBT.println("ATZ");   // reset
  delay(800);
  SerialBT.println("ATE0");  // echo off
  delay(200);
  SerialBT.println("ATL0");  // linefeeds off
  delay(200);
  SerialBT.println("ATS0");  // spaces off
  delay(200);
  SerialBT.println("ATH0");  // headers off
  delay(200);
  SerialBT.println("0100");  // supported PIDs
  delay(300);
  logPrint("[ELM]", "Inicializado");
}

// ===== WEB SERVER =====
String makeHomePage(const String &ip) {
  String html = "<!doctype html><html><head><meta charset='utf-8'>"
                "<title>M5 OBD2 Logs</title>"
                "<style>body{font-family:monospace;background:#111;color:#0f0} .box{white-space:pre-wrap}</style>"
                "</head><body><h3>M5StickC Plus2 - OBD2 logs</h3>"
                "<p>IP: " + ip + "</p>"
                "<pre id='logbox' class='box'>Loading...</pre>"
                "<script>async function upd(){try{let r=await fetch('/status');let t=await r.text();document.getElementById('logbox').textContent=t;}catch(e){document.getElementById('logbox').textContent='error:'+e}}setInterval(upd,1000);upd();</script>"
                "</body></html>";
  return html;
}

void handleRoot() {
  String ip = WiFi.localIP().toString();
  server.send(200, "text/html", makeHomePage(ip));
}

void handleStatus() {
  String out = "";
  int idx = logIndex;
  for (int i = 0; i < LOG_LINES; ++i) {
    const String &line = logs[idx];
    if (line.length() > 0) out += line + "\n";
    idx = (idx + 1) % LOG_LINES;
  }
  server.send(200, "text/plain", out);
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  for (int i=0;i<LOG_LINES;i++) logs[i] = "";

  logPrint("[BOOT]", "Inicializando...");

  // WiFi
  setup_wifi();

  // Web server
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.begin();
  logPrint("[WEB]", "Server ativo");

  // MQTT
  client.setServer(mqtt_server, mqtt_port);

  // Bluetooth como cliente
  if (!SerialBT.begin("M5OBD", true)) {
    logPrint("[BT]", "Falha stack BT");
  } else {
    logPrint("[BT]", "Stack BT ok, tentando ligar...");
    if (SerialBT.connect(obd_mac)) {
      logPrint("[BT]", "Ligado ao OBDII por MAC");
      initELM327();
    } else {
      logPrint("[BT]", "Falha ao ligar (pode exigir PIN)");
    }
  }
}

// ===== LOOP =====
void loop() {
  M5.update();

  // Botão B = Reset
  if (M5.BtnB.wasPressed()) {
    logPrint("[BTN]", "Reset...");
    delay(300);
    ESP.restart();
  }

  // Botão A = Limpar ecrã
  if (M5.BtnA.wasPressed()) {
    for (int i=0;i<LOG_LINES;i++) logs[i] = "";
    logIndex = 0;
    addLog("Ecrã limpo!");
  }

  server.handleClient();

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) reconnect();
    client.loop();
  }

  static unsigned long lastCmd = 0;
  if (millis() - lastCmd > 3000) {
    if (SerialBT.connected()) {
      SerialBT.print("010C\r");
      logPrint("[OBD]", "Enviado 010C");
    } else {
      logPrint("[OBD]", "BT não conectado");
    }
    lastCmd = millis();
  }

  if (SerialBT.available()) {
    String response = SerialBT.readStringUntil('\r');
    response.trim();
    if (response.length() > 0) {
      logPrint("[RESP]", response);
      if (client.connected()) {
        client.publish("car/obd/raw", response.c_str());
      }
    }
  }

  delay(10);
}
