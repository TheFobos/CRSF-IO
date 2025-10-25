# Makefile и сборка проекта

Система сборки для компиляции CRSF-IO-3_2 проекта с поддержкой различных конфигураций и целей.

## 📁 Структура сборки

```
CRSF-IO-3_2/
├── Makefile              # Основной Makefile
├── main.o               # Скомпилированный main.cpp
├── crsf_io_rpi          # Основное исполняемое приложение
├── crsf_io_rpi_alt      # Альтернативная версия
├── uart_test            # Тестовая утилита UART
├── *.o                  # Объектные файлы
└── libs/                # Библиотеки
    ├── *.o              # Объектные файлы библиотек
    └── crsf/            # CRSF библиотека
        └── *.o          # Объектные файлы CRSF
```

## 🔧 Основные цели

### Компиляция проекта

```bash
# Полная сборка проекта
make

# Очистка и пересборка
make clean && make

# Только основные файлы
make crsf_io_rpi
```

### Тестовые утилиты

```bash
# Тест UART
make uart_test

# Альтернативная версия
make crsf_io_rpi_alt
```

### Очистка

```bash
# Удаление всех объектных файлов
make clean

# Удаление исполняемых файлов
make clean-exec

# Полная очистка
make distclean
```

## ⚙️ Конфигурация сборки

### Переменные окружения

```bash
# Компилятор
export CXX=g++

# Флаги компиляции
export CXXFLAGS="-std=c++17 -Wall -Wextra"

# Отладка
export DEBUG=1

# Релиз
export RELEASE=1
```

### Флаги компиляции

```makefile
# Основные флаги
CXXFLAGS += -std=c++17 -Wall -Wextra -O2
CXXFLAGS += -DRPI_BUILD -DUSE_PWM -DUSE_GPIO

# Отладочные флаги
ifeq ($(DEBUG),1)
    CXXFLAGS += -g -DDEBUG_UART -DDEBUG_PWM -DDEBUG_JOYSTICK
    CXXFLAGS += -DUSE_LOG
endif

# Релизные флаги
ifeq ($(RELEASE),1)
    CXXFLAGS += -O3 -DNDEBUG -DENABLE_COMPILER_OPTIMIZATIONS
endif
```

## 📋 Полный Makefile

