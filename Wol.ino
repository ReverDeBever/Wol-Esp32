#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>

const char* ssid = "H369ABF038A";
const char* password = "D5679F29DF69";
const char* targetMac = "bc:ae:c5:33:8f:1e";

AsyncWebServer server(80);
WiFiUDP udp;

void sendWakeOnLan(const char* macAddress) {
  byte mac[6];
  if (sscanf(macAddress, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) != 6) {
    Serial.println("Invalid MAC address format!");
    return;
  }

  byte packet[102];
  memset(packet, 0xFF, 6);
  for (int i = 1; i <= 16; i++) {
    memcpy(&packet[i * 6], mac, 6);
  }
  udp.beginPacket("192.168.2.255", 9);
  udp.write(packet, sizeof(packet));
  udp.endPacket();
  Serial.println("WOL packet sent!");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<button onclick=\"fetch('/wake')\">Wake PC</button><div id='msg'></div>"
                  "<script>"
                  "document.querySelector('button').onclick=function(){"
                  "fetch('/wake').then(r=>r.text()).then(txt=>document.getElementById('msg').innerText=txt);"
                  "}"
                  "</script>";
    request->send(200, "text/html", html);
  });

  server.on("/wake", HTTP_GET, [](AsyncWebServerRequest *request){
    sendWakeOnLan(targetMac);
    request->send(200, "text/plain", "WOL sent!");
  });

  server.begin();
  udp.begin(12345);
}

void loop() {}
