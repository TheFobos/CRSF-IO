#!/usr/bin/env python3
"""
Тест реалтайма CRSF API
Проверяет скорость обновления данных
"""

import requests
import time
import json

def test_realtime_speed():
    base_url = "http://localhost:8080"
    
    print("⚡ Тест реалтайма CRSF API")
    print("=" * 50)
    
    # Получаем начальное время
    start_time = time.time()
    timestamps = []
    
    print("🔄 Получение данных каждые 50мс (20 Гц)...")
    print("⏱️  Время обновления:")
    
    for i in range(20):  # 20 запросов = 1 секунда
        try:
            response = requests.get(f"{base_url}/api/telemetry", timeout=1)
            if response.status_code == 200:
                data = response.json()
                current_time = time.time()
                elapsed = (current_time - start_time) * 1000  # в миллисекундах
                timestamps.append(elapsed)
                
                # Показываем время обновления
                print(f"  {i+1:2d}: {elapsed:6.1f}мс - {data['timestamp']}")
                
                # Небольшая задержка для имитации реального использования
                time.sleep(0.05)  # 50мс
            else:
                print(f"❌ Ошибка: {response.status_code}")
                break
        except Exception as e:
            print(f"❌ Ошибка: {e}")
            break
    
    # Анализ результатов
    if len(timestamps) > 1:
        intervals = []
        for i in range(1, len(timestamps)):
            interval = timestamps[i] - timestamps[i-1]
            intervals.append(interval)
        
        avg_interval = sum(intervals) / len(intervals)
        min_interval = min(intervals)
        max_interval = max(intervals)
        
        print("\n📊 Результаты теста реалтайма:")
        print(f"   Средний интервал: {avg_interval:.1f}мс")
        print(f"   Минимальный: {min_interval:.1f}мс")
        print(f"   Максимальный: {max_interval:.1f}мс")
        print(f"   Частота: {1000/avg_interval:.1f} Гц")
        
        # Оценка производительности
        if avg_interval <= 60:
            print("✅ Отличная производительность реалтайма!")
        elif avg_interval <= 100:
            print("✅ Хорошая производительность реалтайма")
        else:
            print("⚠️  Производительность может быть улучшена")
    
    print("\n🎯 Тест завершен!")

if __name__ == "__main__":
    test_realtime_speed()
