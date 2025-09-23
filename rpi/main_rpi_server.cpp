// RPi server: periodically send CRSF channels

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "CrsfSenderLinux.h"

int main(int argc, char** argv)
{
    std::string device = "/dev/ttyAMA0";
    if (argc > 1)
        device = argv[1];

    CrsfSenderLinux tx;
    if (!tx.begin(device))
    {
        std::cerr << "Не удалось открыть порт: " << device << std::endl;
        return 1;
    }

    std::cout << "CRSF сервер (передача) открыт на: " << device << std::endl;

    // Пример: центр по всем каналам, CH1-CH4 бегают синусоидой-пилообразно
    for (int i = 1; i <= 16; ++i)
        tx.setChannel(i, 1500);

    int t = 0;
    while (true)
    {
        // Генерируем простую пилу для CH1 и CH2
        int v = 1000 + (t % 1001);
        int v2 = 2000 - (t % 1001);
        tx.setChannel(1, v);
        tx.setChannel(2, v2);

        // Отправляем пакет
        tx.sendChannels();

        // 50 Гц
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        t = (t + 10) % 1000;
    }

    return 0;
}


