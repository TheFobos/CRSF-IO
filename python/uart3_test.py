#!/usr/bin/env python3

# Простой тест UART3 на Raspberry Pi
# - Отправляет строки в указанный порт
# - Параллельно читает входящие данные и печатает в консоль
# Использование:
#   python3 uart3_test.py                 # порт по умолчанию /dev/ttyS2 (часто UART3), скорость 115200
#   python3 uart3_test.py /dev/ttyAMA2    # указать устройство
#   python3 uart3_test.py /dev/ttyS2 420000 "hello"  # указать устройство, скорость и сообщение

import sys
import time
import threading

try:
    import serial  # pyserial
except ImportError:
    print("[ERROR] Не установлен pyserial. Установите: pip3 install pyserial")
    sys.exit(1)


def reader_thread(ser: serial.Serial):
    """Фоновое чтение UART и вывод в консоль."""
    while True:
        try:
            data = ser.read(ser.in_waiting or 1)
            if data:
                # Печатаем в hex и как текст
                hex_str = ' '.join(f"{b:02X}" for b in data)
                try:
                    text = data.decode('utf-8', errors='replace')
                except Exception:
                    text = str(data)
                print(f"[RX] {len(data)} байт | HEX: {hex_str} | TXT: {text}")
        except serial.SerialException as e:
            print(f"[ERROR] Ошибка чтения: {e}")
            break
        except KeyboardInterrupt:
            break
        time.sleep(0.005)


def main():
    # Значения по умолчанию для UART3 (часто /dev/ttyS2 или /dev/ttyAMA2 в зависимости от оверлеев)
    dev = "/dev/ttyS2"  # попробуйте также "/dev/ttyAMA2" или проверьте dmesg/ls -l /dev/tty* 
    baud = 115200
    msg = "UART3 TEST: привет от Python!"

    # Парсим аргументы командной строки
    if len(sys.argv) >= 2:
        dev = sys.argv[1]
    if len(sys.argv) >= 3:
        baud = int(sys.argv[2])
    if len(sys.argv) >= 4:
        msg = sys.argv[3]

    print(f"[INFO] Открываю порт: {dev} @ {baud} бод")
    try:
        ser = serial.Serial(
            port=dev,
            baudrate=baud,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=0.05,           # неблокирующее чтение с коротким таймаутом
            xonxoff=False,
            rtscts=False,
            dsrdtr=False,
            write_timeout=0.5,
        )
    except serial.SerialException as e:
        print(f"[ERROR] Не удалось открыть порт: {e}")
        sys.exit(2)

    print("[INFO] Порт открыт. Запускаю поток чтения...")
    t = threading.Thread(target=reader_thread, args=(ser,), daemon=True)
    t.start()

    try:
        # Цикл отправки: каждые 500 мс, 50 сообщений
        for i in range(50):
            line = f"{msg} [{i}]\r\n".encode('utf-8')
            try:
                n = ser.write(line)
                ser.flush()  # дождаться отправки
                print(f"[TX] Отправлено {n} байт")
            except serial.SerialTimeoutException:
                print("[WARN] Таймаут записи")
            except serial.SerialException as e:
                print(f"[ERROR] Ошибка записи: {e}")
                break
            time.sleep(0.5)
    except KeyboardInterrupt:
        pass
    finally:
        print("[INFO] Закрываю порт...")
        try:
            ser.close()
        except Exception:
            pass


if __name__ == "__main__":
    main()


