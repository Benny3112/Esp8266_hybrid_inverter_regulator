#include "rs232.h"
#include <Arduino.h>

// RS232-Instanz für die serielle Kommunikation mit dem Wechselrichter
SoftwareSerial rs232(RS232_RX, RS232_TX);

void setupRS232() {
    rs232.begin(2400);
    Serial.println("RS232 Verbindung gestartet");
}

void handleRS232() {
    static String responseBuffer = ""; // Buffer für serielle Daten

    while (rs232.available()) {
        char c = rs232.read();
        responseBuffer += c;
        Serial.write(c);  // Debug-Ausgabe auf Serial-Monitor

        // Prüfen, ob eine vollständige Antwort empfangen wurde (z. B. durch Zeilenumbruch)
        if (c == '\n' || c == '\r') {
            Serial.println("\nEmpfangene RS232-Antwort: " + responseBuffer);
            responseBuffer = ""; // Buffer zurücksetzen
        }
    }
}
