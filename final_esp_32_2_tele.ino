#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#define RXD2 16
#define TXD2 17

const char* ssid = "Nite";
const char* pass = "dontknow";

String BOT_TOKEN = "8567551754:AAEXq1gx-9MttB345_RnMB38q0Zx4bOgFLg";
String CHAT_ID   = "5053708807";

WiFiClientSecure client;   // ✅ MUST BE GLOBAL

unsigned long lastAlert = 0;

void sendTelegram(String msg) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ No Internet");
    return;
  }

  client.setInsecure(); // skip SSL cert validation

  HTTPClient https;
  String url = "https://api.telegram.org/bot" + BOT_TOKEN +
               "/sendMessage?chat_id=" + CHAT_ID +
               "&text=" + msg;

  Serial.println("Sending Telegram...");
  https.begin(client, url);

  int httpCode = https.GET();
  Serial.println("Telegram HTTP Code: " + String(httpCode));

  https.end();
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  WiFi.begin(ssid, pass);
  Serial.print("Connecting Internet");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Internet Connected");
  Serial.println(WiFi.localIP());

  sendTelegram("ESP32 IDS Controller Online");
}

void loop() {
  if (Serial2.available()) {
    String line = Serial2.readStringUntil('\n');
    Serial.println("RAW UART: " + line);

    char mode[10];
    float packetRate, bitrate, rssi;
    int attackFlag;

    sscanf(line.c_str(), "%[^,],%f,%f,%f,%d",
           mode, &packetRate, &bitrate, &rssi, &attackFlag);

    Serial.printf("Mode:%s Rate:%.2f Bitrate:%.2f RSSI:%.2f Attack:%d\n",
                  mode, packetRate, bitrate, rssi, attackFlag);

    if (attackFlag == 1 && millis() - lastAlert > 15000) {
      Serial.println("🚨 ATTACK ALERT!");

      String msg = "🚨 ATTACK ALERT! ATTACK Rate=" + String(packetRate) +
                   " Bitrate=" + String(bitrate);

      sendTelegram(msg);
      lastAlert = millis();
    }
  }
}
