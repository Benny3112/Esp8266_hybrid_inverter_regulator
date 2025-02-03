#include "rs232.h"
#include "inverter.h"
#include "mqtt.h"
#include "web.h"
#include "wifi_manager.h"  // Falls du den WiFi-Manager nutzt

void setup() {
    Serial.begin(115200);

    // WLAN-Verbindung herstellen oder AP-Modus starten
    setupWiFiManager();

    // RS232 initialisieren
    setupRS232();  // Falls in rs232.cpp implementiert

    // MQTT initialisieren
    setupMQTT();

    // Webserver initialisieren
    setupWebInterface();
}

void loop() {
    // Webserver-Handling
    handleWebInterface();
    handleWiFiConfig(); // Falls AP-Modus aktiv ist

    // Automatische WLAN-Reconnect-Logik
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("⚠ WLAN-Verbindung verloren! Versuche Reconnect...");
        WiFi.reconnect();
        delay(5000);
    }

    // Daten vom Wechselrichter lesen und veröffentlichen
    static unsigned long lastReadTime = 0;
    if (millis() - lastReadTime >= 10000) { // Alle 10 Sekunden
        lastReadTime = millis();
        readAndPublishInverterData();
        queryAndPublishEnergyData(); // Jetzt auch die Energieerträge regelmäßig abrufen
    }

    // MQTT-Loop für stabile Verbindung
    handleMQTT();
}
