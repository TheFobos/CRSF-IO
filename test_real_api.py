#!/usr/bin/env python3
"""
Тестовый скрипт для проверки реального CRSF API
"""

import requests
import json
import time

def test_api():
    base_url = "http://localhost:8080"
    
    print("🧪 Тестирование CRSF API...")
    
    # Тест 1: Получение телеметрии
    print("\n1️⃣ Получение телеметрии...")
    try:
        response = requests.get(f"{base_url}/api/telemetry", timeout=5)
        if response.status_code == 200:
            data = response.json()
            print(f"✅ Статус связи: {'Подключено' if data['linkUp'] else 'Отключено'}")
            print(f"📡 Активный порт: {data['activePort']}")
            print(f"⏰ Время: {data['timestamp']}")
            print(f"🎮 Каналы: {data['channels'][:4]}... (показаны первые 4)")
            print(f"🔋 Батарея: {data['battery']['voltage']:.1f}В, {data['battery']['current']:.0f}мА")
            print(f"✈️ Положение: Roll={data['attitude']['roll']:.1f}°, Pitch={data['attitude']['pitch']:.1f}°")
        else:
            print(f"❌ Ошибка: {response.status_code}")
    except Exception as e:
        print(f"❌ Ошибка подключения: {e}")
        return
    
    # Тест 2: Команды управления
    print("\n2️⃣ Тестирование команд управления...")
    
    # Установка режима
    try:
        response = requests.get(f"{base_url}/api/command?cmd=setMode&value=manual", timeout=5)
        if response.status_code == 200:
            print("✅ Режим установлен: manual")
        else:
            print(f"❌ Ошибка установки режима: {response.status_code}")
    except Exception as e:
        print(f"❌ Ошибка команды режима: {e}")
    
    # Установка канала
    try:
        response = requests.get(f"{base_url}/api/command?cmd=setChannel&value=1=1500", timeout=5)
        if response.status_code == 200:
            print("✅ Канал 1 установлен: 1500")
        else:
            print(f"❌ Ошибка установки канала: {response.status_code}")
    except Exception as e:
        print(f"❌ Ошибка команды канала: {e}")
    
    # Тест 3: Мониторинг в реальном времени
    print("\n3️⃣ Мониторинг в реальном времени (5 секунд)...")
    for i in range(5):
        try:
            response = requests.get(f"{base_url}/api/telemetry", timeout=2)
            if response.status_code == 200:
                data = response.json()
                print(f"⏱️ {data['timestamp']} - Каналы: {data['channels'][0]}, {data['channels'][1]}, {data['channels'][2]}, {data['channels'][3]}")
            time.sleep(1)
        except Exception as e:
            print(f"❌ Ошибка мониторинга: {e}")
            break
    
    print("\n✅ Тестирование завершено!")

if __name__ == "__main__":
    test_api()