```makefile
# ============================================================================
# CRSF-IO-3_2 Makefile
# ============================================================================

# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
CXXFLAGS += -DRPI_BUILD -DUSE_PWM -DUSE_GPIO

# Отладочные флаги
ifeq ($(DEBUG),1)
    CXXFLAGS += -g -DDEBUG_UART -DDEBUG_PWM -DDEBUG_JOYSTICK
    CXXFLAGS += -DUSE_LOG
endif

# Релизные флаги
ifeq ($(RELEASE),1)
    CXXFLAGS += -O3 -DNDEBUG -DENABLE_COMPILER_OPTIMIZATIONS
endif

# Библиотеки
LIBS = -lpthread -ludev

# Директории
SRC_DIR = .
LIBS_DIR = libs
CRSF_DIR = crsf
RPI_DIR = rpi

# Основные файлы
MAIN_SRC = main.cpp
MAIN_OBJ = main.o
MAIN_EXEC = crsf_io_rpi

# Альтернативная версия
ALT_EXEC = crsf_io_rpi_alt

# Тестовые утилиты
UART_TEST_SRC = uart_test.cpp
UART_TEST_OBJ = uart_test.o
UART_TEST_EXEC = uart_test

# Библиотеки
CRSF_SRC = $(CRSF_DIR)/crsf.cpp
CRSF_OBJ = $(CRSF_DIR)/crsf.o

TELEMETRY_SRC = telemetry_server.cpp
TELEMETRY_OBJ = telemetry_server.o

# Библиотеки libs
LIBS_SRCS = $(LIBS_DIR)/joystick.cpp \
            $(LIBS_DIR)/rpi_hal.cpp \
            $(LIBS_DIR)/SerialPort.cpp \
            $(LIBS_DIR)/console_display.o

LIBS_OBJS = $(LIBS_DIR)/joystick.o \
            $(LIBS_DIR)/rpi_hal.o \
            $(LIBS_DIR)/SerialPort.o \
            $(LIBS_DIR)/console_display.o

# CRSF библиотеки
CRSF_LIBS_SRCS = $(LIBS_DIR)/crsf/CrsfSerial.cpp \
                 $(LIBS_DIR)/crsf/crc8.cpp

CRSF_LIBS_OBJS = $(LIBS_DIR)/crsf/CrsfSerial.o \
                 $(LIBS_DIR)/crsf/crc8.o

# RPi специфичные файлы
RPI_SRCS = $(RPI_DIR)/CrsfClientLinux.cpp \
           $(RPI_DIR)/CrsfSenderLinux.cpp \
           $(RPI_DIR)/SerialLinux.cpp

RPI_OBJS = $(RPI_DIR)/CrsfClientLinux.o \
           $(RPI_DIR)/CrsfSenderLinux.o \
           $(RPI_DIR)/SerialLinux.o

# Все объектные файлы
ALL_OBJS = $(MAIN_OBJ) $(CRSF_OBJ) $(TELEMETRY_OBJ) \
           $(LIBS_OBJS) $(CRSF_LIBS_OBJS) $(RPI_OBJS)

# ============================================================================
# ЦЕЛИ ПО УМОЛЧАНИЮ
# ============================================================================

.PHONY: all clean clean-exec distclean help

all: $(MAIN_EXEC) $(ALT_EXEC) $(UART_TEST_EXEC)

# ============================================================================
# ОСНОВНОЕ ПРИЛОЖЕНИЕ
# ============================================================================

$(MAIN_EXEC): $(MAIN_OBJ) $(CRSF_OBJ) $(TELEMETRY_OBJ) $(LIBS_OBJS) $(CRSF_LIBS_OBJS) $(RPI_OBJS)
	@echo "🔗 Линковка основного приложения..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	@echo "✅ $(MAIN_EXEC) готов!"

$(MAIN_OBJ): $(MAIN_SRC) config.h
	@echo "🔨 Компиляция main.cpp..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# ============================================================================
# АЛЬТЕРНАТИВНАЯ ВЕРСИЯ
# ============================================================================

$(ALT_EXEC): $(RPI_DIR)/main_rpi.cpp $(RPI_OBJS) $(CRSF_LIBS_OBJS)
	@echo "🔗 Линковка альтернативной версии..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	@echo "✅ $(ALT_EXEC) готов!"

# ============================================================================
# ТЕСТОВЫЕ УТИЛИТЫ
# ============================================================================

$(UART_TEST_EXEC): $(UART_TEST_OBJ) $(LIBS_DIR)/SerialPort.o
	@echo "🔗 Линковка UART теста..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	@echo "✅ $(UART_TEST_EXEC) готов!"

$(UART_TEST_OBJ): $(UART_TEST_SRC)
	@echo "🔨 Компиляция uart_test.cpp..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# ============================================================================
# CRSF МОДУЛЬ
# ============================================================================

$(CRSF_OBJ): $(CRSF_SRC) $(CRSF_DIR)/crsf.h
	@echo "🔨 Компиляция crsf.cpp..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# ============================================================================
# ТЕЛЕМЕТРИЯ
# ============================================================================

$(TELEMETRY_OBJ): $(TELEMETRY_SRC) telemetry_server.h
	@echo "🔨 Компиляция telemetry_server.cpp..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# ============================================================================
# БИБЛИОТЕКИ LIBS
# ============================================================================

$(LIBS_DIR)/joystick.o: $(LIBS_DIR)/joystick.cpp $(LIBS_DIR)/joystick.h
	@echo "🔨 Компиляция joystick.cpp..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(LIBS_DIR)/rpi_hal.o: $(LIBS_DIR)/rpi_hal.cpp $(LIBS_DIR)/rpi_hal.h
	@echo "🔨 Компиляция rpi_hal.cpp..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(LIBS_DIR)/SerialPort.o: $(LIBS_DIR)/SerialPort.cpp $(LIBS_DIR)/SerialPort.h
	@echo "🔨 Компиляция SerialPort.cpp..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# ============================================================================
# CRSF БИБЛИОТЕКИ
# ============================================================================

$(LIBS_DIR)/crsf/CrsfSerial.o: $(LIBS_DIR)/crsf/CrsfSerial.cpp $(LIBS_DIR)/crsf/CrsfSerial.h
	@echo "🔨 Компиляция CrsfSerial.cpp..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(LIBS_DIR)/crsf/crc8.o: $(LIBS_DIR)/crsf/crc8.cpp $(LIBS_DIR)/crsf/crc8.h
	@echo "🔨 Компиляция crc8.cpp..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# ============================================================================
# RPI СПЕЦИФИЧНЫЕ ФАЙЛЫ
# ============================================================================

$(RPI_DIR)/CrsfClientLinux.o: $(RPI_DIR)/CrsfClientLinux.cpp $(RPI_DIR)/CrsfClientLinux.h
	@echo "🔨 Компиляция CrsfClientLinux.cpp..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(RPI_DIR)/CrsfSenderLinux.o: $(RPI_DIR)/CrsfSenderLinux.cpp $(RPI_DIR)/CrsfSenderLinux.h
	@echo "🔨 Компиляция CrsfSenderLinux.cpp..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(RPI_DIR)/SerialLinux.o: $(RPI_DIR)/SerialLinux.cpp $(RPI_DIR)/SerialLinux.h
	@echo "🔨 Компиляция SerialLinux.cpp..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# ============================================================================
# ОЧИСТКА
# ============================================================================

clean:
	@echo "🧹 Очистка объектных файлов..."
	rm -f $(ALL_OBJS) $(UART_TEST_OBJ)
	@echo "✅ Очистка завершена!"

clean-exec:
	@echo "🧹 Очистка исполняемых файлов..."
	rm -f $(MAIN_EXEC) $(ALT_EXEC) $(UART_TEST_EXEC)
	@echo "✅ Исполняемые файлы удалены!"

distclean: clean clean-exec
	@echo "🧹 Полная очистка..."
	rm -f *.o *~ core
	@echo "✅ Полная очистка завершена!"

# ============================================================================
# СПЕЦИАЛЬНЫЕ ЦЕЛИ
# ============================================================================

# Только библиотеки
libs: $(LIBS_OBJS) $(CRSF_LIBS_OBJS)
	@echo "✅ Библиотеки готовы!"

# Только CRSF
crsf: $(CRSF_OBJ) $(CRSF_LIBS_OBJS)
	@echo "✅ CRSF модуль готов!"

# Только RPi
rpi: $(RPI_OBJS)
	@echo "✅ RPi модули готовы!"

# Проверка зависимостей
check-deps:
	@echo "🔍 Проверка зависимостей..."
	@which $(CXX) > /dev/null || (echo "❌ Компилятор $(CXX) не найден!" && exit 1)
	@pkg-config --exists libudev || (echo "❌ libudev не найден!" && exit 1)
	@echo "✅ Все зависимости найдены!"

# Установка зависимостей (Ubuntu/Debian)
install-deps:
	@echo "📦 Установка зависимостей..."
	sudo apt-get update
	sudo apt-get install -y build-essential libudev-dev
	@echo "✅ Зависимости установлены!"

# ============================================================================
# СПРАВКА
# ============================================================================

help:
	@echo "🚁 CRSF-IO-3_2 Makefile"
	@echo ""
	@echo "Основные цели:"
	@echo "  make              - Полная сборка проекта"
	@echo "  make crsf_io_rpi  - Основное приложение"
	@echo "  make crsf_io_rpi_alt - Альтернативная версия"
	@echo "  make uart_test    - Тест UART"
	@echo ""
	@echo "Очистка:"
	@echo "  make clean        - Удалить объектные файлы"
	@echo "  make clean-exec   - Удалить исполняемые файлы"
	@echo "  make distclean    - Полная очистка"
	@echo ""
	@echo "Модули:"
	@echo "  make libs         - Только библиотеки"
	@echo "  make crsf         - Только CRSF модуль"
	@echo "  make rpi          - Только RPi модули"
	@echo ""
	@echo "Утилиты:"
	@echo "  make check-deps   - Проверить зависимости"
	@echo "  make install-deps - Установить зависимости"
	@echo "  make help         - Показать эту справку"
	@echo ""
	@echo "Переменные:"
	@echo "  DEBUG=1           - Отладочная сборка"
	@echo "  RELEASE=1         - Релизная сборка"
	@echo ""
	@echo "Примеры:"
	@echo "  make DEBUG=1      - Отладочная сборка"
	@echo "  make RELEASE=1    - Релизная сборка"
	@echo "  make clean && make - Пересборка"
```

