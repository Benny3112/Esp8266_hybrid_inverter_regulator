#include "wifi_manager.h"
#include "web.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <Arduino.h>  // Für Serial Ausgaben

// Globale Variablen für gespeicherte Konfigurationsdaten
String savedSSID = "";
String savedPassword = "";
String savedMQTTServer = "";
int savedMQTTPort = 1883;
String savedMQTTUser = "";
String savedMQTTPassword = "";

void saveMQTTConfig(const char* server, int port, const char* user, const char* password);

void handleWiFiConfig() {
    // Code zur Handhabung der WLAN-Verbindung
}

void loadConfig() {
    if (LittleFS.begin()) {
        Serial.println("LittleFS gestartet.");
        if (LittleFS.exists("/config.json")) {
            Serial.println("Konfigurationsdatei gefunden. Lade Daten...");
            File configFile = LittleFS.open("/config.json", "r");
            if (configFile) {
                size_t size = configFile.size();
                std::unique_ptr<char[]> buf(new char[size]);
                configFile.readBytes(buf.get(), size);
                DynamicJsonDocument doc(1024);
                deserializeJson(doc, buf.get());

                savedSSID = doc["ssid"].as<String>();
                savedPassword = doc["password"].as<String>();
                savedMQTTServer = doc["mqtt_server"].as<String>();
                savedMQTTPort = doc["mqtt_port"] | 1883;
                savedMQTTUser = doc["mqtt_user"].as<String>();
                savedMQTTPassword = doc["mqtt_password"].as<String>();

                Serial.println("Konfigurationsdaten geladen:");
                Serial.println("SSID: " + savedSSID);
                Serial.println("MQTT Server: " + savedMQTTServer);
            }
        } else {
            Serial.println("Keine Konfigurationsdatei gefunden.");
        }
    } else {
        Serial.println("Fehler beim Starten von LittleFS.");
    }
}

void saveWiFiConfig() {
    DynamicJsonDocument doc(1024);
    doc["ssid"] = savedSSID;
    doc["password"] = savedPassword;
    doc["mqtt_server"] = savedMQTTServer;
    doc["mqtt_port"] = savedMQTTPort;
    doc["mqtt_user"] = savedMQTTUser;
    doc["mqtt_password"] = savedMQTTPassword;

    File configFile = LittleFS.open("/config.json", "w");
    if (configFile) {
        serializeJson(doc, configFile);
        Serial.println("Konfigurationsdaten gespeichert.");
    } else {
        Serial.println("Fehler beim Speichern der Konfigurationsdaten.");
    }
}

void setupWiFiManager() {
    Serial.println("Starte WiFi Manager...");
    loadConfig();
    
    if (savedSSID != "") {
        Serial.println("Versuche Verbindung zu SSID: " + savedSSID);
        WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
        unsigned long startAttemptTime = millis();

        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 60000) {
            Serial.print(".");
            delay(500);
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nMit WLAN verbunden!");
            Serial.print("IP-Adresse: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("\nVerbindung fehlgeschlagen. Wechsle in den AP-Modus.");
            WiFi.mode(WIFI_AP);
            WiFi.softAP("Wechselrichter_Config", "12345678");
            Serial.print("AP-Modus aktiv. IP-Adresse: ");
            Serial.println(WiFi.softAPIP());
        }
    } else {
        Serial.println("Keine gespeicherten WLAN-Daten gefunden. Starte AP-Modus.");
        WiFi.mode(WIFI_AP);
        WiFi.softAP("Wechselrichter_Config", "12345678");
        Serial.print("AP-Modus aktiv. IP-Adresse: ");
        Serial.println(WiFi.softAPIP());
    }

    // Webserver für Konfigurationsseite
    server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = "<html><body><h1>Konfiguration</h1>";
        html += "<form method='POST' action='/config'>";
        html += "SSID: <input type='text' name='ssid' value='" + savedSSID + "'><br>";
        html += "Password: <input type='password' name='password' value='" + savedPassword + "'><br>";
        html += "MQTT Server: <input type='text' name='mqtt_server' value='" + savedMQTTServer + "'><br>";
        html += "MQTT Port: <input type='number' name='mqtt_port' value='" + String(savedMQTTPort) + "'><br>";
        html += "MQTT User: <input type='text' name='mqtt_user' value='" + savedMQTTUser + "'><br>";
        html += "MQTT Password: <input type='password' name='mqtt_password' value='" + savedMQTTPassword + "'><br>";
        html += "<input type='submit' value='Speichern'>";
        html += "</form><br><a href='/'>Zurück</a></body></html>";
        request->send(200, "text/html", html);
    });

    server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("ssid", true)) savedSSID = request->getParam("ssid", true)->value();
        if (request->hasParam("password", true)) savedPassword = request->getParam("password", true)->value();
        if (request->hasParam("mqtt_server", true)) savedMQTTServer = request->getParam("mqtt_server", true)->value();
        if (request->hasParam("mqtt_port", true)) savedMQTTPort = request->getParam("mqtt_port", true)->value().toInt();
        if (request->hasParam("mqtt_user", true)) savedMQTTUser = request->getParam("mqtt_user", true)->value();
        if (request->hasParam("mqtt_password", true)) savedMQTTPassword = request->getParam("mqtt_password", true)->value();

        saveWiFiConfig();
        request->send(200, "text/html", "Konfiguration gespeichert. <a href='/'>Zurück zur Hauptseite</a>");
        ESP.restart();
    });

    server.begin();
    Serial.println("Webserver gestartet.");
}
