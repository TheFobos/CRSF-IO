// Точка входа для Raspberry Pi 5: чтение CRSF из сериал-порта и печать каналов

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "CrsfClientLinux.h"

int main(int argc, char** argv)
{
    // Использование: ./crsf_rpi /dev/ttyAMA0
    std::string device = "/dev/ttyAMA0";
    if (argc > 1)
        device = argv[1];

    CrsfClientLinux crsf;
    if (!crsf.begin(device))
    {
        std::cerr << "Не удалось открыть порт: " << device << std::endl;
        return 1;
    }

    std::cout << "CRSF открыт на устройстве: " << device << std::endl;

    // Главный цикл: читаем порт, при наличии пакета печатаем каналы
    while (true)
    {
        crsf.loop();

        // Печать каждые ~100 мс упрощённо
        static uint32_t lastPrint = 0;
        uint32_t now = SerialLinux::millis();
        if (now - lastPrint > 100)
        {
            lastPrint = now;
            std::cout << "CH:";
            for (int i = 1; i <= 16; ++i)
            {
                int v = crsf.getChannel(i);
                std::cout << ' ' << std::setw(4) << v;
            }
            std::cout << std::endl;
        }
    }

    return 0;
}


