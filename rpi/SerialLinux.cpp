#include "SerialLinux.h"

// Реализация Linux UART с поддержкой нестандартных скоростей
// Комментарии на русском для наглядности

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <cstring>

// Настройка нестандартной скорости через termios2 (Linux-specific)
#ifndef BOTHER
#include <sys/ioctl.h>
#include <linux/termios.h>
#endif

static bool setCustomBaud(int fd, uint32_t baud)
{
#ifdef BOTHER
    struct termios2 tio2;
    if (ioctl(fd, TCGETS2, &tio2) != 0)
        return false;
    tio2.c_cflag &= ~CBAUD;
    tio2.c_cflag |= BOTHER;
    tio2.c_ispeed = baud;
    tio2.c_ospeed = baud;
    if (ioctl(fd, TCSETS2, &tio2) != 0)
        return false;
    return true;
#else
    // fallback: попробуем стандартные скорости, если нет поддержки BOTHER
    (void)fd; (void)baud;
    return false;
#endif
}

bool SerialLinux::open(const std::string& device, uint32_t baud)
{
    _fd = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (_fd < 0)
        return false;

    struct termios tio{};
    if (tcgetattr(_fd, &tio) != 0)
        return false;

    cfmakeraw(&tio);
    tio.c_cflag |= (CLOCAL | CREAD);
    tio.c_cflag &= ~CSTOPB;   // 1 стоп-бит
    tio.c_cflag &= ~PARENB;   // без чётности
    tio.c_cflag &= ~CRTSCTS;  // без аппаратного управления потоком

    // Временные стандартные скорости, затем переопределим нестандартную
    cfsetispeed(&tio, B115200);
    cfsetospeed(&tio, B115200);

    if (tcsetattr(_fd, TCSANOW, &tio) != 0)
        return false;

    // Устанавливаем нестандартную скорость, если требуется
    if (baud != 115200)
    {
#ifdef BOTHER
        if (!setCustomBaud(_fd, baud))
            return false;
#else
        return false;
#endif
    }

    return true;
}

void SerialLinux::close()
{
    if (_fd >= 0)
    {
        ::close(_fd);
        _fd = -1;
    }
}

int SerialLinux::readByte()
{
    if (_fd < 0) return -1;
    uint8_t b;
    ssize_t n = ::read(_fd, &b, 1);
    if (n == 1) return (int)b;
    return -1;
}

ssize_t SerialLinux::writeBytes(const uint8_t* data, size_t len)
{
    if (_fd < 0) return -1;
    return ::write(_fd, data, len);
}

void SerialLinux::writeByte(uint8_t b)
{
    writeBytes(&b, 1);
}

int SerialLinux::available()
{
    if (_fd < 0) return 0;
    int bytes = 0;
    if (ioctl(_fd, FIONREAD, &bytes) != 0) return 0;
    return bytes;
}

uint32_t SerialLinux::millis()
{
    timeval tv{};
    gettimeofday(&tv, nullptr);
    return (uint32_t)(tv.tv_sec * 1000ull + tv.tv_usec / 1000ull);
}


