# Руководство по ручному режиму

## Переключение режимов

### Режим джойстика (по умолчанию)
```bash
curl "http://localhost:8081/api/command?cmd=setMode&value=joystick"
```
В этом режиме каналы управляются джойстиком автоматически.

### Ручной режим
```bash
curl "http://localhost:8081/api/command?cmd=setMode&value=manual"
```
В этом режиме вы можете устанавливать значения каналов вручную через API.

## Установка каналов в ручном режиме

### Формат команды
```bash
curl "http://localhost:8081/api/command?cmd=setChannel&value=КАНАЛ=ЗНАЧЕНИЕ"
```

### Параметры
- **КАНАЛ**: номер канала от 1 до 16
- **ЗНАЧЕНИЕ**: значение в микросекундах от 1000 до 2000
  - 1000 мкс = минимум
  - 1500 мкс = центр
  - 2000 мкс = максимум

### Примеры

#### Установить все каналы в центр
```bash
curl "http://localhost:8081/api/command?cmd=setMode&value=manual"
curl "http://localhost:8081/api/command?cmd=setChannel&value=1=1500"
curl "http://localhost:8081/api/command?cmd=setChannel&value=2=1500"
curl "http://localhost:8081/api/command?cmd=setChannel&value=3=1500"
curl "http://localhost:8081/api/command?cmd=setChannel&value=4=1500"
```

#### Управление дроном
```bash
# Переключаемся в ручной режим
curl "http://localhost:8081/api/command?cmd=setMode&value=manual"

# Roll (крен) вправо
curl "http://localhost:8081/api/command?cmd=setChannel&value=1=1800"

# Pitch (тангаж) вперед
curl "http://localhost:8081/api/command?cmd=setChannel&value=2=1700"

# Throttle (газ) на 50%
curl "http://localhost:8081/api/command?cmd=setChannel&value=3=1500"

# Yaw (рысканье) влево
curl "http://localhost:8081/api/command?cmd=setChannel&value=4=1300"
```

#### Тестирование каналов
```bash
# Переключаемся в ручной режим
curl "http://localhost:8081/api/command?cmd=setMode&value=manual"

# Тест канала 1: минимум -> центр -> максимум
curl "http://localhost:8081/api/command?cmd=setChannel&value=1=1000"
sleep 1
curl "http://localhost:8081/api/command?cmd=setChannel&value=1=1500"
sleep 1
curl "http://localhost:8081/api/command?cmd=setChannel&value=1=2000"
```

## Проверка текущих значений

```bash
curl "http://localhost:8081/api/telemetry" | python3 -m json.tool
```

Обратите внимание на поля:
- `workMode`: текущий режим ("joystick" или "manual")
- `channels`: массив значений всех 16 каналов

## Возврат к джойстику

```bash
curl "http://localhost:8081/api/command?cmd=setMode&value=joystick"
```

После переключения обратно в режим джойстика, каналы снова будут управляться джойстиком.

## Частота обновления

- Каналы отправляются с частотой **100 Гц** (каждые 10 мс)
- Установленные вручную значения сохраняются до следующего изменения
- Переключение режима происходит мгновенно

## Безопасность

⚠️ **ВАЖНО**: Всегда устанавливайте безопасные значения перед переключением в ручной режим!

Рекомендуется:
1. Сначала установить все каналы в безопасные значения (обычно 1500)
2. Затем переключиться в ручной режим
3. Постепенно изменять значения

```bash
# Безопасная последовательность
curl "http://localhost:8081/api/command?cmd=setChannel&value=1=1500"
curl "http://localhost:8081/api/command?cmd=setChannel&value=2=1500"
curl "http://localhost:8081/api/command?cmd=setChannel&value=3=1000"  # Газ на минимум!
curl "http://localhost:8081/api/command?cmd=setChannel&value=4=1500"
curl "http://localhost:8081/api/command?cmd=setMode&value=manual"
```
