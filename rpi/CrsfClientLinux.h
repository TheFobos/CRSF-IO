#pragma once

// Клиент CRSF для Linux, использует SerialLinux и существующие протокол/CRC

#include <cstdint>
#include "../libs/crsf/crsf_protocol.h"
#include "../libs/crsf/crc8.h"
#include "SerialLinux.h"

class CrsfClientLinux
{
public:
    // Инициализация с устройством UART, например "/dev/ttyAMA0"
    bool begin(const std::string& device, uint32_t baud = CRSF_BAUDRATE);
    void end();

    // Вызывать часто в главном цикле
    void loop();

    // Текущее значение канала (1..16) в мкс
    int getChannel(unsigned int ch) const;

private:
    SerialLinux _serial;
    Crc8 _crc{0xd5};
    uint8_t _rxBuf[CRSF_MAX_PACKET_SIZE]{};
    uint8_t _rxPos{0};
    uint32_t _lastReceive{0};
    uint32_t _lastChannels{0};
    bool _linkUp{false};
    int _channels[CRSF_NUM_CHANNELS]{};

    void handleByte(uint8_t b);
    void processPacket(uint8_t len);
    void shiftRx(uint8_t cnt);
    void checkTimeouts();
};


