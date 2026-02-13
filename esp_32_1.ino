#include <WiFi.h>
#include <WiFiUdp.h>
#include <string.h>

// ================= WiFi Hotspot =================
const char* ap_ssid = "ESP32_IDS";
const char* ap_pass = "12345678";

// ================= UDP ==========================
WiFiUDP udp;
const int udpPort = 5005;
char incomingPacket[255];

// ================= UART2 ========================
#define TXD2 17
#define RXD2 16

// ================= TinyML Model =================
void score(double * input, double * output) {
    double var0[2];
    if (input[1] <= -60.0) {   // RSSI based ML decision
        memcpy(var0, (double[]){0.0, 1.0}, 2 * sizeof(double)); // Attack
    } else {
        memcpy(var0, (double[]){1.0, 0.0}, 2 * sizeof(double)); // Normal
    }
    memcpy(output, var0, 2 * sizeof(double));
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);

  Serial.println("ESP32 #1 HOTSPOT STARTED");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  udp.begin(udpPort);
  Serial.println("UDP Server Started");
}

void loop() {

  int packetSize = udp.parsePacket();
  if (packetSize) {

    int len = udp.read(incomingPacket, 255);
    if (len > 0) incomingPacket[len] = 0;

    Serial.print("RAW UDP: ");
    Serial.println(incomingPacket);

    // ========== Parse Data ==========
    char mode[10];
    float packetRate, bitrate, rssi;

    sscanf(incomingPacket, "%[^,],%f,%f,%f",
           mode, &packetRate, &bitrate, &rssi);

    Serial.print("Mode: "); Serial.print(mode);
    Serial.print(" | PacketRate: "); Serial.print(packetRate);
    Serial.print(" | Bitrate: "); Serial.print(bitrate);
    Serial.print(" | RSSI: "); Serial.println(rssi);

    // ========== TinyML Inference ==========
    double input[2];
    double output[2];

    input[0] = packetRate;
    input[1] = rssi;

    score(input, output);

    bool attackFlag = false;

    if (output[1] > output[0]) {
        attackFlag = true;
        Serial.println("🚨 ATTACK DETECTED BY TinyML!");
    } else {
        Serial.println("✅ NORMAL TRAFFIC (TinyML)");
    }

    // ========== Send to ESP32 #2 ==========
    Serial2.printf("%s,%.2f,%.2f,%.2f,%d\n",
                   mode, packetRate, bitrate, rssi, attackFlag);
  }
}
