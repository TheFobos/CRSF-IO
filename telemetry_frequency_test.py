#!/usr/bin/env python3
"""
Скрипт для измерения реальной частоты изменения телеметрии CRSF
Анализирует изменения данных attitude, battery, flight_mode в реальном времени
"""

import requests
import time
import json
from datetime import datetime
import statistics

class TelemetryFrequencyAnalyzer:
    def __init__(self, api_url="http://localhost:8081/api/telemetry"):
        self.api_url = api_url
        self.last_data = None
        self.changes_log = []
        self.start_time = None
        
    def get_telemetry_data(self):
        """Получить данные телеметрии с сервера"""
        try:
            response = requests.get(self.api_url, timeout=1)
            if response.status_code == 200:
                return response.json()
            else:
                print(f"❌ Ошибка HTTP: {response.status_code}")
                return None
        except requests.exceptions.RequestException as e:
            print(f"❌ Ошибка запроса: {e}")
            return None
    
    def has_data_changed(self, current_data, last_data):
        """Проверить, изменились ли данные телеметрии"""
        if last_data is None:
            return True
            
        # Проверяем изменения в attitude
        current_attitude = current_data.get('attitude', {})
        last_attitude = last_data.get('attitude', {})
        
        attitude_changed = (
            abs(current_attitude.get('roll', 0) - last_attitude.get('roll', 0)) > 0.1 or
            abs(current_attitude.get('pitch', 0) - last_attitude.get('pitch', 0)) > 0.1 or
            abs(current_attitude.get('yaw', 0) - last_attitude.get('yaw', 0)) > 0.1
        )
        
        # Проверяем изменения в battery
        current_battery = current_data.get('battery', {})
        last_battery = last_data.get('battery', {})
        
        battery_changed = (
            abs(current_battery.get('voltage', 0) - last_battery.get('voltage', 0)) > 0.01 or
            abs(current_battery.get('current', 0) - last_battery.get('current', 0)) > 1.0 or
            current_battery.get('remaining', 0) != last_battery.get('remaining', 0)
        )
        
        # Проверяем изменения в flight_mode
        flight_mode_changed = (
            current_data.get('workMode', '') != last_data.get('workMode', '')
        )
        
        # Проверяем изменения в каналах
        current_channels = current_data.get('channels', [])
        last_channels = last_data.get('channels', [])
        
        channels_changed = False
        if len(current_channels) == len(last_channels):
            for i in range(min(len(current_channels), 4)):  # Проверяем первые 4 канала
                if abs(current_channels[i] - last_channels[i]) > 5:  # Изменение больше 5 мкс
                    channels_changed = True
                    break
        
        return attitude_changed or battery_changed or flight_mode_changed or channels_changed
    
    def log_change(self, data_type, old_value, new_value):
        """Записать изменение данных"""
        timestamp = time.time()
        if self.start_time is None:
            self.start_time = timestamp
            
        elapsed = timestamp - self.start_time
        
        change_record = {
            'timestamp': timestamp,
            'elapsed': elapsed,
            'type': data_type,
            'old_value': old_value,
            'new_value': new_value
        }
        
        self.changes_log.append(change_record)
        
        # Выводим изменение в реальном времени
        print(f"[{elapsed:6.1f}s] 📊 {data_type}: {old_value} → {new_value}")
    
    def analyze_frequency(self):
        """Анализировать частоту изменений"""
        if len(self.changes_log) < 2:
            print("❌ Недостаточно данных для анализа")
            return
            
        # Вычисляем интервалы между изменениями
        intervals = []
        for i in range(1, len(self.changes_log)):
            interval = self.changes_log[i]['timestamp'] - self.changes_log[i-1]['timestamp']
            intervals.append(interval)
        
        if not intervals:
            print("❌ Нет интервалов для анализа")
            return
            
        # Статистика
        avg_interval = statistics.mean(intervals)
        min_interval = min(intervals)
        max_interval = max(intervals)
        median_interval = statistics.median(intervals)
        
        # Частота
        avg_frequency = 1.0 / avg_interval if avg_interval > 0 else 0
        max_frequency = 1.0 / min_interval if min_interval > 0 else 0
        
        print("\n" + "="*60)
        print("📈 АНАЛИЗ ЧАСТОТЫ ИЗМЕНЕНИЙ ТЕЛЕМЕТРИИ")
        print("="*60)
        print(f"📊 Всего изменений: {len(self.changes_log)}")
        print(f"⏱️  Время измерения: {self.changes_log[-1]['elapsed']:.1f} секунд")
        print(f"📈 Средний интервал: {avg_interval*1000:.1f} мс")
        print(f"📉 Минимальный интервал: {min_interval*1000:.1f} мс")
        print(f"📊 Максимальный интервал: {max_interval*1000:.1f} мс")
        print(f"📊 Медианный интервал: {median_interval*1000:.1f} мс")
        print(f"🔄 Средняя частота: {avg_frequency:.1f} Гц")
        print(f"⚡ Максимальная частота: {max_frequency:.1f} Гц")
        
        # Анализ по типам данных
        type_counts = {}
        for change in self.changes_log:
            data_type = change['type']
            type_counts[data_type] = type_counts.get(data_type, 0) + 1
            
        print(f"\n📋 Изменения по типам:")
        for data_type, count in type_counts.items():
            percentage = (count / len(self.changes_log)) * 100
            print(f"   {data_type}: {count} ({percentage:.1f}%)")
    
    def run_test(self, duration_seconds=30):
        """Запустить тест на указанное время"""
        print(f"🚀 Запуск теста частоты телеметрии на {duration_seconds} секунд")
        print("📱 Двигайте полетником для генерации изменений...")
        print("⏹️  Нажмите Ctrl+C для остановки\n")
        
        try:
            start_time = time.time()
            last_check_time = start_time
            
            while time.time() - start_time < duration_seconds:
                current_data = self.get_telemetry_data()
                
                if current_data is None:
                    time.sleep(0.1)
                    continue
                
                # Проверяем изменения каждые 10мс (100 Гц)
                current_time = time.time()
                if current_time - last_check_time >= 0.01:  # 10мс
                    last_check_time = current_time
                    
                    if self.has_data_changed(current_data, self.last_data):
                        # Детально анализируем что изменилось
                        if self.last_data is not None:
                            # Attitude
                            current_attitude = current_data.get('attitude', {})
                            last_attitude = self.last_data.get('attitude', {})
                            
                            for axis in ['roll', 'pitch', 'yaw']:
                                current_val = current_attitude.get(axis, 0)
                                last_val = last_attitude.get(axis, 0)
                                if abs(current_val - last_val) > 0.1:
                                    self.log_change(f"ATTITUDE_{axis.upper()}", 
                                                   f"{last_val:.2f}°", 
                                                   f"{current_val:.2f}°")
                            
                            # Battery
                            current_battery = current_data.get('battery', {})
                            last_battery = self.last_data.get('battery', {})
                            
                            voltage_change = abs(current_battery.get('voltage', 0) - last_battery.get('voltage', 0))
                            if voltage_change > 0.01:
                                self.log_change("BATTERY_VOLTAGE", 
                                               f"{last_battery.get('voltage', 0):.2f}V", 
                                               f"{current_battery.get('voltage', 0):.2f}V")
                            
                            # Channels
                            current_channels = current_data.get('channels', [])
                            last_channels = self.last_data.get('channels', [])
                            
                            for i in range(min(len(current_channels), 4)):
                                if abs(current_channels[i] - last_channels[i]) > 5:
                                    self.log_change(f"CHANNEL_{i+1}", 
                                                   f"{last_channels[i]}", 
                                                   f"{current_channels[i]}")
                        
                        self.last_data = current_data.copy()
                    else:
                        # Небольшая задержка если нет изменений
                        time.sleep(0.001)  # 1мс
                
        except KeyboardInterrupt:
            print("\n⏹️  Тест остановлен пользователем")
        
        # Анализ результатов
        self.analyze_frequency()

if __name__ == "__main__":
    analyzer = TelemetryFrequencyAnalyzer()
    
    print("🔧 CRSF Телеметрия - Анализ частоты изменений")
    print("=" * 50)
    
    # Проверяем доступность API
    test_data = analyzer.get_telemetry_data()
    if test_data is None:
        print("❌ Не удается подключиться к API серверу")
        print("💡 Убедитесь, что crsf_io_rpi запущен и веб-сервер работает на порту 8081")
        exit(1)
    
    print("✅ API сервер доступен")
    print(f"📊 Текущие данные: {json.dumps(test_data, indent=2)}")
    print()
    
    # Запускаем тест
    analyzer.run_test(duration_seconds=30)
