# CRSF API - Реальная телеметрия

## Описание

API сервер для получения реальной телеметрии CRSF и управления режимами работы. Данные получаются напрямую из CRSF протокола, а не генерируются.

## Запуск

### Тестовый режим (сгенерированные данные)
```bash
python3 test_api.py
```

### Полный режим с реальным CRSF
```bash
sudo ./crsf_io_rpi
```

**Важно:** В полном режиме API получает реальные данные из CRSF протокола:
- Реальные RC каналы от полетного контроллера
- Реальные данные батареи (если передаются)
- Реальные GPS координаты (если передаются)
- Реальные данные положения (если передаются)
- Реальная статистика связи

## API Endpoints

### GET /api/telemetry
Возвращает JSON с данными телеметрии:

```json
{
  "linkUp": true,
  "activePort": "UART Active",
  "lastReceive": 1761338846650,
  "timestamp": "21:47:26",
  "channels": [1500, 1500, 1000, 1500, ...],
  "packetsReceived": 232,
  "packetsSent": 50,
  "packetsLost": 5,
  "gps": {
    "latitude": 55.7556,
    "longitude": 37.6144,
    "altitude": 78.95,
    "speed": 38.46
  },
  "battery": {
    "voltage": 12.90,
    "current": 1429.93,
    "capacity": 5297.45,
    "remaining": 22
  },
  "attitude": {
    "roll": 3.79,
    "pitch": 14.90,
    "yaw": 317.67
  },
  "workMode": "joystick"
}
```

### Источники данных

- **channels**: Реальные RC каналы от полетного контроллера (1000-2000 мкс)
- **packetsReceived/Sent/Lost**: Статистика связи из CRSF протокола
- **gps**: GPS координаты, высота и скорость (если передаются полетным контроллером)
- **battery**: Данные батареи (напряжение в В, ток в мА, емкость в мАч, остаток в %)
- **attitude**: Положение (крен, тангаж, рысканье в градусах)
- **linkUp**: Статус связи с полетным контроллером

### GET /api/command
Команды управления:

#### Установка режима работы
```bash
curl "http://localhost:8081/api/command?cmd=setMode&value=joystick"
curl "http://localhost:8081/api/command?cmd=setMode&value=manual"
```

#### Установка канала (только в ручном режиме)
```bash
curl "http://localhost:8081/api/command?cmd=setChannel&value=1=1500"
curl "http://localhost:8081/api/command?cmd=setChannel&value=2=2000"
curl "http://localhost:8081/api/command?cmd=setChannel&value=3=1000"
curl "http://localhost:8081/api/command?cmd=setChannel&value=4=1800"
```

## Режимы работы

### 1. joystick - Передача джойстика (по умолчанию)
- Автоматическая передача данных с джойстика
- 4 оси: Roll, Pitch, Throttle, Yaw
- Преобразование в CRSF формат (1000-2000 мкс)
- Джойстик опрашивается с частотой ~100 Гц

### 2. manual - Ручная установка
- Установка значений каналов вручную через API
- Выбор канала (1-16) и значения (1000-2000 мкс)
- Каналы сохраняются до следующего изменения
- Отправка с частотой ~100 Гц

## Примеры использования

### Получение телеметрии
```bash
# Получить данные телеметрии
curl http://localhost:8081/api/telemetry

# Сохранить в файл
curl http://localhost:8081/api/telemetry > telemetry.json
```

### Управление каналами в ручном режиме
```bash
# Сначала переключаемся в ручной режим
curl "http://localhost:8081/api/command?cmd=setMode&value=manual"

# Установить канал 1 (Roll) в 1500 мкс
curl "http://localhost:8081/api/command?cmd=setChannel&value=1=1500"

# Установить канал 2 (Pitch) в 2000 мкс
curl "http://localhost:8081/api/command?cmd=setChannel&value=2=2000"

# Установить канал 3 (Throttle) в 1000 мкс
curl "http://localhost:8081/api/command?cmd=setChannel&value=3=1000"

# Установить канал 4 (Yaw) в 1800 мкс
curl "http://localhost:8081/api/command?cmd=setChannel&value=4=1800"
```

### Переключение режимов
```bash
# Включить режим джойстика
curl "http://localhost:8081/api/command?cmd=setMode&value=joystick"

# Включить ручной режим
curl "http://localhost:8081/api/command?cmd=setMode&value=manual"
```

## Тестирование

Запустите тестовый сервер:
```bash
python3 test_api.py
```

Проверьте API:
```bash
curl http://localhost:8081/api/telemetry
curl "http://localhost:8081/api/command?cmd=setMode&value=manual"
```

## Порт

По умолчанию: **8081**

Можно изменить в коде:
```cpp
startTelemetryServer((CrsfSerial*)crsfGetActive(), 8081, 10); // порт, интервал обновления (мс)
```

## Частота обновления

- **Телеметрия**: 100 Гц (10 мс)
- **Отправка RC каналов**: 100 Гц (10 мс)
- **Опрос джойстика**: ~100 Гц
