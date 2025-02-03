#include "web.h"
#include "inverter.h"
#include "mqtt.h"
#include "wifi_manager.h"
#include <Arduino.h>

AsyncWebServer server(80); 

void saveMQTTConfig(const char* server, int port, const char* user, const char* password);

// Webserver-Handler für die Hauptseite
void handleWebInterface() {
    InverterData data = getInverterData();
    float feedInPower = getCurrentFeedInPower();

    String html = "<html><body>";
    html += "<h1>Wechselrichter Status</h1>";
    html += "<table border='1'><tr><th>Parameter</th><th>Wert</th></tr>";
    html += "<tr><td>Netzspannung (V)</td><td>" + String(data.gridVoltage) + "</td></tr>";
    html += "<tr><td>Netzfrequenz (Hz)</td><td>" + String(data.gridFrequency) + "</td></tr>";
    html += "<tr><td>Netzleistung (W)</td><td>" + String(data.gridPower) + "</td></tr>";
    html += "<tr><td>Netzstrom (A)</td><td>" + String(data.gridCurrent) + "</td></tr>";
    html += "<tr><td>Ausgangsspannung (V)</td><td>" + String(data.outputVoltage) + "</td></tr>";
    html += "<tr><td>Ausgangsfrequenz (Hz)</td><td>" + String(data.outputFrequency) + "</td></tr>";
    html += "<tr><td>Ausgangsleistung (W)</td><td>" + String(data.outputPower) + "</td></tr>";
    html += "<tr><td>Ausgangsstrom (A)</td><td>" + String(data.outputCurrent) + "</td></tr>";
    html += "<tr><td>Aktuelle Einspeiseleistung (W)</td><td>" + String(feedInPower) + "</td></tr>";
    html += "</table>";

    html += "<br><a href='/config'>Zur Konfigurationsseite</a>";
    html += "</body></html>";

    server.on("/", HTTP_GET, [html](AsyncWebServerRequest *request) {
        request->send(200, "text/html", html);
    });
}

// Webserver-Handler für die Konfigurationsseite
void setupWebInterface() {
    server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = "<html><body>";
        html += "<h1>Konfiguration</h1>";
        html += "<form action='/config' method='POST'>";
        html += "<h2>WLAN-Einstellungen</h2>";
        html += "SSID: <input type='text' name='ssid'><br>";
        html += "Passwort: <input type='password' name='password'><br><br>";
        html += "<h2>MQTT-Einstellungen</h2>";
        html += "MQTT Server: <input type='text' name='mqtt_server'><br>";
        html += "MQTT Port: <input type='number' name='mqtt_port'><br>";
        html += "MQTT Benutzer: <input type='text' name='mqtt_user'><br>";
        html += "MQTT Passwort: <input type='password' name='mqtt_password'><br><br>";
        html += "<input type='submit' value='Speichern'>";
        html += "</form>";
        html += "<br><a href='/'>Zurück zur Hauptseite</a>";
        html += "</body></html>";
        request->send(200, "text/html", html);
    });

    server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("ssid", true) && request->hasParam("password", true) &&
            request->hasParam("mqtt_server", true) && request->hasParam("mqtt_port", true) &&
            request->hasParam("mqtt_user", true) && request->hasParam("mqtt_password", true)) {

            String ssid = request->getParam("ssid", true)->value();
            String password = request->getParam("password", true)->value();
            String mqtt_server = request->getParam("mqtt_server", true)->value();
            int mqtt_port = request->getParam("mqtt_port", true)->value().toInt();
            String mqtt_user = request->getParam("mqtt_user", true)->value();
            String mqtt_password = request->getParam("mqtt_password", true)->value();

            //saveWiFiConfig(ssid.c_str(), password.c_str(), "Inverter_Control");

            saveMQTTConfig(mqtt_server.c_str(), mqtt_port, mqtt_user.c_str(), mqtt_password.c_str());

            request->send(200, "text/html", "Einstellungen gespeichert. <a href='/'>Zurück zur Hauptseite</a>");
        } else {
            request->send(400, "text/html", "Fehlende Parameter!");
        }
    });
}
