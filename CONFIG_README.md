# Конфигурация системы

Центральный файл конфигурации для настройки всех параметров CRSF-IO-3_2 системы.

## 📁 Файл конфигурации

**`config.h`** - основной файл конфигурации

```cpp
#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// ОСНОВНЫЕ НАСТРОЙКИ СИСТЕМЫ
// ============================================================================

// Включение/отключение компонентов
#define USE_CRSF_RECV true     // Прием CRSF данных
#define USE_CRSF_SEND true     // Отправка телеметрии
#define USE_LOG false          // Логирование (отключено для производительности)

// ============================================================================
// UART И СВЯЗЬ
// ============================================================================

// Скорости передачи
#define SERIAL_BAUD 115200     // Обычная отладочная скорость
#define CRSF_BAUD 420000      // Скорость CRSF протокола

// Пути к UART портам
#define CRSF_PORT_PRIMARY "/dev/ttyAMA0"    // PL011 UART (основной)
#define CRSF_PORT_SECONDARY "/dev/ttyS0"    // miniUART (резервный)

// Таймауты и интервалы
#define CRSF_TIMEOUT_MS 30000              // 30 сек до переключения порта
#define CRSF_STABILIZATION_MS 5000         // 5 сек стабилизации после переключения
#define CRSF_SEND_PERIOD_MS 10             // 10мс между отправками (100 Гц)

// ============================================================================
// RASPBERRY PI GPIO ПИНЫ
// ============================================================================

// Моторы и сервоприводы
#define motor_1_digital 17    // GPIO17 - направление мотора 1
#define motor_2_digital 27    // GPIO27 - направление мотора 2
#define motor_1_analog  18    // GPIO18 (PWM0) - ШИМ мотора 1
#define motor_2_analog  19    // GPIO19 (PWM1) - ШИМ мотора 2

// PWM настройки
#define PWM_CHIP_M1 0         // pwmchip номер для мотора 1
#define PWM_NUM_M1  0         // номер канала внутри pwmchip
#define PWM_CHIP_M2 0         // pwmchip номер для мотора 2
#define PWM_NUM_M2  1         // номер канала внутри pwmchip

// Дополнительные пины
#define rele_1 22            // GPIO22 - реле 1
#define rele_2 23            // GPIO23 - реле 2
#define camera 24            // GPIO24 - управление камерой

// ============================================================================
// ВЕБ-СЕРВЕР И API
// ============================================================================

// Порт веб-сервера
#define TELEMETRY_PORT 8081  // Порт для телеметрии и API

// Частота обновления
#define TELEMETRY_UPDATE_MS 10  // 10мс обновление телеметрии (100 Гц)

// ============================================================================
// ДЖОЙСТИК
// ============================================================================

// Путь к джойстику
#define JOYSTICK_DEVICE "/dev/input/js0"  // USB джойстик

// Настройки осей
#define JOYSTICK_DEADZONE 0.1             // Мертвая зона осей
#define JOYSTICK_MAX_VALUE 32767          // Максимальное значение оси

// ============================================================================
// RC КАНАЛЫ
// ============================================================================

// Диапазон значений каналов
#define RC_CHANNEL_MIN 1000   // Минимальное значение (мкс)
#define RC_CHANNEL_MAX 2000   // Максимальное значение (мкс)
#define RC_CHANNEL_CENTER 1500 // Центральное значение (мкс)

// Количество каналов
#define RC_CHANNELS_COUNT 16  // Всего каналов

// ============================================================================
// БЕЗОПАСНОСТЬ
// ============================================================================

// Безопасные значения по умолчанию
#define SAFE_THROTTLE_MIN 1000    // Минимальный газ (безопасно)
#define SAFE_THROTTLE_MAX 1200    // Максимальный газ для тестирования
#define SAFE_CHANNEL_CENTER 1500  // Центр для остальных каналов

// Fail-safe настройки
#define FAILSAFE_TIMEOUT_MS 1000  // 1 сек до активации fail-safe
#define FAILSAFE_THROTTLE 1000     // Газ при fail-safe

// ============================================================================
// ОТЛАДКА И ЛОГИРОВАНИЕ
// ============================================================================

// Уровни логирования
#define LOG_LEVEL_INFO  0
#define LOG_LEVEL_WARN  1
#define LOG_LEVEL_ERROR 2

// Макросы для логирования
#ifdef USE_LOG
    #define log_info(msg)   log_message(LOG_LEVEL_INFO, msg)
    #define log_warn(msg)   log_message(LOG_LEVEL_WARN, msg)
    #define log_error(msg)  log_message(LOG_LEVEL_ERROR, msg)
#else
    #define log_info(msg)   // Отключено
    #define log_warn(msg)   // Отключено
    #define log_error(msg)  // Отключено
#endif

// Отладочные флаги
#define DEBUG_UART false      // Отладка UART
#define DEBUG_PWM false       // Отладка PWM
#define DEBUG_JOYSTICK false // Отладка джойстика
#define DEBUG_CRSF false     // Отладка CRSF

// ============================================================================
// ПРОИЗВОДИТЕЛЬНОСТЬ
// ============================================================================

// Приоритеты процессов
#define PROCESS_PRIORITY_HIGH true    // Высокий приоритет для реального времени

// Размеры буферов
#define UART_BUFFER_SIZE 256          // Размер буфера UART
#define TELEMETRY_BUFFER_SIZE 1024    // Размер буфера телеметрии

// Оптимизации
#define ENABLE_COMPILER_OPTIMIZATIONS true  // Включить оптимизации компилятора

// ============================================================================
// СОВМЕСТИМОСТЬ
// ============================================================================

// Версия системы
#define SYSTEM_VERSION_MAJOR 1
#define SYSTEM_VERSION_MINOR 2
#define SYSTEM_VERSION_PATCH 0

// Поддерживаемые протоколы
#define SUPPORT_CRSF_V1 true         // CRSF версия 1
#define SUPPORT_CRSF_V2 false        // CRSF версия 2 (будущее)

// Поддерживаемые платформы
#define SUPPORT_RASPBERRY_PI true    // Raspberry Pi
#define SUPPORT_LINUX true           // Linux системы

#endif // CONFIG_H
```

