#pragma once

#include "libs/crsf/CrsfSerial.h"

// Функция для запуска веб-сервера телеметрии
// crsf - указатель на объект CRSF для получения телеметрии
// port - порт для веб-сервера (по умолчанию 8080)
void startTelemetryServer(CrsfSerial* crsf, int port = 8080);

// Функции для обновления данных телеметрии (вызываются из обработчиков пакетов)
void updateTelemetryBattery(double voltage, int current, int capacity, int remaining);
void updateTelemetryAttitude(double pitch, double roll, double yaw);
void updateTelemetryFlightMode(const std::string& mode);