## 🚀 Использование

### Базовая сборка

```bash
# Проверка зависимостей
make check-deps

# Установка зависимостей (если нужно)
make install-deps

# Полная сборка
make

# Запуск
sudo ./crsf_io_rpi
```

### Отладочная сборка

```bash
# Сборка с отладкой
make DEBUG=1

# Запуск с отладчиком
sudo gdb ./crsf_io_rpi
```

### Релизная сборка

```bash
# Оптимизированная сборка
make RELEASE=1

# Проверка размера
ls -lh crsf_io_rpi
```

### Тестирование

```bash
# Сборка тестовых утилит
make uart_test

# Тест UART
sudo ./uart_test

# Альтернативная версия
make crsf_io_rpi_alt
sudo ./crsf_io_rpi_alt
```

## 🔧 Настройка сборки

### Изменение компилятора

```bash
# Использование clang
make CXX=clang++

# Использование gcc-9
make CXX=g++-9
```

### Добавление флагов

```bash
# Дополнительные флаги
make CXXFLAGS="-std=c++17 -Wall -Wextra -O2 -march=native"

# Отключение предупреждений
make CXXFLAGS="-std=c++17 -O2 -w"
```

### Кросс-компиляция

```bash
# Для ARM (Raspberry Pi)
make CXX=arm-linux-gnueabihf-g++

# Для другой архитектуры
make CXX=x86_64-linux-gnu-g++
```

