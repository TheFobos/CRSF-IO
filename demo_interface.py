#!/usr/bin/env python3
"""
Демонстрационный скрипт для тестирования CRSF Realtime Interface
"""

import subprocess
import sys
import time
import os

def main():
    """Демонстрация интерфейса"""
    print("""
🎯 CRSF Realtime Interface - Демонстрация
========================================

Этот скрипт покажет как использовать интерфейс для мониторинга CRSF данных.

📋 Шаги:

1. Запуск тестового API сервера
2. Запуск интерфейса
3. Демонстрация возможностей

🚀 Начинаем...
""")
    
    # Проверяем наличие файлов
    required_files = [
        "test_api.py",
        "crsf_realtime_interface.py"
    ]
    
    for file in required_files:
        if not os.path.exists(file):
            print(f"❌ Файл {file} не найден")
            return
    
    print("✅ Все необходимые файлы найдены")
    
    # Запускаем API сервер
    print("\n1️⃣ Запуск тестового API сервера...")
    api_process = subprocess.Popen([sys.executable, "test_api.py"])
    
    # Ждем запуска
    print("⏳ Ожидание запуска API сервера...")
    time.sleep(3)
    
    print("✅ API сервер запущен на http://localhost:8080")
    
    # Запускаем интерфейс
    print("\n2️⃣ Запуск интерфейса...")
    print("💡 Интерфейс откроется в новом окне")
    print("📊 Вы увидите:")
    print("   - Статус подключения")
    print("   - RC каналы с прогресс барами")
    print("   - GPS данные")
    print("   - Данные батареи")
    print("   - Положение (Roll/Pitch/Yaw)")
    print("   - Статистику пакетов")
    
    print("\n🎮 Попробуйте:")
    print("   - Изменить интервал обновления")
    print("   - Переключить режимы работы")
    print("   - Просмотреть разные вкладки телеметрии")
    
    print("\n🛑 Для остановки закройте интерфейс")
    
    try:
        # Запускаем интерфейс
        subprocess.run([sys.executable, "crsf_realtime_interface.py"])
        
    except KeyboardInterrupt:
        print("\n🛑 Остановка по Ctrl+C...")
    
    finally:
        # Останавливаем API сервер
        print("\n3️⃣ Остановка API сервера...")
        api_process.terminate()
        api_process.wait()
        print("✅ Демонстрация завершена")

if __name__ == "__main__":
    main()
