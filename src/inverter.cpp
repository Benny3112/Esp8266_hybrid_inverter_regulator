#include <time.h>  // Nutzt die Standard-C-Zeitbibliothek
#include "inverter.h"
#include "rs232.h"
#include "mqtt.h"
#include <Arduino.h>

// Funktion zur Berechnung der Prüfsumme (CRC-HALF Methode)
String calculateChecksum(String command) {
    int checksum = 0;
    for (size_t i = 0; i < command.length(); i++) {
        checksum += command[i];
    }
    checksum &= 0xFF; // Prüfsumme auf 8 Bit begrenzen
    return String(checksum);
}

// Funktion zum Abrufen des aktuellen Datums
void getCurrentDate(int &year, int &month, int &day) {
    time_t now = time(nullptr);    // Aktuelle Zeit abrufen
    struct tm *timeinfo = localtime(&now);  // In lokale Zeit umwandeln

    year = timeinfo->tm_year + 1900;  // Jahr abrufen (ab 1900)
    month = timeinfo->tm_mon + 1;     // Monat abrufen (0-basiert → +1)
    day = timeinfo->tm_mday;          // Tag abrufen
}

// Funktion zum Senden eines Befehls an den Wechselrichter
String sendCommandToInverter(String command, bool addChecksum = true) {
    if (!rs232.availableForWrite()) {
        Serial.println("RS232 Schreibpuffer voll!");
        return "ERROR";
    }

    String fullCommand = command;
    if (addChecksum && command != "QPIGS") {  // Keine Prüfsumme für QPIGS hinzufügen
        fullCommand += calculateChecksum(command);
    }
    
    Serial.println("Sende Befehl: " + fullCommand);
    rs232.println(fullCommand);
    delay(500);

    String response = "";
    while (rs232.available()) {
        char c = rs232.read();
        if (c == '\r' || c == '\n') break;
        response += c;
    }

    if (response.isEmpty()) {
        Serial.println("Keine Antwort vom Wechselrichter!");
        return "ERROR";
    }

    Serial.println("Antwort: " + response);
    return response;
}

// Funktion zum Abfragen der Tages-, Monats- und Jahreserträge
void queryAndPublishEnergyData() {
    int year, month, day;
    getCurrentDate(year, month, day);  // Datum abrufen

    // Befehl mit Prüfsumme berechnen
    String cmdQEY = "QEY" + String(year);
    String cmdQEM = "QEM" + String(year) + (month < 10 ? "0" : "") + String(month);
    String cmdQED = "QED" + String(year) + (month < 10 ? "0" : "") + String(month) + (day < 10 ? "0" : "") + String(day);

    // Checksumme berechnen und anhängen
    cmdQEY += calculateChecksum(cmdQEY);
    cmdQEM += calculateChecksum(cmdQEM);
    cmdQED += calculateChecksum(cmdQED);

    // Senden und Ergebnisse empfangen
    String yearlyEnergy = sendCommandToInverter(cmdQEY);
    String monthlyEnergy = sendCommandToInverter(cmdQEM);
    String dailyEnergy = sendCommandToInverter(cmdQED);

    // Falls Antwort erfolgreich, via MQTT senden
    if (yearlyEnergy != "ERROR") publishMQTT("wechselrichter/energy_year", yearlyEnergy);
    if (monthlyEnergy != "ERROR") publishMQTT("wechselrichter/energy_month", monthlyEnergy);
    if (dailyEnergy != "ERROR") publishMQTT("wechselrichter/energy_day", dailyEnergy);
}

InverterData getInverterData() {
    String response = sendCommandToInverter("QPIGS", false);  // QPIGS ohne Checksumme senden
    if (response.startsWith("(")) {
        return parseQPIGSResponse(response);
    } else {
        Serial.println("Fehler beim Abrufen von QPIGS-Daten!");
        return InverterData();
    }
}

// QPIGS Parser
InverterData parseQPIGSResponse(String response) {
    InverterData data;
    int index = 1; // Überspringe "("

    data.gridVoltage = response.substring(index, response.indexOf(' ', index)).toFloat();
    index = response.indexOf(' ', index) + 1;
    data.gridFrequency = response.substring(index, response.indexOf(' ', index)).toFloat();
    index = response.indexOf(' ', index) + 1;
    data.gridPower = response.substring(index, response.indexOf(' ', index)).toFloat();

    return data;
}

