# Inverter Control 🌟

## 🔌 Project Overview
This project provides a comprehensive control solution for inverters using RS232, MQTT, and a web interface. It allows real-time monitoring and dynamic adjustment of feed-in power based on grid load data. Configuration is easily managed via the web interface.

## 🔹 Features
1. **Real-Time Data Retrieval:** 
   - Fetch grid voltage, frequency, power, battery status, and PV data from the inverter.
2. **Feed-In Control:** 
   - Dynamically adjust feed-in power based on current grid load.
3. **Web Interface:** 
   - Display current inverter data.
   - Configuration page for modifying Wi-Fi and MQTT settings.
4. **MQTT Integration:** 
   - Publish inverter and energy yield data to MQTT topics.
5. **Filesystem (LittleFS):** 
   - Store configuration data (Wi-Fi and MQTT) in flash memory.
6. **Automatic Wi-Fi Mode:** 
   - Switches to AP mode if the known Wi-Fi network is not reachable.

## 🚀 Planned Extensions
1. **Modbus, Modbus TCP, and MQTT Integration:**
   - Integration with any power meter to capture grid load data.
   - This feature is planned but not yet implemented.
2. **Automatic Protocol Detection:**
   - The application will automatically detect the required protocol (PI16, PI17, or PI30) for the inverter during boot. This feature is currently under development.

# Wechselrichter Steuerung 🌟

## 🔌 Projektübersicht
Dieses Projekt ist eine umfassende Steuerungslösung für Wechselrichter über RS232, MQTT und eine Weboberfläche. Es ermöglicht die Echtzeitüberwachung und Anpassung der Einspeiseleistung basierend auf Netzlastdaten. Die Konfiguration erfolgt bequem über eine Weboberfläche.

## 🔹 Funktionen
1. **Echtzeit-Datenabruf:** 
   - Netzspannung, Frequenz, Leistung, Batteriestatus und PV-Daten vom Wechselrichter.
2. **Einspeisesteuerung:** 
   - Dynamische Anpassung der Einspeiseleistung basierend auf der aktuellen Netzlast.
3. **Weboberfläche:** 
   - Anzeige aktueller Wechselrichterdaten.
   - Konfigurationsseite zur Anpassung von WLAN- und MQTT-Einstellungen.
4. **MQTT-Integration:** 
   - Veröffentlichung von Wechselrichter- und Energieertragsdaten auf MQTT-Themen.
5. **Dateisystem (LittleFS):** 
   - Speichern von Konfigurationsdaten (WLAN und MQTT) im Flash-Speicher.
6. **Automatischer WLAN-Modus:** 
   - Wechselt in den AP-Modus, wenn das bekannte WLAN nicht erreichbar ist.

## 🚀 Geplante Erweiterungen
1. **Modbus, Modbus TCP und MQTT Anbindung:**
   - Anbindung an beliebige Stromzähler zur Erfassung von Netzlastdaten. 
   - Diese Funktion ist geplant, aber noch nicht implementiert.
2. **Automatische Protokollerkennung:**
   - Die Anwendung soll zukünftig beim Booten automatisch erkennen, welches Protokoll (PI16, PI17 oder PI30) vom Wechselrichter benötigt wird. Diese Funktion wird derzeit entwickelt.

---