## ⚙️ Настройка параметров

### UART и связь

```cpp
// Изменение скорости CRSF
#define CRSF_BAUD 250000      // Для совместимости с некоторыми FC

// Изменение портов
#define CRSF_PORT_PRIMARY "/dev/ttyUSB0"    // USB-UART адаптер
#define CRSF_PORT_SECONDARY "/dev/ttyUSB1"  // Второй адаптер

// Настройка таймаутов
#define CRSF_TIMEOUT_MS 60000              // 60 сек для стабильных соединений
#define CRSF_STABILIZATION_MS 10000        // 10 сек стабилизации
```

### GPIO пины

```cpp
// Изменение пинов для моторов
#define motor_1_digital 21    // GPIO21 вместо GPIO17
#define motor_2_digital 20    // GPIO20 вместо GPIO27

// Добавление дополнительных пинов
#define led_status 25         // GPIO25 - светодиод статуса
#define buzzer 26             // GPIO26 - зуммер
```

### Веб-сервер

```cpp
// Изменение порта
#define TELEMETRY_PORT 8080   // Стандартный HTTP порт

// Изменение частоты обновления
#define TELEMETRY_UPDATE_MS 50  // 20 Гц вместо 100 Гц
```

### Джойстик

```cpp
// Изменение устройства
#define JOYSTICK_DEVICE "/dev/input/js1"  // Второй джойстик

// Настройка чувствительности
#define JOYSTICK_DEADZONE 0.05            // Меньшая мертвая зона
#define JOYSTICK_MAX_VALUE 16383          // Для 14-битных осей
```

## 🔧 Компиляция с конфигурацией

### Makefile цели

