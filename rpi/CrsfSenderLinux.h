#pragma once

// Отправитель CRSF для Linux: формирует и отправляет пакеты каналов/телеметрии

#include <cstdint>
#include <string>
#include "../libs/crsf/crsf_protocol.h"
#include "../libs/crsf/crc8.h"
#include "SerialLinux.h"

class CrsfSenderLinux
{
public:
    // Открыть порт, например "/dev/ttyAMA0"; по умолчанию 420000 бод
    bool begin(const std::string& device, uint32_t baud = CRSF_BAUDRATE);
    void end();

    // Установить значение канала в микросекундах (1000..2000), ch: 1..16
    void setChannel(unsigned int ch, int value);

    // Отправить пакет CRSF_FRAMETYPE_RC_CHANNELS_PACKED на адрес полётного контроллера
    bool sendChannels();

    // Доступ к линк-флагу (для совместимости)
    bool isLinkUp() const { return _linkUp; }
    void setLinkUp(bool up) { _linkUp = up; }

private:
    SerialLinux _serial;
    Crc8 _crc{0xd5};
    int _channels[CRSF_NUM_CHANNELS]{}; // 1000..2000
    bool _linkUp{true}; // по умолчанию позволяем отправку

    // Преобразование в протокольные значения 11 бит
    static int usToCrsf(int us);
};


