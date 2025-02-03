#ifndef WEB_H
#define WEB_H

#include <ESPAsyncWebServer.h>
#include "inverter.h"

extern AsyncWebServer server;

void setupWebInterface();
void handleWebInterface();
InverterData getInverterData(); 
float getCurrentFeedInPower();

#endif
