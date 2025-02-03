#ifndef INVERTER_H
#define INVERTER_H

#include "rs232.h"
#include "mqtt.h"
#include "Arduino.h"

// Datenstruktur für Wechselrichter-Daten
struct InverterData {
    float gridVoltage = 0.0;
    float gridFrequency = 0.0;
    float gridPower = 0.0;
    float gridCurrent = 0.0;
    float outputVoltage = 0.0;
    float outputFrequency = 0.0;
    float outputPower = 0.0;
    float outputCurrent = 0.0;
    float outputLoadPercent = 0.0;
    float batteryVoltage = 0.0;
    float batteryCapacity = 0.0;
    float pvVoltage = 0.0;
    float pvPower = 0.0;
};

// Funktionserklärungen
String calculateChecksum(String command);
InverterData parseQPIGSResponse(String response);
InverterData queryQPIGS();
void readAndPublishInverterData();
String queryEnergy(String command);
void queryAndPublishEnergyData();
float parseFeedInPower(String response);
String formatFeedInPower(float power);
String calculateCRC16(String command);
InverterData getInverterData(); 
float getCurrentFeedInPower();


#endif
