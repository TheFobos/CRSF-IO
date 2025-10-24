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
  "workMode": "joystick",
  "autoMode": false,
  "autoStep": 100,
  "autoInterval": 1000
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
curl "http://localhost:8080/api/command?cmd=setMode&value=joystick"
curl "http://localhost:8080/api/command?cmd=setMode&value=manual"
curl "http://localhost:8080/api/command?cmd=setMode&value=auto"
```

#### Установка канала
```bash
curl "http://localhost:8080/api/command?cmd=setChannel&value=1=1500"
curl "http://localhost:8080/api/command?cmd=setChannel&value=2=2000"
```

#### Настройка автоматического режима
```bash
curl "http://localhost:8080/api/command?cmd=setAutoStep&value=100"
curl "http://localhost:8080/api/command?cmd=setAutoInterval&value=1000"
```

## Режимы работы

### 1. joystick - Передача джойстика
- Автоматическая передача данных с джойстика
- 4 оси: Roll, Pitch, Throttle, Yaw
- Преобразование в CRSF формат (1000-2000 мкс)

### 2. manual - Ручная установка
- Установка значений каналов вручную через API
- Выбор канала (1-16) и значения (1000-2000)

### 3. auto - Автоматическая отправка
- Автоматическая отправка значений с заданным шагом
- Настраиваемый интервал времени (100-10000 мс)

## Примеры использования

### Получение телеметрии
```bash
# Получить данные телеметрии
curl http://localhost:8080/api/telemetry

# Сохранить в файл
curl http://localhost:8080/api/telemetry > telemetry.json
```

### Управление каналами
```bash
# Установить канал 1 в 1500 мкс
curl "http://localhost:8080/api/command?cmd=setChannel&value=1=1500"

# Установить канал 2 в 2000 мкс
curl "http://localhost:8080/api/command?cmd=setChannel&value=2=2000"
```

### Переключение режимов
```bash
# Включить режим джойстика
curl "http://localhost:8080/api/command?cmd=setMode&value=joystick"

# Включить ручной режим
curl "http://localhost:8080/api/command?cmd=setMode&value=manual"

# Включить автоматический режим
curl "http://localhost:8080/api/command?cmd=setMode&value=auto"
```

## Тестирование

Запустите тестовый сервер:
```bash
python3 test_api.py
```

Проверьте API:
```bash
curl http://localhost:8080/api/telemetry
curl "http://localhost:8080/api/command?cmd=setMode&value=manual"
```

## Порт

По умолчанию: **8080**

Можно изменить в коде:
```cpp
startTelemetryServer((CrsfSerial*)crsfGetActive(), 8080); // изменить порт
```
