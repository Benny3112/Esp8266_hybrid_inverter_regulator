#include <ESP8266WiFi.h>  // Für WiFiClient
#include <PubSubClient.h> // Für MQTT-Funktionalität
#include "mqtt.h"         // Die eigene Header-Datei muss eingebunden sein

WiFiClient espClient;      // WiFiClient initialisieren
PubSubClient client(espClient);  // MQTT-Client mit WiFiClient verbinden

void setupMQTT() {
    // MQTT Initialisierungscode
}

void connectToMQTT(PubSubClient& client, const char* mqtt_user, const char* mqtt_password) {
  while (!client.connected()) {
    Serial.print("Verbinde mit MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("verbunden!");
    } else {
      Serial.print("Fehler, rc=");
      Serial.print(client.state());
      Serial.println(" Versuche es in 5 Sekunden erneut...");
      delay(5000);
    }
  }
}

void publishMQTT(PubSubClient& client, const char* topic, const String& payload) {
  if (client.publish(topic, payload.c_str())) {
    Serial.println("MQTT-Payload gesendet: " + payload);
  } else {
    Serial.println("Fehler beim Senden des MQTT-Payloads");
  }
}


void handleMQTT() {
    if (!client.connected()) {
        if (client.connect("ESP8266Client", "mqtt", "mqtt")) {
            Serial.println("Verbunden mit MQTT");
        } else {
            Serial.print("Fehler beim Verbinden: ");
            Serial.println(client.state());
        }
    }
    client.loop();
}

void publishMQTT(const char* topic, const String& payload) {
    client.publish(topic, payload.c_str());
}

void saveMQTTConfig(const char* server, int port, const char* user, const char* password) {

}