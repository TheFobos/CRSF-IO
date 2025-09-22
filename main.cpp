/*
  ВНИМАНИЕ: Этот файл содержит Arduino-специфичный код и отключён для сборки под Raspberry Pi.
  Код сохранён для справки. Для Raspberry Pi используйте файлы в директории rpi/.

  // Точка входа Arduino-проекта. Настройка и основной цикл.
  #include <arduino.h>
  #include "config.h"
  #include "crsf/crsf.h"

  void setup()
  {
    // Ветка приёма CRSF: готовим PWM и стартуем CRSF приёмник
  #if USE_CRSF_RECV == true
  #if DEVICE_2 == true
    PWMinit();        // Аппаратный PWM на таймерах
  #elif DEVICE_1 == true
    analogInit();     // Программное управление моторами
  #endif
    crsfInitRecv();   // Настройка CRSF портов и колбэков
  #elif USE_CRSF_SEND == true
    // Только отправка телеметрии CRSF (без приёма каналов)
  #else
    // Резервный лог-порт
    Serial.begin(SERIAL_BAUD);
  #endif
  #if PIN_INIT == true
    pinInit();        // Реле/камера
  #endif
  }

  bool isCan = true;                 // Зарезервировано под состояние CAN/канала
  unsigned long previousMillis = 0;  // Для таймера телеметрии
  unsigned long currentMillis;
  unsigned long interval = 200;      // Интервал отправки телеметрии, мс
  #define BUF_SIZE 50                // Резерв
  int t = 1500;                      // Резерв (сервопозиция)
  int inc = 10;                      // Резерв (шаг)

  void loop()
  {
    // Обработка связи/каналов CRSF (приём)
  #if USE_CRSF_RECV == true
    loop_ch();
  #endif

    // Периодическая отправка телеметрии (если включена)
  #if USE_CRSF_SEND == true
    currentMillis = millis();
    if (currentMillis - previousMillis > interval)
    {
      previousMillis = currentMillis;
      crsfTelemetrySend();
    }
  #endif
  }
*/
