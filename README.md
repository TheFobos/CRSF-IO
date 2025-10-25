# CRSF-IO-3_2

🚁 **Система управления дроном через CRSF протокол на Raspberry Pi**

Полнофункциональная система для управления дроном с поддержкой джойстика, ручного управления через API и телеметрии в реальном времени.

## 📋 Содержание

- [Возможности](#возможности)
- [Архитектура](#архитектура)
- [Быстрый старт](#быстрый-старт)
- [Документация](#документация)
- [API Reference](#api-reference)
- [Конфигурация](#конфигурация)
- [Разработка](#разработка)
- [Лицензия](#лицензия)

## 🚀 Возможности

### ✈️ Управление дроном
- **CRSF протокол** - совместимость с полетными контроллерами Betaflight/iNAV
- **16 RC каналов** - полная поддержка всех каналов управления
- **100 Гц частота** - минимальная задержка для точного управления
- **Двойной UART** - автоматическое переключение между портами при потере связи

### 🎮 Режимы управления
- **Джойстик** - управление через USB джойстик (по умолчанию)
- **Ручной режим** - установка каналов через REST API
- **Автоматическое переключение** - между основным и резервным портами

### 📊 Телеметрия
- **Реальное время** - обновление каждые 10мс (100 Гц)
- **Веб-интерфейс** - мониторинг через браузер
- **REST API** - программный доступ к данным
- **Статистика связи** - пакеты, ошибки, качество сигнала

### 🔧 Технические особенности
- **Raspberry Pi оптимизация** - использование GPIO и PWM
- **Многопоточность** - параллельная обработка данных
- **Fail-safe** - безопасное поведение при потере связи
- **Логирование** - детальная отладка

## 🏗️ Архитектура

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Джойстик      │    │   CRSF-IO-3_2    │    │ Полетный        │
│   (USB HID)     │───▶│   Raspberry Pi   │───▶│ контроллер      │
└─────────────────┘    │                  │    │ (Betaflight)    │
                        │  ┌─────────────┐  │    └─────────────────┘
┌─────────────────┐    │  │ Веб-сервер  │  │
│   REST API      │───▶│  │ :8081        │  │
│   Клиенты       │    │  └─────────────┘  │
└─────────────────┘    └──────────────────┘
```

### Основные компоненты

- **`main.cpp`** - главный процесс, управление джойстиком и CRSF
- **`telemetry_server.cpp`** - веб-сервер телеметрии и API
- **`crsf/`** - модуль CRSF протокола
- **`libs/`** - библиотеки для работы с UART, PWM, джойстиком
- **`rpi/`** - специфичный код для Raspberry Pi

## 🚀 Быстрый старт

### Требования

- **Raspberry Pi** (3B+, 4, 5)
- **Linux** с поддержкой GPIO
- **USB джойстик** (опционально)
- **Полетный контроллер** с CRSF поддержкой

### Установка

```bash
# Клонирование репозитория
git clone https://github.com/your-repo/CRSF-IO-3_2.git
cd CRSF-IO-3_2

# Компиляция
make clean && make

# Запуск (требует sudo для GPIO)
sudo ./crsf_io_rpi
```

### Первое подключение

1. **Подключите полетный контроллер** к UART портам:
   - Основной: `/dev/ttyAMA0` (GPIO 14/15)
   - Резервный: `/dev/ttyS0` (GPIO 8/10)

2. **Откройте веб-интерфейс**: http://localhost:8081

3. **Проверьте статус**: статус-бар должен показывать "Связь: Активна"

## 📚 Документация

### Основные файлы документации

- **[API_README.md](API_README.md)** - Полное описание REST API
- **[README_telemetry.md](README_telemetry.md)** - Веб-интерфейс телеметрии
- **[MANUAL_MODE_GUIDE.md](MANUAL_MODE_GUIDE.md)** - Руководство по ручному режиму
- **[config.h](config.h)** - Конфигурация системы

### Компоненты системы

- **[crsf/README.md](crsf/README.md)** - CRSF протокол и обработка
- **[libs/README.md](libs/README.md)** - Библиотеки и драйверы
- **[rpi/README.md](rpi/README.md)** - Raspberry Pi специфика
- **[python/README.md](python/README.md)** - Python утилиты

## 🔌 API Reference

### Основные endpoints

```bash
# Получение телеметрии
GET /api/telemetry

# Управление режимами
POST /api/command?cmd=setMode&value=joystick|manual

# Установка каналов (только в ручном режиме)
POST /api/command?cmd=setChannel&value=1=1500
```

### Примеры использования

```bash
# Переключение в ручной режим
curl "http://localhost:8081/api/command?cmd=setMode&value=manual"

# Установка каналов управления
curl "http://localhost:8081/api/command?cmd=setChannel&value=1=1800"  # Roll
curl "http://localhost:8081/api/command?cmd=setChannel&value=2=1700"  # Pitch  
curl "http://localhost:8081/api/command?cmd=setChannel&value=3=1000"  # Throttle
curl "http://localhost:8081/api/command?cmd=setChannel&value=4=1500"  # Yaw

# Возврат к джойстику
curl "http://localhost:8081/api/command?cmd=setMode&value=joystick"
```

## ⚙️ Конфигурация

### Основные настройки (`config.h`)

```cpp
#define USE_CRSF_RECV true     // Прием CRSF данных
#define USE_CRSF_SEND true    // Отправка телеметрии
#define USE_LOG false           // Логирование (отключено для производительности)

#define CRSF_BAUD 420000      // Скорость CRSF протокола
#define CRSF_PORT_PRIMARY "/dev/ttyAMA0"    // Основной UART
#define CRSF_PORT_SECONDARY "/dev/ttyS0"             // Резервный UART
```

### GPIO пины (Raspberry Pi)

```cpp
#define motor_1_digital 17   // GPIO17 - направление мотора 1
#define motor_2_digital 27   // GPIO27 - направление мотора 2
#define motor_1_analog  18   // GPIO18 (PWM0) - ШИМ мотора 1
#define motor_2_analog  19   // GPIO19 (PWM1) - ШИМ мотора 2
```

## 🛠️ Разработка

### Структура проекта

```
CRSF-IO-3_2/
├── main.cpp              # Главный файл
├── telemetry_server.cpp  # Веб-сервер
├── config.h              # Конфигурация
├── crsf/              # CRSF протокол
│   ├── crsf.cpp
│   └── crsf.h
├── libs/           # Библиотеки
│   ├── crsf/          # CRSF библиотека
│   ├── joystick.cpp   # Джойстик
│   ├── rpi_hal.cpp  # Raspberry Pi HAL
│   └── SerialPort.cpp # UART
├── rpi/              # RPi специфика
└── python/           # Python утилиты
```

### Компиляция

```bash
# Очистка и сборка
make clean && make

# Только основные файлы
make crsf_io_rpi

# Тестовые утилиты
make uart_test
```

### Отладка

```bash
# Включить логирование
# В config.h: #define USE_LOG true

# Проверка портов
sudo ./uart_test

# Мониторинг телеметрии
curl "http://localhost:8081/api/telemetry" | jq
```

## 🔒 Безопасность

⚠️ **ВАЖНО**: Всегда устанавливайте безопасные значения перед переключением в ручной режим!

### Рекомендуемая последовательность:

1. **Безопасные значения**:
   ```bash
   curl "http://localhost:8081/api/command?cmd=setChannel&value=3=1000"  # Газ на минимум!
   ```

2. **Переключение режима**:
   ```bash
   curl "http://localhost:8081/api/command?cmd=setMode&value=manual"
   ```

3. **Постепенное управление**:
   ```bash
   curl "http://localhost:8081/api/command?cmd=setChannel&value=3=1200"  # Медленно добавляем газ
   ```

## 📄 Лицензия

MIT License - см. [LICENSE](LICENSE)

## 🤝 Вклад в проект

1. Fork репозитория
2. Создайте feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit изменения (`git commit -m 'Add some AmazingFeature'`)
4. Push в branch (`git push origin feature/AmazingFeature`)
5. Откройте Pull Request

## 📞 Поддержка

- **Issues**: [GitHub Issues](https://github.com/your-repo/CRSF-IO-3_2/issues)
- **Discussions**: [GitHub Discussions](https://github.com/your-repo/CRSF-IO-3_2/discussions)
- **Wiki**: [Документация](https://github.com/your-repo/CRSF-IO-3_2/wiki)

## 🏷️ Версии

- **v1.0.0** - Базовая функциональность
- **v1.1.0** - Добавлен ручной режим
- **v1.2.0** - Оптимизация производительности

---

**Создано с ❤️ для сообщества дронов**
