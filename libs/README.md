# Библиотеки (libs/)

Коллекция низкоуровневых библиотек для работы с аппаратурой Raspberry Pi и CRSF протоколом.

## 📁 Структура

```
libs/
├── crsf/                 # CRSF протокол
│   ├── CrsfSerial.cpp    # Сериал коммуникация
│   ├── CrsfSerial.h   # Заголовки
│   ├── crsf_protocol.h         # Определения пакетов
│   └── crc8.cpp         # CRC8 проверка
├── joystick.cpp              # USB джойстик
├── joystick.h                # Заголовки джойстика
├── rpi_hal.cpp           # Raspberry Pi HAL
├── rpi_hal.h              # Заголовки HAL
├── SerialPort.cpp         # UART порт
├── SerialPort.h           # Заголовки UART
└── log.h                   # Система логгирования
```

## 🔧 Компоненты

### CRSF Serial (`libs/crsf/`)

**CrsfSerial.cpp** - Основной класс для работы с CRSF протоколом

```cpp
class CrsfSerial {
public:
    CrsfSerial(SerialPort& port, uint32_t baud);
    
    // Основные методы
    void processPacketIn(uint8_t len);
    void packetAttitude(const crsf_header_t* p);
    void packetBattery(const crsf_header_t* p);
    void packetLinkStats(const crsf_header_t* p);
    
    // Отправка
    void sendChannels(const uint16_t* channels);
    void sendTelemetry(uint8_t type, const uint8_t* data, uint8_t len);
    
    // Статус
    bool isLinkUp() const;
    uint32_t getLastReceive() const;
    TelemetryData getTelemetry() const;
};
```

**crsf_protocol.h** - Определения пакетов CRSF

```cpp
// Типы пакетов
#define CRSF_FRAMETYPE_RC_CHANNELS_PACKED 0x16
#define CRSF_FRAMETYPE_ATTITUDE 0x1E
#define CRSF_FRAMETYPE_BATTERY 0x08
#define CRSF_FRAMETYPE_FLIGHT_MODE 0x21
#define CRSF_FRAMETYPE_LINK_STATISTICS 0x14

// Структуры пакетов
struct crsf_header_t {
    uint8_t device_addr;
    uint8_t frame_size;
    uint8_t type;
    uint8_t data[CRSF_MAX_PAYLOAD];
    uint8_t crc;
};
```

**crc8.cpp** - CRC8 проверка для CRSF

```cpp
uint8_t crc8_calc(uint8_t* ptr, uint8_t len);
```

### Джойстик (`joystick.cpp`)

USB HID джойстик интерфейс

```cpp
class Joystick {
public:
    Joystick();
    ~Joystick();
    
    bool isConnected() const;
    int getAxisCount() const;
    int getButtonCount() const;
    
    // Чтение данных
    float getAxis(int axis) const;     // -1.0 до 1.0
    bool getButton(int button) const;  // true/false
    
    // Обновление
    void update();
};
```

**Использование:**

```cpp
Joystick joy;
if (joy.isConnected()) {
    float roll = joy.getAxis(0);      // Ось X
    float pitch = joy.getAxis(1);     // Ось Y  
    float throttle = joy.getAxis(2);   // Ось Z
    bool arm = joy.getButton(0);      // Кнопка 0
}
```

### Raspberry Pi HAL (`rpi_hal.cpp`)

Аппаратно-зависимые функции

```cpp
// GPIO управление
void rpi_gpio_init();
void rpi_gpio_set(int pin, bool state);
bool rpi_gpio_get(int pin);

// PWM управление  
void rpi_pwm_init(int chip, int channel);
void rpi_pwm_set_duty(int chip, int channel, float duty);
void rpi_pwm_enable(int chip, int channel);

// Время
uint32_t rpi_millis();
void rpi_delay_ms(uint32_t ms);

// UART
int rpi_uart_open(const char* device, int baud);
int rpi_uart_write(int fd, const uint8_t* data, size_t len);
int rpi_uart_read(int fd, uint8_t* buffer, size_t max_len);
```

### Serial Port (`SerialPort.cpp`)

Универсальный UART интерфейс

