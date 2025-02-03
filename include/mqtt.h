#ifndef MQTT_H
#define MQTT_H

void setupMQTT();
void saveMQTTConfig(const char* server, int port, const char* user, const char* password);
void handleMQTT();
void publishMQTT(const char* topic, const String& payload);

#endif
