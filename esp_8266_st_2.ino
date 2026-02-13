#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "ESP32_IDS";
const char* password = "12345678";

WiFiUDP udp;
const char* esp32_ip = "192.168.4.1";
const int udpPort = 5005;

#define BUTTON_PIN 5

unsigned long packetCount = 0;
unsigned long byteCount = 0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to ESP32!");
  lastTime = millis();
}

void loop() {

  bool attack = digitalRead(BUTTON_PIN) == LOW;
  String mode = attack ? "ATTACK" : "NORMAL";

  // Simulate traffic
  if (attack) {
    packetCount += 200;   // heavy attack packets
    byteCount += 200 * 512;
  } else {
    packetCount += 1;     // normal traffic
    byteCount += 1 * 32;
  }

  // Calculate every 5 seconds
  if (millis() - lastTime >= 5000) {
    float packetRate = packetCount / 5.0;
    float bitrate = (byteCount * 8) / 5.0;
    float rssi = WiFi.RSSI();

    packetCount = 0;
    byteCount = 0;
    lastTime = millis();

    char data[120];
    sprintf(data, "%s,%.2f,%.2f,%.2f", mode.c_str(), packetRate, bitrate, rssi);

    udp.beginPacket(esp32_ip, udpPort);
    udp.write(data);
    udp.endPacket();

    Serial.println(data);
  }

  delay(100);
}