## 🐛 Отладка сборки

### Проблемы с зависимостями

```bash
# Проверка компилятора
which g++
g++ --version

# Проверка библиотек
pkg-config --exists libudev
pkg-config --libs libudev

# Установка недостающих пакетов
sudo apt-get install build-essential libudev-dev
```

### Проблемы с правами

```bash
# Проверка прав на файлы
ls -la *.cpp *.h

# Исправление прав
chmod 644 *.cpp *.h
chmod 755 Makefile
```

### Ошибки компиляции

```bash
# Подробный вывод
make VERBOSE=1

# Только ошибки
make 2>&1 | grep -i error

# Проверка синтаксиса
g++ -fsyntax-only main.cpp
```

## 📊 Производительность сборки

### Параллельная сборка

```bash
# Использование всех ядер
make -j$(nproc)

# Ограничение количества потоков
make -j4
```

### Кэширование

```bash
# Сохранение объектных файлов
make && tar czf build-cache.tar.gz *.o libs/*.o crsf/*.o rpi/*.o

# Восстановление кэша
tar xzf build-cache.tar.gz
```

## 🔗 Связанные файлы

- `config.h` - Конфигурация системы
- `main.cpp` - Основной файл
- `telemetry_server.cpp` - Веб-сервер
- `crsf/crsf.cpp` - CRSF модуль
- `libs/` - Библиотеки
- `rpi/` - RPi специфика
