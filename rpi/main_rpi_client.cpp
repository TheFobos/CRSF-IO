// RPi client: receive CRSF and print channels

#include <iostream>
#include <iomanip>
#include <string>
#include "CrsfClientLinux.h"

int main(int argc, char** argv)
{
    std::string device = "/dev/ttyAMA0";
    if (argc > 1)
        device = argv[1];

    CrsfClientLinux crsf;
    if (!crsf.begin(device))
    {
        std::cerr << "Не удалось открыть порт: " << device << std::endl;
        return 1;
    }

    std::cout << "CRSF клиент (приём) открыт на: " << device << std::endl;

    while (true)
    {
        crsf.loop();

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


