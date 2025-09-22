#ifndef CONFIG_H
#define CONFIG_H

// Общие флаги компиляции функционала
// USE_CRSF_RECV: включить приём CRSF пакетов (RC управление)
// USE_CRSF_SEND: включить отправку телеметрии CRSF
// USE_LOG: включить вывод отладочной информации (если используется)
#define USE_CRSF_RECV true
#define USE_CRSF_SEND true
#define USE_LOG true

// Конфигурация целевого устройства/платы (выберите одну)
// DEVICE_1: управление через аналоговый PWM (H-бриджи)
// DEVICE_2: управление через таймеры (аппаратный PWM)
#define DEVICE_1 true
#define DEVICE_2 false

// PIN_INIT: инициализация вспомогательных пинов (реле/камера)
#define PIN_INIT false

// Назначение пинов для моторов и исполнительных устройств
#define  motor_1_digital PB8   // Направление/логический уровень для мотора 1
#define  motor_2_digital PB9   // Направление/логический уровень для мотора 2
#define  motor_1_analog  PB6   // ШИМ/аналог для мотора 1
#define  motor_2_analog  PB7   // ШИМ/аналог для мотора 2
#define  rele_1 PB4            // Реле 1
#define  rele_2 PB3            // Реле 2
#define  camera PA15           // Управление камерой (переключение)

// Номера каналов таймера (для DEVICE_2)
#define  timer_ch_motor_1 1
#define  timer_ch_motor_2 2

// Скорости UART
#define SERIAL_BAUD 115200     // Основной лог/диагностика
#define CRSF_BAUD 420000       // Скорость шины CRSF

// Индикаторный светодиод
#define LED              PC13
#define LED_BUILTIN      PC13
#endif