// Globale Variable für Netzlast (wird später durch separate Abfrage aktualisiert)
float currentGridLoad = 0.0;  // Netzlast in Watt

// Funktion zur Aktualisierung der Netzlast (später durch Abfrage füllen)
void updateGridLoad(float gridLoad) {
    currentGridLoad = gridLoad;
    Serial.println("Netzlast aktualisiert: " + String(currentGridLoad) + " W");
}

// Funktion zur Steuerung der Einspeisung
void regulateFeedIn() {
    // Schritt 1: Abfrage der aktuellen Einspeisewerte vom Wechselrichter
    String response = sendCommandToInverter("^P007EMINFO");
    if (response.startsWith("(")) {
        // Parse der Antwort zur Extraktion der aktuellen Einspeiseleistung
        float currentFeedInPower = parseFeedInPower(response);  // Aktuelle Einspeisung

        // Schritt 2: Berechne die gewünschte Einspeiseleistung basierend auf der Netzlast
        float desiredFeedIn = currentGridLoad - currentFeedInPower;

        // Sicherstellen, dass keine negative Einspeisung erfolgt
        if (desiredFeedIn < 0) desiredFeedIn = 0;

        // Schritt 3: Sende den neuen Einspeisewert an den Wechselrichter
        String feedInCommand = "^S026EMINFO" + formatFeedInPower(desiredFeedIn);

        // CRC16-Prüfsumme berechnen und anfügen
        feedInCommand += calculateCRC16(feedInCommand);

        // Sende den Anpassungsbefehl an den Wechselrichter
        sendCommandToInverter(feedInCommand);
        
        Serial.println("Einspeisung angepasst: " + String(desiredFeedIn) + " W");
    } else {
        Serial.println("Fehler beim Abrufen der Einspeisedaten.");
    }
}

// Hilfsfunktion zum Parsen der Einspeiseleistung
float parseFeedInPower(String response) {
    // Extrahiere Einspeiseleistung aus der Antwort
    int startIndex = response.lastIndexOf(',') + 1;
    int endIndex = response.indexOf(')', startIndex);
    return response.substring(startIndex, endIndex).toFloat();
}

// Formatierung des Einspeisewerts im richtigen Format
String formatFeedInPower(float power) {
    char buffer[10];
    sprintf(buffer, "%06.2f", power);  // Format: 6-stellig mit 2 Nachkommastellen
    return String(buffer);
}
// Funktion zum Abrufen der aktuellen Einspeiseleistung
float getCurrentFeedInPower() {
    String response = sendCommandToInverter("^P007EMINFO");  // Befehl zur Abfrage der Einspeisedaten
    if (response.startsWith("(")) {
        return parseFeedInPower(response);  // Verwende die bereits vorhandene Parse-Funktion
    } else {
        Serial.println("Fehler beim Abrufen der Einspeiseleistung.");
        return 0.0;
    }
}

// CRC16-Berechnung (Modbus-Standard)
String calculateCRC16(String data) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < data.length(); i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    char crcStr[5];
    sprintf(crcStr, "%04X", crc);
    return String(crcStr);
}

// MQTT-Daten veröffentlichen
void readAndPublishInverterData() {
    InverterData data = getInverterData();
    publishMQTT("wechselrichter/grid_voltage", String(data.gridVoltage));
    publishMQTT("wechselrichter/grid_frequency", String(data.gridFrequency));
    publishMQTT("wechselrichter/grid_power", String(data.gridPower));
    publishMQTT("wechselrichter/grid_current", String(data.gridCurrent));
    publishMQTT("wechselrichter/output_voltage", String(data.outputVoltage));
    publishMQTT("wechselrichter/output_frequency", String(data.outputFrequency));
    publishMQTT("wechselrichter/output_power", String(data.outputPower));
    publishMQTT("wechselrichter/output_current", String(data.outputCurrent));
    publishMQTT("wechselrichter/output_load_percent", String(data.outputLoadPercent));
    publishMQTT("wechselrichter/battery_voltage", String(data.batteryVoltage));
    publishMQTT("wechselrichter/battery_capacity", String(data.batteryCapacity));
    publishMQTT("wechselrichter/pv_voltage", String(data.pvVoltage));
    publishMQTT("wechselrichter/pv_power", String(data.pvPower));
}


String queryEnergy(String command) {
    String response = sendCommandToInverter(command);
    if (response.startsWith("(")) {
        return response.substring(1, response.length() - 1); // Klammern entfernen
    }
    return "Fehler";
}
