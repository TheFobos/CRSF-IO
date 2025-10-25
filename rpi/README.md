# Raspberry Pi специфика (rpi/)

Специализированные модули для работы с аппаратурой Raspberry Pi.

## 📁 Структура

```
rpi/
├── CrsfClientLinux.cpp    # CRSF клиент для Linux
├── CrsfClientLinux.h      # Заголовки клиента
├── CrsfSenderLinux.cpp    # CRSF отправитель
├── CrsfSenderLinux.h      # Заголовки отправителя
├── main_rpi_client.cpp    # Клиентское приложение
├── main_rpi_server.cpp    # Серверное приложение  
├── main_rpi.cpp          # Основное приложение
├── SerialLinux.cpp       # Linux UART драйвер
└── SerialLinux.h         # Заголовки UART
```

## 🔧 Компоненты

### CRSF Client (`CrsfClientLinux.cpp`)

Клиентская часть для приема CRSF данных

```cpp
class CrsfClientLinux {
public:
    CrsfClientLinux(const std::string& device, int baud);
    ~CrsfClientLinux();
    
    // Подключение
    bool connect();
    void disconnect();
    bool isConnected() const;
    
    // Чтение данных
    bool readPacket(CrsfPacket& packet);
    TelemetryData getTelemetry() const;
    
    // Статистика
    uint32_t getPacketsReceived() const;
    uint32_t getPacketsLost() const;
    float getLinkQuality() const;
};
```

**Использование:**

```cpp
CrsfClientLinux client("/dev/ttyAMA0", 420000);
if (client.connect()) {
    CrsfPacket packet;
    while (client.readPacket(packet)) {
        // Обработка пакета
        if (packet.type == CRSF_FRAMETYPE_RC_CHANNELS) {
            // Обновление каналов
        }
    }
}
```

### CRSF Sender (`CrsfSenderLinux.cpp`)

Серверная часть для отправки CRSF команд

```cpp
class CrsfSenderLinux {
public:
    CrsfSenderLinux(const std::string& device, int baud);
    ~CrsfSenderLinux();
    
    // Подключение
    bool connect();
    void disconnect();
    bool isConnected() const;
    
    // Отправка
    bool sendChannels(const uint16_t* channels, size_t count);
    bool sendTelemetry(uint8_t type, const uint8_t* data, size_t len);
    
    // Статистика
    uint32_t getPacketsSent() const;
    uint32_t getBytesSent() const;
};
```

**Пример отправки каналов:**

```cpp
CrsfSenderLinux sender("/dev/ttyAMA0", 420000);
if (sender.connect()) {
    uint16_t channels[16] = {1500, 1500, 1000, 1500, /* ... */};
    sender.sendChannels(channels, 16);
}
```

### Linux Serial (`SerialLinux.cpp`)

Низкоуровневый UART драйвер для Linux

```cpp
class SerialLinux {
public:
    SerialLinux(const std::string& device, int baud);
    ~SerialLinux();
    
    // Управление соединением
    bool open();
    void close();
    bool isOpen() const;
    
    // Настройки
    void setBaud(int baud);
    void setTimeout(int timeout_ms);
    void setParity(bool enable, bool odd = false);
    void setStopBits(int bits);
    
    // Передача данных
    int write(const uint8_t* data, size_t len);
    int read(uint8_t* buffer, size_t max_len);
    
    // Статистика
    uint32_t getBytesRead() const;
    uint32_t getBytesWritten() const;
    uint32_t getErrors() const;
};
```

**Конфигурация UART:**

```cpp
SerialLinux uart("/dev/ttyAMA0", 420000);
uart.setTimeout(100);        // 100мс таймаут
uart.setParity(false);       // Без четности
uart.setStopBits(1);         // 1 стоп-бит

if (uart.open()) {
    // UART готов к работе
}
```

## 🚀 Приложения

### Основное приложение (`main_rpi.cpp`)

Главный файл для Raspberry Pi

```cpp
int main() {
    // Инициализация GPIO
    rpi_gpio_init();
    
    // Инициализация PWM
    rpi_pwm_init(0, 0);  // pwmchip0, pwm0
    rpi_pwm_init(0, 1);  // pwmchip0, pwm1
    
    // Инициализация CRSF
    CrsfClientLinux client(CRSF_PORT_PRIMARY, CRSF_BAUD);
    CrsfSenderLinux sender(CRSF_PORT_PRIMARY, CRSF_BAUD);
    
    // Основной цикл
    while (running) {
        // Обработка входящих пакетов
        CrsfPacket packet;
        if (client.readPacket(packet)) {
            processPacket(packet);
        }
        
        // Отправка каналов
        sender.sendChannels(currentChannels, 16);
        
        // Обновление телеметрии
        updateTelemetry();
        
        // Небольшая задержка
        rpi_delay_ms(10);  // 100 Гц
    }
    
    return 0;
}
```

### Клиентское приложение (`main_rpi_client.cpp`)

Только прием данных (для мониторинга)