```cpp
class SerialPort {
public:
    SerialPort(const std::string& path, int baud);
    ~SerialPort();
    
    bool open();
    void close();
    bool isOpen() const;
    
    // Чтение/запись
    int read(uint8_t* buffer, size_t max_len);
    int write(const uint8_t* data, size_t len);
    
    // Настройки
    void setBaud(int baud);
    void setTimeout(int timeout_ms);
};
```

**Пример использования:**

```cpp
SerialPort uart("/dev/ttyAMA0", 420000);
if (uart.open()) {
    uint8_t buffer[256];
    int bytes = uart.read(buffer, sizeof(buffer));
    if (bytes > 0) {
        // Обработка данных
    }
}
```

### Логирование (`log.h`)

Система логирования с временными метками

```cpp
// Уровни логирования
#define LOG_LEVEL_INFO  0
#define LOG_LEVEL_WARN  1  
#define LOG_LEVEL_ERROR 2

// Макросы
#define log_info(msg)   log_message(LOG_LEVEL_INFO, msg)
#define log_warn(msg)   log_message(LOG_LEVEL_WARN, msg)  
#define log_error(msg)  log_message(LOG_LEVEL_ERROR, msg)

// Функции
std::string log_timestamp();
void log_message(int level, const std::string& msg);
```

**Примеры:**

```cpp
log_info("CRSF: получен пакет типа " + std::to_string(type));
log_warn("CRSF: переключение на резервный порт");
log_error("Ошибка открытия UART: " + std::string(strerror(errno)));
```

## ⚙️ Конфигурация

### GPIO пины (config.h)

```cpp
// Моторы
#define motor_1_digital 17   // GPIO17 - направление мотора 1
#define motor_2_digital 27   // GPIO27 - направление мотора 2  
#define motor_1_analog  18   // GPIO18 (PWM0) - ШИМ мотора 1
#define motor_2_analog  19   // GPIO19 (PWM1) - ШИМ мотора 2

// PWM чипы
#define PWM_CHIP_M1 0        // pwmchip номер для мотора 1
#define PWM_NUM_M1  0        // номер канала внутри pwmchip
#define PWM_CHIP_M2 0        // pwmchip номер для мотора 2  
#define PWM_NUM_M2  1        // номер канала внутри pwmchip

// Дополнительные пины
#define rele_1 22            // GPIO22 - реле 1
#define rele_2 23            // GPIO23 - реле 2
#define camera 24            // GPIO24 - управление камерой
```

### UART настройки

```cpp
#define SERIAL_BAUD 115200   // Обычная отладочная скорость
#define CRSF_BAUD 420000     // Скорость CRSF протокола

// Пути к портам
#define CRSF_PORT_PRIMARY "/dev/ttyAMA0"    // PL011 UART
#define CRSF_PORT_SECONDARY "/dev/ttyS0"    // miniUART
```

## 🔧 Компиляция

### Makefile цели

```bash
# Компиляция всех библиотек
make libs

# Только CRSF библиотека  
make libs/crsf/CrsfSerial.o

# Только джойстик
make libs/joystick.o

# Только HAL
make libs/rpi_hal.o
```

### Зависимости

```bash
# Системные библиотеки
sudo apt-get install libudev-dev

# Для разработки
sudo apt-get install build-essential cmake
```

## 🐛 Отладка

### Проверка UART

```bash
# Проверка доступности портов
ls -la /dev/ttyAMA* /dev/ttyS*

# Тест скорости
sudo ./uart_test

# Мониторинг трафика
sudo cat /dev/ttyAMA0 | hexdump -C
```

### Проверка GPIO

```bash
# Экспорт пина
echo 17 > /sys/class/gpio/export

# Установка направления
echo out > /sys/class/gpio/gpio17/direction

# Установка значения
echo 1 > /sys/class/gpio/gpio17/value
```

### Проверка PWM

```bash
# Поиск PWM чипов
ls /sys/class/pwm/

# Активация PWM
echo 0 > /sys/class/pwm/pwmchip0/export
echo 1000000 > /sys/class/pwm/pwmchip0/pwm0/period
echo 500000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle
echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable
```

## 📚 Связанная документация

- [CRSF Protocol](https://github.com/betaflight/betaflight/wiki/CRSF-Protocol)
- [Raspberry Pi GPIO](https://www.raspberrypi.org/documentation/usage/gpio/)
- [Linux UART](https://www.kernel.org/doc/html/latest/driver-api/serial.html)
