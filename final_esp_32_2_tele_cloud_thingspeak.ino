#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ThingSpeak.h>

#define RXD2 16
#define TXD2 17

const char* ssid = "Nite";
const char* pass = "dontknow";

// Telegram
String BOT_TOKEN = "YOUR TELEGRAM API";
String CHAT_ID   = "YOUR CHANNEL ID ";

// ThingSpeak
unsigned long channelID = "THING SPEAK CHANNEL ID (without quotes)";
const char* writeAPIKey = "API key";

WiFiClient tsClient;              // ThingSpeak client
WiFiClientSecure telegramClient;  // Telegram client

unsigned long lastAlert = 0;
unsigned long lastTSUpdate = 0;

// =====================================================
// TELEGRAM FUNCTION 
// ===================================
void sendTelegram(String msg) {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi Not Connected");
    return;
  }

  telegramClient.setInsecure();

  HTTPClient https;
  String url = "https://api.telegram.org/bot" + BOT_TOKEN + "/sendMessage";

  https.begin(telegramClient, url);
  https.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String postData = "chat_id=" + CHAT_ID + "&text=" + msg;

  int httpCode = https.POST(postData);

  Serial.print("Telegram HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode > 0) {
    String response = https.getString();
    Serial.println(response);
  } else {
    Serial.println("❌ Telegram Failed");
  }

  https.end();
}

// =====================================================
// SETUP
// =====================================================
void setup() {

  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  // Connect WiFi
  WiFi.begin(ssid, pass);
  Serial.print("Connecting Internet");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n Internet Connected");
  Serial.println(WiFi.localIP());

  // Initialize ThingSpeak
  ThingSpeak.begin(tsClient);

  // Online message
  sendTelegram("ESP32 IDS Controller Online");
}

// =====================================================
// LOOP
// =====================================================
void loop() {

  if (Serial2.available()) {

    String line = Serial2.readStringUntil('\n');
    line.trim();

    Serial.println("Received: [" + line + "]");

    // ================= PARSING (SAFE METHOD) =================
    int first = line.indexOf(',');
    int second = line.indexOf(',', first + 1);
    int third = line.indexOf(',', second + 1);
    int fourth = line.indexOf(',', third + 1);

    if (first == -1 || second == -1 || third == -1 || fourth == -1) {
      Serial.println("❌ Parsing Error");
      return;
    }

    String modeStr = line.substring(0, first);
    float packetRate = line.substring(first + 1, second).toFloat();
    float bitrate = line.substring(second + 1, third).toFloat();
    float rssi = line.substring(third + 1, fourth).toFloat();
    int attackFlag = line.substring(fourth + 1).toInt();

    Serial.println("Mode: " + modeStr);
    Serial.println("PacketRate: " + String(packetRate));
    Serial.println("Bitrate: " + String(bitrate));
    Serial.println("RSSI: " + String(rssi));
    Serial.println("AttackFlag: " + String(attackFlag));

    // ================= THINGSPEAK UPDATE =================
    if (millis() - lastTSUpdate > 15000) {

      ThingSpeak.setField(1, packetRate);
      ThingSpeak.setField(2, bitrate);
      ThingSpeak.setField(3, rssi);
      ThingSpeak.setField(4, attackFlag);

      int modeValue = (modeStr == "ATTACK") ? 1 : 0;
      ThingSpeak.setField(5, modeValue);

      int x = ThingSpeak.writeFields(channelID, writeAPIKey);

      if (x == 200) {
        Serial.println(" ThingSpeak Updated");
      } else {
        Serial.println(" ThingSpeak Error: " + String(x));
      }

      lastTSUpdate = millis();
    }

    // ================= TELEGRAM ALERT =================
    if (attackFlag == 1 && millis() - lastAlert > 15000) {

      Serial.println("🚨 ATTACK DETECTED - Sending Telegram");

      String msg = "ALERT! IDS Attack Detected\n";
      msg += "Rate: " + String(packetRate) + "\n";
      msg += "Bitrate: " + String(bitrate) + "\n";
      msg += "RSSI: " + String(rssi);

      sendTelegram(msg);

      lastAlert = millis();
    }
  }
}