```cpp
int main() {
    CrsfClientLinux client(CRSF_PORT_PRIMARY, CRSF_BAUD);
    
    if (!client.connect()) {
        std::cerr << "Ошибка подключения к CRSF" << std::endl;
        return 1;
    }
    
    std::cout << "CRSF клиент запущен..." << std::endl;
    
    while (true) {
        CrsfPacket packet;
        if (client.readPacket(packet)) {
            // Вывод информации о пакете
            std::cout << "Получен пакет типа: 0x" 
                      << std::hex << (int)packet.type << std::endl;
        }
        
        // Статистика каждые 5 секунд
        static auto lastStats = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        if (now - lastStats > std::chrono::seconds(5)) {
            std::cout << "Пакетов получено: " << client.getPacketsReceived() 
                      << ", потеряно: " << client.getPacketsLost() << std::endl;
            lastStats = now;
        }
    }
}
```

### Серверное приложение (`main_rpi_server.cpp`)

Только отправка команд (для управления)

```cpp
int main() {
    CrsfSenderLinux sender(CRSF_PORT_PRIMARY, CRSF_BAUD);
    
    if (!sender.connect()) {
        std::cerr << "Ошибка подключения к CRSF" << std::endl;
        return 1;
    }
    
    std::cout << "CRSF сервер запущен..." << std::endl;
    
    // Безопасные значения по умолчанию
    uint16_t channels[16] = {1500, 1500, 1000, 1500};
    
    while (true) {
        // Отправка каналов
        if (!sender.sendChannels(channels, 16)) {
            std::cerr << "Ошибка отправки каналов" << std::endl;
            break;
        }
        
        // Обновление значений (например, от джойстика)
        updateChannelsFromJoystick(channels);
        
        // 100 Гц частота
        rpi_delay_ms(10);
    }
}
```

## ⚙️ Конфигурация Raspberry Pi

### Включение UART

```bash
# Редактировать /boot/config.txt
sudo nano /boot/config.txt

# Добавить/раскомментировать:
enable_uart=1
dtoverlay=disable-bt  # Отключить Bluetooth для освобождения UART
```

### Настройка GPIO

```bash
# Проверка доступности GPIO
ls -la /sys/class/gpio/

# Экспорт пина
echo 17 > /sys/class/gpio/export

# Установка направления (out/in)
echo out > /sys/class/gpio/gpio17/direction

# Установка значения
echo 1 > /sys/class/gpio/gpio17/value
```

### Настройка PWM

```bash
# Поиск PWM чипов
ls /sys/class/pwm/

# Активация PWM канала
echo 0 > /sys/class/pwm/pwmchip0/export

# Настройка периода (1мс = 1000000 нс)
echo 1000000 > /sys/class/pwm/pwmchip0/pwm0/period

# Настройка duty cycle (50% = 500000 нс)
echo 500000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle

# Включение PWM
echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable
```

## 🔧 Компиляция

### Основные цели

```bash
# Компиляция основного приложения
make main_rpi

# Компиляция клиента
make main_rpi_client

# Компиляция сервера  
make main_rpi_server

# Компиляция всех RPi модулей
make rpi
```

### Флаги компиляции

```bash
# В Makefile
CXXFLAGS += -DRPI_BUILD
CXXFLAGS += -DUSE_PWM
CXXFLAGS += -DUSE_GPIO

# Для отладки
CXXFLAGS += -DDEBUG_UART
CXXFLAGS += -DDEBUG_PWM
```

## 🐛 Отладка

### Проверка UART

```bash
# Проверка доступности портов
ls -la /dev/ttyAMA* /dev/ttyS*

# Проверка прав доступа
groups $USER
sudo usermod -a -G dialout $USER

# Тест скорости передачи
sudo stty -F /dev/ttyAMA0 420000
echo "test" | sudo tee /dev/ttyAMA0
```

### Мониторинг трафика

```bash
# Просмотр входящих данных
sudo cat /dev/ttyAMA0 | hexdump -C

# Мониторинг с фильтрацией
sudo cat /dev/ttyAMA0 | hexdump -C | grep "c8"  # CRSF адрес

# Логирование в файл
sudo cat /dev/ttyAMA0 > /tmp/crsf_dump.bin
```

### Проверка GPIO

```bash
# Экспорт всех используемых пинов
for pin in 17 18 19 22 23 24; do
    echo $pin > /sys/class/gpio/export
done

# Проверка состояния
for pin in 17 18 19 22 23 24; do
    echo "GPIO$pin: $(cat /sys/class/gpio/gpio$pin/value)"
done
```

## 📊 Производительность

### Оптимизация UART

```cpp
// Увеличение буферов
struct termios2 tty;
ioctl(fd, TCGETS2, &tty);
tty.c_cflag |= CREAD | CLOCAL;
tty.c_cflag &= ~CSIZE;
tty.c_cflag |= CS8;
tty.c_cflag &= ~PARENB;
tty.c_cflag &= ~CSTOPB;
tty.c_cflag &= ~CRTSCTS;
tty.c_ispeed = tty.c_ospeed = 420000;
ioctl(fd, TCSETS2, &tty);
```

### Приоритеты процессов

```bash
# Установка высокого приоритета
sudo chrt -f 99 ./crsf_io_rpi

# Или через nice
sudo nice -n -20 ./crsf_io_rpi
```

## 🔗 Связанные файлы

- `config.h` - Конфигурация пинов и портов
- `libs/rpi_hal.cpp` - Аппаратно-зависимые функции
- `libs/SerialPort.cpp` - Универсальный UART интерфейс
- `main.cpp` - Основное приложение
