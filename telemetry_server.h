#ifndef TELEMETRY_SERVER_H
#define TELEMETRY_SERVER_H

#include "libs/crsf/CrsfSerial.h"

// Запуск веб-сервера телеметрии
void startTelemetryServer(CrsfSerial* crsf, int port = 8080, int updateIntervalMs = 50);

#endif // TELEMETRY_SERVER_H
