
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Golioth.h>
#include <Update.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "settings.h"

#ifndef RAW_VERSION
#define RAW_VERSION local
#endif
#define STRINGIFY(s) STRINGIFY1(s)
#define STRINGIFY1(s) #s
#define VERSION STRINGIFY(RAW_VERSION)

int status = WL_IDLE_STATUS;

WiFiClientSecure net;
GoliothClient* client = GoliothClient::getInstance();

unsigned long lastMillis = 0;
unsigned long counter = 0;

void onDownloadArtifact(String pkg, String version, uint8_t* buf,
                        size_t buf_size, int current, int total) {
  Serial.print("Downloading " + pkg + " " + version + " " + String(current) +
               "/" + String(total) + " ");
  Serial.print(buf_size);
  Serial.println(" bytes");
  if (current == 0) {
    if (pkg == "main") {
      if (!Update.begin(total)) {
        Serial.println("Not enough space to begin OTA");
      }
    }
  }
  if (buf_size) {
    if (pkg == "main") {
      Update.write(buf, buf_size);
    }
  }
  if (current >= total) {
    if (pkg == "main") {
      if (Update.end()) {
        Serial.println("Update complete");
        if (Update.isFinished()) {
          Serial.println("Update successfully completed. Rebooting.");
          ESP.restart();
        } else {
          Serial.println("Update not finished? Something went wrong!");
        }
      } else {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }
    }
  }
}

void connect() {
  String wifissid = getWifiSSID();
  String wifipass = getWifiPSK();
  if (wifissid.length() == 0 || wifipass.length() == 0) {
    delay(1000);
    return;
  }
  String pskid = getGoliothPSKID();
  String psk = getGoliothPSK();
  if (pskid.length() == 0 || psk.length() == 0) {
    delay(1000);
    return;
  }
  Serial.print("checking wifi...");
  int tries = 0;
  WiFi.begin(wifissid.c_str(), wifipass.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    if (tries > 5) {
      Serial.println("Wifi not connected");
      return;
    }
    Serial.print(".");
    delay(1000);
    tries++;
  }
  Serial.println("Connected to WiFi!");

  Serial.println("connecting to cloud gateway...");
  tries = 0;

  net.setCACert(GOLIOTH_ROOT_CA);

  client->setClient(net);
  client->setPSKId(pskid.c_str());
  client->setPSK(psk.c_str());
  while (!client->connect()) {
    if (tries > 1) {
      Serial.println("not connected");
      return;
    }
    Serial.print(".");
    delay(1000);
    tries++;
  }

  Serial.println("Connected to Golioth");

  client->onHello([](String name) { Serial.println(name); });
  client->listenHello();
  client->onDesiredVersionChanged([](String pkg, String version, String hash) {
    Serial.println("New version - " + pkg + " " + version + " " + hash);
    if (pkg == "main" && version != VERSION) {
      Serial.println("Update available");
      client->downloadArtifact(pkg.c_str(), version.c_str());
    }
  });
  client->onDownloadArtifact(onDownloadArtifact);
  client->listenDesiredVersion();
}

void setup() {
  Serial.begin(115200);
  Serial.print("running version ");
  Serial.println(VERSION);

  client->setClient(net);

  setupSettings();

  delay(100);
  connect();
  if (client->connected()) {
    client->logInfo("running version " + String(VERSION));
  }
}

void loop() {
  client->poll();
  handleSettingsCLI();

  if (!net.connected() || !client->connected()) {
    connect();
  }

  if (millis() - lastMillis > 5 * 1000 && client->connected()) {
    lastMillis = millis();
    counter++;
    client->setLightDBStateAtPath("/counter", String(counter).c_str());
    lastMillis = millis();
  }
}