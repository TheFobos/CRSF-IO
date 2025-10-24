#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "config.h"
#include "libs/SerialPort.h"

// Простой тест UART: отправляет текстовые строки через заданный порт с заданной скоростью
// Использование:
//   ./uart_test               -> по умолчанию CRSF_PORT_PRIMARY и CRSF_BAUD
//   ./uart_test /dev/ttyS0    -> указать порт, скорость по умолчанию CRSF_BAUD
//   ./uart_test /dev/ttyS0 115200 "hello" -> указать порт, скорость и сообщение

int main(int argc, char** argv) {
    std::string dev = CRSF_PORT_PRIMARY;
    uint32_t baud = CRSF_BAUD;
    std::string msg = "UART TEST: привет от Raspberry Pi!";

    if (argc >= 2) dev = argv[1];
    if (argc >= 3) baud = static_cast<uint32_t>(std::stoul(argv[2]));
    if (argc >= 4) msg = argv[3];

    std::cout << "[INFO] Открываю порт: " << dev << " @ " << baud << " бод" << std::endl;
    SerialPort sp(dev, baud);
    if (!sp.open()) {
        std::cerr << "[ERROR] Не удалось открыть порт." << std::endl;
        return 1;
    }

    // Отправляем каждые 500 мс несколько строк
    for (int i = 0; i < 20; ++i) {
        std::string line = msg + " [" + std::to_string(i) + "]\r\n";
        int w = sp.write(reinterpret_cast<const uint8_t*>(line.data()), line.size());
        if (w < 0) {
            std::cerr << "[ERROR] Ошибка записи в UART" << std::endl;
            return 2;
        }
        std::cout << "[INFO] Отправлено " << w << " байт" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "[INFO] Готово." << std::endl;
    return 0;
}


