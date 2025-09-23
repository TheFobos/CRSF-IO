#include "CrsfSenderLinux.h"

// Простая реализация отправителя CRSF для Linux

bool CrsfSenderLinux::begin(const std::string& device, uint32_t baud)
{
    return _serial.open(device, baud);
}

void CrsfSenderLinux::end()
{
    _serial.close();
}

void CrsfSenderLinux::setChannel(unsigned int ch, int value)
{
    if (ch == 0 || ch > CRSF_NUM_CHANNELS) return;
    if (value < 1000) value = 1000;
    if (value > 2000) value = 2000;
    _channels[ch - 1] = value;
}

int CrsfSenderLinux::usToCrsf(int us)
{
    // Линейное преобразование 1000..2000 -> CRSF_CHANNEL_VALUE_1000..CRSF_CHANNEL_VALUE_2000
    const int inMin = 1000;
    const int inMax = 2000;
    const int outMin = CRSF_CHANNEL_VALUE_1000;
    const int outMax = CRSF_CHANNEL_VALUE_2000;
    long long num = (long long)(us - inMin) * (outMax - outMin);
    int mapped = (int)(outMin + (num / (inMax - inMin)));
    if (mapped < CRSF_CHANNEL_VALUE_MIN) mapped = CRSF_CHANNEL_VALUE_MIN;
    if (mapped > CRSF_CHANNEL_VALUE_MAX) mapped = CRSF_CHANNEL_VALUE_MAX;
    return mapped;
}

bool CrsfSenderLinux::sendChannels()
{
    if (!_linkUp) return false;

    // Сформировать полезную нагрузку 22 байта: 16 каналов по 11 бит
    crsf_channels_t payload{};
    int chv[CRSF_NUM_CHANNELS];
    for (unsigned i = 0; i < CRSF_NUM_CHANNELS; ++i)
        chv[i] = usToCrsf(_channels[i]);

    payload.ch0 = chv[0];
    payload.ch1 = chv[1];
    payload.ch2 = chv[2];
    payload.ch3 = chv[3];
    payload.ch4 = chv[4];
    payload.ch5 = chv[5];
    payload.ch6 = chv[6];
    payload.ch7 = chv[7];
    payload.ch8 = chv[8];
    payload.ch9 = chv[9];
    payload.ch10 = chv[10];
    payload.ch11 = chv[11];
    payload.ch12 = chv[12];
    payload.ch13 = chv[13];
    payload.ch14 = chv[14];
    payload.ch15 = chv[15];

    // Собрать полный пакет: [addr][len][type][payload][crc]
    uint8_t buf[CRSF_MAX_PACKET_SIZE];
    const uint8_t addr = CRSF_ADDRESS_FLIGHT_CONTROLLER;
    const uint8_t type = CRSF_FRAMETYPE_RC_CHANNELS_PACKED;
    const uint8_t payloadLen = CRSF_FRAME_RC_CHANNELS_PAYLOAD_SIZE; // 22
    buf[0] = addr;
    buf[1] = payloadLen + 2; // type + payload + crc
    buf[2] = type;
    std::memcpy(&buf[3], &payload, payloadLen);
    buf[3 + payloadLen] = _crc.calc(&buf[2], payloadLen + 1);

    ssize_t wrote = _serial.writeBytes(buf, payloadLen + 4);
    return wrote == (payloadLen + 4);
}