```bash
# Компиляция с текущей конфигурацией
make clean && make

# Компиляция с отладкой
make DEBUG=1

# Компиляция с оптимизацией
make RELEASE=1
```

### Флаги компиляции

```bash
# В Makefile
ifeq ($(DEBUG),1)
    CXXFLAGS += -DDEBUG_UART -DDEBUG_PWM -DDEBUG_JOYSTICK
    CXXFLAGS += -DUSE_LOG
endif

ifeq ($(RELEASE),1)
    CXXFLAGS += -O3 -DNDEBUG
    CXXFLAGS += -DENABLE_COMPILER_OPTIMIZATIONS
endif
```

## 🐛 Отладка конфигурации

### Проверка настроек

```cpp
// Добавить в main.cpp для проверки конфигурации
void printConfig() {
    printf("=== КОНФИГУРАЦИЯ СИСТЕМЫ ===\n");
    printf("CRSF порт основной: %s\n", CRSF_PORT_PRIMARY);
    printf("CRSF порт резервный: %s\n", CRSF_PORT_SECONDARY);
    printf("CRSF скорость: %d\n", CRSF_BAUD);
    printf("Период отправки: %d мс\n", CRSF_SEND_PERIOD_MS);
    printf("Порт телеметрии: %d\n", TELEMETRY_PORT);
    printf("Обновление телеметрии: %d мс\n", TELEMETRY_UPDATE_MS);
    printf("Мотор 1: GPIO%d (PWM%d)\n", motor_1_digital, motor_1_analog);
    printf("Мотор 2: GPIO%d (PWM%d)\n", motor_2_digital, motor_2_analog);
    printf("Логирование: %s\n", USE_LOG ? "Включено" : "Отключено");
    printf("=============================\n");
}
```

### Валидация конфигурации

```cpp
// Проверка корректности настроек
bool validateConfig() {
    bool valid = true;
    
    // Проверка диапазонов
    if (CRSF_SEND_PERIOD_MS < 1 || CRSF_SEND_PERIOD_MS > 100) {
        printf("ОШИБКА: CRSF_SEND_PERIOD_MS должен быть 1-100 мс\n");
        valid = false;
    }
    
    if (TELEMETRY_UPDATE_MS < 5 || TELEMETRY_UPDATE_MS > 1000) {
        printf("ОШИБКА: TELEMETRY_UPDATE_MS должен быть 5-1000 мс\n");
        valid = false;
    }
    
    // Проверка GPIO пинов
    if (motor_1_digital == motor_2_digital) {
        printf("ОШИБКА: Пины моторов не должны совпадать\n");
        valid = false;
    }
    
    return valid;
}
```

## 📊 Профили конфигурации

### Высокая производительность

```cpp
// config_performance.h
#define USE_LOG false
#define TELEMETRY_UPDATE_MS 5      // 200 Гц
#define CRSF_SEND_PERIOD_MS 5      // 200 Гц
#define ENABLE_COMPILER_OPTIMIZATIONS true
#define PROCESS_PRIORITY_HIGH true
```

### Отладка

```cpp
// config_debug.h
#define USE_LOG true
#define DEBUG_UART true
#define DEBUG_PWM true
#define DEBUG_JOYSTICK true
#define DEBUG_CRSF true
#define TELEMETRY_UPDATE_MS 100    // 10 Гц для отладки
```

### Безопасность

```cpp
// config_safe.h
#define SAFE_THROTTLE_MAX 1100     // Ограниченный газ
#define FAILSAFE_TIMEOUT_MS 500    // Быстрый fail-safe
#define CRSF_TIMEOUT_MS 15000      // Частые проверки связи
```

## 🔗 Связанные файлы

- `main.cpp` - Использует конфигурацию
- `telemetry_server.cpp` - Настройки веб-сервера
- `crsf/crsf.cpp` - Настройки CRSF
- `libs/rpi_hal.cpp` - GPIO и PWM настройки
- `Makefile` - Флаги компиляции
