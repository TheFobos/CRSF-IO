# 📚 Документация CRSF-IO-3_2

Полная документация проекта CRSF-IO-3_2 - системы управления дроном через CRSF протокол на Raspberry Pi.

## 📋 Список документации

### 🏠 Основная документация

- **[README.md](README.md)** - Главный README для GitHub
  - Описание проекта и возможностей
  - Быстрый старт и установка
  - Архитектура системы
  - Примеры использования
  - Лицензия и поддержка

### 🔧 Техническая документация

- **[CONFIG_README.md](CONFIG_README.md)** - Конфигурация системы
  - Полное описание config.h
  - Настройка UART, GPIO, PWM
  - Профили конфигурации (отладка, производительность, безопасность)
  - Примеры настройки

- **[MAKEFILE_README.md](MAKEFILE_README.md)** - Система сборки
  - Описание Makefile
  - Цели компиляции и очистки
  - Отладочная и релизная сборка
  - Кросс-компиляция

### 📡 Модули системы

- **[crsf/README.md](crsf/README.md)** - CRSF протокол
  - Описание CRSF модуля
  - API для работы с протоколом
  - Логика переключения портов
  - Структуры данных

- **[libs/README.md](libs/README.md)** - Библиотеки
  - CRSF Serial, Джойстик, RPi HAL
  - UART драйвер, система логирования
  - Примеры использования
  - Отладка и настройка

- **[rpi/README.md](rpi/README.md)** - Raspberry Pi специфика
  - CRSF клиент и отправитель
  - Linux UART драйвер
  - Приложения (основное, клиент, сервер)
  - Настройка GPIO и PWM

### 🌐 Интерфейсы

- **[README_telemetry.md](README_telemetry.md)** - Веб-интерфейс телеметрии
  - Описание веб-интерфейса
  - Технические детали (100 Гц обновление)
  - Устранение неполадок
  - Настройка портов и частоты

- **[web/README.md](web/README.md)** - Веб-интерфейс (детально)
  - HTML, CSS, JavaScript код
  - Создание собственного интерфейса
  - Мобильная версия
  - Кастомизация стилей

- **[python/README.md](python/README.md)** - Python утилиты
  - UART тест, GUI интерфейс, демо
  - Примеры использования API
  - Установка зависимостей
  - Отладка и логирование

### 🔌 API и управление

- **[API_README.md](API_README.md)** - REST API документация
  - Полное описание всех endpoints
  - Примеры команд и ответов
  - Управление режимами и каналами
  - Безопасность и рекомендации

- **[MANUAL_MODE_GUIDE.md](MANUAL_MODE_GUIDE.md)** - Руководство по ручному режиму
  - Подробные инструкции
  - Примеры команд
  - Безопасные последовательности
  - Тестирование каналов

### 🛠️ Утилиты

- **[set_all_channels.sh](set_all_channels.sh)** - Скрипт установки каналов
  - Установка CH1=1100, CH16=1160
  - Остальные каналы в центр (1500)
  - Проверка результатов
  - Возврат к джойстику

## 🚀 Быстрый старт

### 1. Установка и сборка

```bash
# Клонирование и сборка
git clone <repository>
cd CRSF-IO-3_2
make clean && make

# Установка зависимостей (если нужно)
make install-deps
```

### 2. Запуск системы

```bash
# Запуск основного приложения
sudo ./crsf_io_rpi

# Проверка статуса
curl "http://localhost:8081/api/telemetry" | jq
```

### 3. Веб-интерфейс

```bash
# Открыть в браузере
http://localhost:8081
```

### 4. Управление каналами

```bash
# Установка всех каналов (CH1=1100, CH16=1160)
./set_all_channels.sh

# Ручное управление
curl "http://localhost:8081/api/command?cmd=setMode&value=manual"
curl "http://localhost:8081/api/command?cmd=setChannel&value=1=1800"
```

## 📖 Структура чтения документации

### Для новичков

1. **[README.md](README.md)** - Общее понимание проекта
2. **[API_README.md](API_README.md)** - Основы API
3. **[MANUAL_MODE_GUIDE.md](MANUAL_MODE_GUIDE.md)** - Практические примеры

### Для разработчиков

1. **[CONFIG_README.md](CONFIG_README.md)** - Настройка системы
2. **[MAKEFILE_README.md](MAKEFILE_README.md)** - Сборка проекта
3. **[libs/README.md](libs/README.md)** - Библиотеки и API
4. **[crsf/README.md](crsf/README.md)** - CRSF протокол

### Для системных администраторов

1. **[rpi/README.md](rpi/README.md)** - Настройка Raspberry Pi
2. **[README_telemetry.md](README_telemetry.md)** - Веб-сервер
3. **[CONFIG_README.md](CONFIG_README.md)** - Конфигурация безопасности

### Для пользователей интерфейса

1. **[web/README.md](web/README.md)** - Веб-интерфейс
2. **[python/README.md](python/README.md)** - Python утилиты
3. **[MANUAL_MODE_GUIDE.md](MANUAL_MODE_GUIDE.md)** - Управление

## 🔍 Поиск информации

### По функциональности

- **Управление дроном**: [crsf/README.md](crsf/README.md), [API_README.md](API_README.md)
- **Веб-интерфейс**: [README_telemetry.md](README_telemetry.md), [web/README.md](web/README.md)
- **Настройка**: [CONFIG_README.md](CONFIG_README.md), [rpi/README.md](rpi/README.md)
- **Сборка**: [MAKEFILE_README.md](MAKEFILE_README.md)
- **Отладка**: [libs/README.md](libs/README.md), [python/README.md](python/README.md)

### По компонентам

- **CRSF протокол**: [crsf/README.md](crsf/README.md)
- **Библиотеки**: [libs/README.md](libs/README.md)
- **Raspberry Pi**: [rpi/README.md](rpi/README.md)
- **Python утилиты**: [python/README.md](python/README.md)
- **Веб-интерфейс**: [web/README.md](web/README.md)

### По задачам

- **Установка**: [README.md](README.md), [MAKEFILE_README.md](MAKEFILE_README.md)
- **Настройка**: [CONFIG_README.md](CONFIG_README.md)
- **Использование**: [API_README.md](API_README.md), [MANUAL_MODE_GUIDE.md](MANUAL_MODE_GUIDE.md)
- **Отладка**: Все README содержат разделы отладки
- **Разработка**: [MAKEFILE_README.md](MAKEFILE_README.md), [libs/README.md](libs/README.md)

## 📝 Обновление документации

Документация обновляется при изменении кода. Основные файлы для отслеживания:

- **config.h** → [CONFIG_README.md](CONFIG_README.md)
- **Makefile** → [MAKEFILE_README.md](MAKEFILE_README.md)
- **API endpoints** → [API_README.md](API_README.md)
- **Новые модули** → соответствующие README в папках

## 🤝 Вклад в документацию

При добавлении новых функций:

1. Обновите соответствующий README файл
2. Добавьте примеры использования
3. Обновите этот файл со списком документации
4. Проверьте ссылки между документами

---

**📚 Полная документация CRSF-IO-3_2 проекта готова!**

Все файлы содержат подробные описания, примеры кода, инструкции по настройке и отладке. Документация структурирована по уровням сложности и типам задач.
