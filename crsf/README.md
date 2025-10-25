# CRSF Протокол

Модуль для работы с CRSF (Crossfire) протоколом - стандартом связи между RC передатчиками и полетными контроллерами.

## 📁 Структура

```
crsf/
├── crsf.cpp          # Основная логика CRSF
├── crsf.h            # Заголовочный файл
└── crsf.o            # Скомпилированный объект
```

## 🔧 Функциональность

### Основные возможности

- **Инициализация CRSF** - настройка UART портов
- **Обработка пакетов** - парсинг входящих данных
- **Отправка каналов** - передача RC команд
- **Переключение портов** - автоматический failover
- **Статистика связи** - мониторинг качества

### Поддерживаемые пакеты

- **RC Channels** (0x16) - управляющие каналы
- **Attitude** (0x1E) - данные о положении
- **Battery** (0x08) - информация о батарее  
- **Flight Mode** (0x21) - режим полета
- **Link Statistics** (0x14) - статистика связи

## 🚀 Использование

### Инициализация

```cpp
#include "crsf/crsf.h"

// Инициализация CRSF
crsfInit();

// Получение активного экземпляра
CrsfSerial* crsf = crsfGetActive();
```

### Отправка каналов

```cpp
// Установка значений каналов
crsfSetChannel(1, 1500);  // Roll
crsfSetChannel(2, 1500);  // Pitch
crsfSetChannel(3, 1000);  // Throttle (безопасно!)
crsfSetChannel(4, 1500);  // Yaw

// Отправка пакета
crsfSendChannels();
```

### Получение телеметрии

```cpp
// Проверка статуса связи
bool isConnected = crsfIsLinkUp();

// Получение данных
TelemetryData data = crsfGetTelemetry();
printf("Roll: %.2f°, Pitch: %.2f°, Yaw: %.2f°\n", 
       data.attitude.roll, data.attitude.pitch, data.attitude.yaw);
```

## ⚙️ Конфигурация

### UART порты

```cpp
#define CRSF_PORT_PRIMARY "/dev/ttyAMA0"    // Основной порт
#define CRSF_PORT_SECONDARY "/dev/ttyS0"   // Резервный порт
#define CRSF_BAUD 420000                   // Скорость CRSF
```

### Таймауты

```cpp
#define CRSF_TIMEOUT_MS 30000              // 30 сек до переключения порта
#define CRSF_STABILIZATION_MS 5000         // 5 сек стабилизации
```

## 🔄 Логика переключения портов

Система автоматически переключается между портами при потере связи:

1. **Мониторинг** - отслеживание последнего пакета
2. **Таймаут** - если нет данных > 30 сек
3. **Переключение** - смена на резервный порт
4. **Стабилизация** - ожидание 5 сек перед следующим переключением

## 📊 Структуры данных

### TelemetryData

```cpp
struct TelemetryData {
    // Положение
    struct {
        float roll, pitch, yaw;
    } attitude;
    
    // Батарея
    struct {
        float voltage, current, capacity, remaining;
    } battery;
    
    // Связь
    bool linkUp;
    uint32_t packetsReceived, packetsSent, packetsLost;
    
    // Режим полета
    std::string flightMode;
};
```

### RC Channels

```cpp
// 16 каналов управления (1000-2000 мкс)
uint16_t channels[16] = {
    1500, 1500, 1000, 1500,  // Roll, Pitch, Throttle, Yaw
    0, 0, 0, 0,              // Aux 1-4
    0, 0, 0, 0,              // Aux 5-8  
    0, 0, 0, 0               // Aux 9-12
};
```

## 🐛 Отладка

### Включение логов

```cpp
// В config.h
#define USE_LOG true
```

### Типичные проблемы

1. **Нет связи**:
   - Проверьте подключение UART
   - Убедитесь в правильности портов
   - Проверьте права доступа (`sudo`)

2. **Частые переключения**:
   - Увеличьте `CRSF_TIMEOUT_MS`
   - Проверьте качество соединения

3. **Медленная работа**:
   - Отключите логи (`USE_LOG false`)
   - Проверьте нагрузку системы

## 🔗 Связанные файлы

- `libs/crsf/CrsfSerial.cpp` - низкоуровневая работа с UART
- `libs/crsf/crsf_protocol.h` - определения пакетов CRSF
- `main.cpp` - основной цикл обработки
