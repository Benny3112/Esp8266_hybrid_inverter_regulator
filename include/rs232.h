#ifndef RS232_H
#define RS232_H

#include <SoftwareSerial.h>

// RS232-Pins für ESP8266
#define RS232_RX D5
#define RS232_TX D6

// Globale SoftwareSerial Instanz
extern SoftwareSerial rs232;  

void setupRS232();
void handleRS232();

#endif
