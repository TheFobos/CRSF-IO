# CRSF-IO-3_2

Raspberry Pi CRSF Protocol Interface для управления коптерами

## Описание

CRSF-IO-3_2 - это система для Raspberry Pi, которая обеспечивает прием и отправку CRSF (Crossfire) протокола, управление через джойстик или API, веб-интерфейс для телеметрии и поддержку различных режимов работы.

## Возможности

- ✅ Прием RC-каналов от полетного контроллера
- ✅ Отправка команд управления
- ✅ Телеметрия в реальном времени (GPS, Батарея, Attitude)
- ✅ Веб-API для интеграции
- ✅ Python GUI интерфейс
- ✅ Поддержка двух режимов работы (Joystick/Manual)
- ✅ Fail-safe защита

## Быстрый старт

### Установка

```bash
git clone <repository_url>
cd CRSF-IO-3_2
make
```

### Запуск

```bash
sudo ./crsf_io_rpi
```

### Python GUI

```bash
python3 crsf_realtime_interface.py
```

## Документация

- **[API Documentation](API_README.md)** - API для управления и телеметрии
- **[Configuration Guide](CONFIG_README.md)** - Настройка и конфигурация
- **[Build Guide](MAKEFILE_README.md)** - Руководство по сборке
- **[Manual Mode Guide](MANUAL_MODE_GUIDE.md)** - Ручной режим управления
- **[Telemetry Documentation](README_telemetry.md)** - Документация по телеметрии

## API

### Телеметрия

```bash
curl http://localhost:8081/api/telemetry
```

### Управление

```bash
# Переключиться в ручной режим
curl "http://localhost:8081/api/command?cmd=setMode&value=manual"

# Установить канал
curl "http://localhost:8081/api/command?cmd=setChannel&value=1=1500"
```

Подробнее: [API_README.md](API_README.md)

## Структура проекта

```
CRSF-IO-3_2/
├── main.cpp              # Главная точка входа
├── config.h              # Конфигурация (см. CONFIG_README.md)
├── Makefile              # Сборка (см. MAKEFILE_README.md)
├── crsf/                 # CRSF модуль
├── libs/                 # Библиотеки
├── telemetry_server.cpp  # Веб-сервер
└── crsf_realtime_interface.py  # Python GUI
```

## Требования

- Raspberry Pi (протестировано на Raspberry Pi 5)
- Linux с поддержкой GPIO и UART
- C++17 компилятор
- Python 3.x

## Лицензия

(Укажите вашу лицензию)

## Поддержка

При возникновении проблем создайте issue в репозитории.
