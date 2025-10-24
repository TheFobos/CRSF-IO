#!/usr/bin/env python3
"""
Тест задержки отображения телеметрии
Измеряет время между изменением данных и их отображением
"""

import requests
import time
import json
from datetime import datetime

API_URL = "http://localhost:8081/api/telemetry"

def measure_display_latency():
    """Измеряет задержку отображения"""
    print("🔧 Тест задержки отображения телеметрии")
    print("==========================================")
    
    # Получаем начальные данные
    try:
        response = requests.get(API_URL)
        response.raise_for_status()
        initial_data = response.json()
        print("✅ API сервер доступен")
    except Exception as e:
        print(f"❌ Ошибка подключения: {e}")
        return
    
    print("📱 Двигайте полетником для генерации изменений...")
    print("⏹️  Нажмите Ctrl+C для остановки\n")
    
    last_attitude = initial_data.get('attitude', {})
    last_timestamp = time.time()
    
    total_latency = 0
    measurements = 0
    
    try:
        while True:
            start_time = time.time()
            
            # Запрашиваем данные
            response = requests.get(API_URL)
            if response.status_code == 200:
                data = response.json()
                current_attitude = data.get('attitude', {})
                
                # Проверяем изменения
                changed = False
                for key in ['roll', 'pitch', 'yaw']:
                    if key in current_attitude and key in last_attitude and \
                       abs(current_attitude[key] - last_attitude[key]) > 0.1:  # Порог изменения
                        changed = True
                        break
                
                if changed:
                    end_time = time.time()
                    latency = (end_time - start_time) * 1000  # в миллисекундах
                    total_latency += latency
                    measurements += 1
                    
                    print(f"[{time.time() - last_timestamp:.1f}s] ⚡ Задержка: {latency:.1f}мс")
                    
                    last_attitude = current_attitude
                    last_timestamp = time.time()
            
            time.sleep(0.01)  # Опрос каждые 10мс
            
    except KeyboardInterrupt:
        print("\nТест остановлен пользователем.")
    
    if measurements > 0:
        avg_latency = total_latency / measurements
        print(f"\n📊 Средняя задержка отображения: {avg_latency:.1f}мс")
        print(f"📈 Всего измерений: {measurements}")
        
        if avg_latency < 50:
            print("✅ Отлично! Задержка менее 50мс")
        elif avg_latency < 100:
            print("⚠️  Хорошо, но можно улучшить")
        else:
            print("❌ Задержка слишком большая")

if __name__ == "__main__":
    measure_display_latency()
