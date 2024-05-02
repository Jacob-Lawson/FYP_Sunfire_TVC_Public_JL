#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WiFi.h>
#include <WebServer.h>

void setupWebServer();
void handleRoot();
void handleToggle();
void handleThumbstick();
void handleTemperature();
void handleResetThumbstick();
void handleCommandControl();
void handleZeroPointHome();
void handleMPU6050Data();

#endif
