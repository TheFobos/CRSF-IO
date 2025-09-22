#pragma once

// Обёртка над POSIX UART для Linux (Raspberry Pi)
// Поддерживает нестандартную скорость 420000 бод.

#include <string>
#include <cstdint>

class SerialLinux
{
public:
    // Открыть порт, например "/dev/ttyAMA0" или "/dev/ttyUSB0"
    // baud может быть 420000 для CRSF
    bool open(const std::string& device, uint32_t baud);
    void close();

    // Неблокирующее чтение одного байта, возвращает -1 если нет данных
    int readByte();
    // Запись буфера
    ssize_t writeBytes(const uint8_t* data, size_t len);
    // Запись одного байта
    void writeByte(uint8_t b);

    // Доступно ли для чтения (получение количества)
    int available();

    // Текущее время в миллисекундах с начала процесса
    static uint32_t millis();

private:
    int _fd = -1;
};


