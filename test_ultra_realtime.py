#!/usr/bin/env python3
"""
Ультра-реалтайм тест CRSF API
Максимальная скорость обновления
"""

import requests
import time
import threading
import queue

def ultra_realtime_test():
    base_url = "http://localhost:8080"
    
    print("🚀 УЛЬТРА-РЕАЛТАЙМ ТЕСТ CRSF API")
    print("=" * 60)
    
    # Очередь для сбора данных
    data_queue = queue.Queue()
    stop_flag = threading.Event()
    
    def data_collector():
        """Поток для сбора данных с максимальной скоростью"""
        request_count = 0
        start_time = time.time()
        
        while not stop_flag.is_set():
            try:
                response = requests.get(f"{base_url}/api/telemetry", timeout=0.5)
                if response.status_code == 200:
                    data = response.json()
                    current_time = time.time()
                    elapsed = (current_time - start_time) * 1000
                    
                    data_queue.put({
                        'timestamp': current_time,
                        'elapsed': elapsed,
                        'request_count': request_count,
                        'data': data
                    })
                    
                    request_count += 1
                else:
                    print(f"❌ HTTP {response.status_code}")
                    break
            except Exception as e:
                print(f"❌ Ошибка: {e}")
                break
        
        print(f"📊 Собрано {request_count} запросов")
    
    # Запускаем сбор данных
    collector_thread = threading.Thread(target=data_collector)
    collector_thread.start()
    
    print("🔄 Сбор данных в течение 3 секунд...")
    time.sleep(3)
    
    # Останавливаем сбор
    stop_flag.set()
    collector_thread.join()
    
    # Анализируем результаты
    results = []
    while not data_queue.empty():
        results.append(data_queue.get())
    
    if len(results) > 1:
        # Вычисляем интервалы
        intervals = []
        for i in range(1, len(results)):
            interval = (results[i]['timestamp'] - results[i-1]['timestamp']) * 1000
            intervals.append(interval)
        
        # Статистика
        total_time = results[-1]['elapsed'] - results[0]['elapsed']
        requests_per_second = len(results) / (total_time / 1000)
        avg_interval = sum(intervals) / len(intervals)
        min_interval = min(intervals)
        max_interval = max(intervals)
        
        print(f"\n📈 РЕЗУЛЬТАТЫ УЛЬТРА-РЕАЛТАЙМА:")
        print(f"   Всего запросов: {len(results)}")
        print(f"   Время: {total_time:.1f}мс")
        print(f"   Запросов/сек: {requests_per_second:.1f}")
        print(f"   Средний интервал: {avg_interval:.1f}мс")
        print(f"   Минимальный: {min_interval:.1f}мс")
        print(f"   Максимальный: {max_interval:.1f}мс")
        print(f"   Частота: {1000/avg_interval:.1f} Гц")
        
        # Показываем последние данные
        if results:
            last_data = results[-1]['data']
            print(f"\n📊 Последние данные:")
            print(f"   Время: {last_data['timestamp']}")
            print(f"   Каналы: {last_data['channels'][:4]}...")
            print(f"   Положение: Roll={last_data['attitude']['roll']:.1f}°")
        
        # Оценка
        if requests_per_second >= 15:
            print("🏆 ЭКСТРЕМАЛЬНЫЙ РЕАЛТАЙМ!")
        elif requests_per_second >= 10:
            print("⚡ ОТЛИЧНЫЙ РЕАЛТАЙМ!")
        elif requests_per_second >= 5:
            print("✅ ХОРОШИЙ РЕАЛТАЙМ")
        else:
            print("⚠️  НУЖНА ОПТИМИЗАЦИЯ")
    
    print("\n🎯 Ультра-тест завершен!")

if __name__ == "__main__":
    ultra_realtime_test()
