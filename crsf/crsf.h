#ifndef CRSF_CRSF_H
#define CRSF_CRSF_H

#include <Arduino.h>
#include <HardwareTimer.h>
#include "config.h"

// Инициализация приёмника CRSF (настройка портов и колбэков)
void crsfInitRecv();
// Инициализация передатчика CRSF (для телеметрии)
void crsfInitSend();
// Главный обработчик канальных событий/статуса связи (вызывать в loop)
void loop_ch();
// Установка значения канала для отправки (1..16)
void crsfSetChannel(unsigned int ch, int value);
// Отправка пакета с текущими значениями каналов
void crsfSendChannels();
// Отправка пакета телеметрии (датчик батареи)
void crsfTelemetrySend();
// Инициализация аппаратного PWM через таймер (для DEVICE_2)
void PWMinit();
// Инициализация программного/аналогового управления моторами (для DEVICE_1)
void analogInit();
// Инициализация пинов реле/камеры (если PIN_INIT == true)
void pinInit();

#endif
