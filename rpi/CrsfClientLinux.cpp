#include "CrsfClientLinux.h"

// Реализация простого клиента CRSF для Linux на базе SerialLinux

bool CrsfClientLinux::begin(const std::string& device, uint32_t baud)
{
    return _serial.open(device, baud);
}

void CrsfClientLinux::end()
{
    _serial.close();
}

int CrsfClientLinux::getChannel(unsigned int ch) const
{
    if (ch == 0 || ch > CRSF_NUM_CHANNELS) return 0;
    return _channels[ch - 1];
}

void CrsfClientLinux::loop()
{
    // Читаем все доступные байты
    while (_serial.available() > 0)
    {
        int rb = _serial.readByte();
        if (rb < 0) break;
        _lastReceive = SerialLinux::millis();
        uint8_t b = static_cast<uint8_t>(rb);
        _rxBuf[_rxPos++] = b;
        handleByte(b);
        if (_rxPos == CRSF_MAX_PACKET_SIZE)
        {
            _rxPos = 0; // переполнение, сброс
        }
    }

    checkTimeouts();
}

void CrsfClientLinux::handleByte(uint8_t)
{
    // Пытаемся интерпретировать буфер, когда есть хотя бы 2 байта (addr + len)
    bool reprocess;
    do
    {
        reprocess = false;
        if (_rxPos > 1)
        {
            uint8_t len = _rxBuf[1];
            if (len < 3 || len > (CRSF_MAX_PAYLOAD_LEN + 2))
            {
                shiftRx(1);
                reprocess = true;
            }
            else if (_rxPos >= (len + 2))
            {
                uint8_t inCrc = _rxBuf[2 + len - 1];
                uint8_t crc = _crc.calc(&_rxBuf[2], len - 1);
                if (crc == inCrc)
                {
                    processPacket(len);
                    shiftRx(len + 2);
                    reprocess = true;
                }
                else
                {
                    shiftRx(1);
                    reprocess = true;
                }
            }
        }
    } while (reprocess);
}

void CrsfClientLinux::processPacket(uint8_t len)
{
    const crsf_header_t* hdr = (const crsf_header_t*)_rxBuf;
    if (hdr->device_addr == CRSF_ADDRESS_FLIGHT_CONTROLLER)
    {
        switch (hdr->type)
        {
            case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
            {
                const crsf_channels_t* ch = (const crsf_channels_t*)&hdr->data;
                _channels[0] = ch->ch0;
                _channels[1] = ch->ch1;
                _channels[2] = ch->ch2;
                _channels[3] = ch->ch3;
                _channels[4] = ch->ch4;
                _channels[5] = ch->ch5;
                _channels[6] = ch->ch6;
                _channels[7] = ch->ch7;
                _channels[8] = ch->ch8;
                _channels[9] = ch->ch9;
                _channels[10] = ch->ch10;
                _channels[11] = ch->ch11;
                _channels[12] = ch->ch12;
                _channels[13] = ch->ch13;
                _channels[14] = ch->ch14;
                _channels[15] = ch->ch15;

                // Преобразуем к 1000..2000 мкс
                for (unsigned i = 0; i < CRSF_NUM_CHANNELS; ++i)
                {
                    int v = _channels[i];
                    // Линейная интерполяция
                    v = 1000 + (int)((long long)(v - CRSF_CHANNEL_VALUE_1000) * (2000 - 1000) / (CRSF_CHANNEL_VALUE_2000 - CRSF_CHANNEL_VALUE_1000));
                    _channels[i] = v;
                }

                if (!_linkUp)
                    _linkUp = true;
                _lastChannels = SerialLinux::millis();
                break;
            }
            default:
                break;
        }
    }
    (void)len;
}

void CrsfClientLinux::shiftRx(uint8_t cnt)
{
    if (cnt >= _rxPos)
    {
        _rxPos = 0;
        return;
    }
    uint8_t* src = &_rxBuf[cnt];
    uint8_t* dst = &_rxBuf[0];
    _rxPos -= cnt;
    uint8_t left = _rxPos;
    while (left--)
        *dst++ = *src++;
}

void CrsfClientLinux::checkTimeouts()
{
    // Тайм-аут пакета: если давно не приходили байты — сдвигаем по одному
    if (_rxPos > 0 && (SerialLinux::millis() - _lastReceive) > 100)
        shiftRx(1);

    // Фиксация падения линка по отсутствию каналов длительное время
    if (_linkUp && (SerialLinux::millis() - _lastChannels) > 60000)
        _linkUp = false;
}


