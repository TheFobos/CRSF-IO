# Python утилиты

Коллекция Python скриптов для тестирования, отладки и мониторинга CRSF системы.

## 📁 Структура

```
python/
├── uart3_test.py           # Тест UART порта
├── crsf_realtime_interface.py  # GUI интерфейс телеметрии
├── demo_interface.py       # Демо интерфейс
└── README.md              # Эта документация
```

## 🔧 Утилиты

### UART Test (`uart3_test.py`)

Тестирование UART соединения и скорости передачи

```python
#!/usr/bin/env python3
"""
Тест UART порта для CRSF
Проверяет подключение, скорость и качество связи
"""

import serial
import time
import sys

def test_uart_port(port, baudrate=420000):
    """Тестирование UART порта"""
    try:
        # Открытие порта
        ser = serial.Serial(port, baudrate, timeout=1)
        print(f"✅ Порт {port} открыт успешно")
        
        # Тест записи
        test_data = b'\xC8\x16\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
        ser.write(test_data)
        print(f"📤 Отправлено {len(test_data)} байт")
        
        # Тест чтения
        start_time = time.time()
        received = ser.read(100)
        end_time = time.time()
        
        if received:
            print(f"📥 Получено {len(received)} байт за {end_time-start_time:.3f}с")
            print(f"📊 Скорость: {len(received)/(end_time-start_time):.0f} байт/с")
        else:
            print("⚠️ Данные не получены")
            
        ser.close()
        return True
        
    except serial.SerialException as e:
        print(f"❌ Ошибка UART: {e}")
        return False
    except Exception as e:
        print(f"❌ Общая ошибка: {e}")
        return False

if __name__ == "__main__":
    # Тестирование основных портов
    ports = ["/dev/ttyAMA0", "/dev/ttyS0"]
    
    for port in ports:
        print(f"\n🔍 Тестирование {port}...")
        test_uart_port(port)
```

**Использование:**

```bash
# Тест основного порта
python3 python/uart3_test.py

# Тест с пользовательским портом
python3 -c "
import sys
sys.path.append('python')
from uart3_test import test_uart_port
test_uart_port('/dev/ttyUSB0', 115200)
"
```

### CRSF Realtime Interface (`crsf_realtime_interface.py`)

Графический интерфейс для мониторинга телеметрии в реальном времени

```python
#!/usr/bin/env python3
"""
CRSF Realtime Interface
GUI приложение для мониторинга телеметрии
"""

import tkinter as tk
from tkinter import ttk, messagebox
import requests
import json
import threading
import time
from datetime import datetime

class CRSFRealtimeInterface:
    def __init__(self, root):
        self.root = root
        self.root.title("CRSF Realtime Interface")
        self.root.geometry("1200x800")
        
        # Настройки API
        self.api_url = "http://localhost:8081"
        self.update_interval = 20  # мс
        
        # Состояние
        self.is_running = False
        self.current_data = None
        
        # Создание интерфейса
        self.create_interface()
        
        # Запуск обновления
        self.start_data_update()
    
    def create_interface(self):
        """Создание GUI элементов"""
        # Главный фрейм
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # Статус
        self.status_label = ttk.Label(main_frame, text="Подключение...")
        self.status_label.grid(row=0, column=0, columnspan=2, pady=5)
        
        # Данные телеметрии
        self.create_telemetry_frame(main_frame)
        
        # Управление
        self.create_control_frame(main_frame)
    
    def create_telemetry_frame(self, parent):
        """Создание фрейма телеметрии"""
        telemetry_frame = ttk.LabelFrame(parent, text="📊 Телеметрия", padding="10")
        telemetry_frame.grid(row=1, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=5)
        
        # Attitude
        attitude_frame = ttk.Frame(telemetry_frame)
        attitude_frame.grid(row=0, column=0, sticky=(tk.W, tk.E), padx=5)
        
        ttk.Label(attitude_frame, text="Roll:").grid(row=0, column=0, sticky=tk.W)
        self.roll_label = ttk.Label(attitude_frame, text="0.00°")
        self.roll_label.grid(row=0, column=1, sticky=tk.W, padx=5)
        
        ttk.Label(attitude_frame, text="Pitch:").grid(row=1, column=0, sticky=tk.W)
        self.pitch_label = ttk.Label(attitude_frame, text="0.00°")
        self.pitch_label.grid(row=1, column=1, sticky=tk.W, padx=5)
        
        ttk.Label(attitude_frame, text="Yaw:").grid(row=2, column=0, sticky=tk.W)
        self.yaw_label = ttk.Label(attitude_frame, text="0.00°")
        self.yaw_label.grid(row=2, column=1, sticky=tk.W, padx=5)
        
        # Battery
        battery_frame = ttk.Frame(telemetry_frame)
        battery_frame.grid(row=0, column=1, sticky=(tk.W, tk.E), padx=5)
        
        ttk.Label(battery_frame, text="Voltage:").grid(row=0, column=0, sticky=tk.W)
        self.voltage_label = ttk.Label(battery_frame, text="0.00V")
        self.voltage_label.grid(row=0, column=1, sticky=tk.W, padx=5)
        
        ttk.Label(battery_frame, text="Current:").grid(row=1, column=0, sticky=tk.W)
        self.current_label = ttk.Label(battery_frame, text="0mA")
        self.current_label.grid(row=1, column=1, sticky=tk.W, padx=5)
        
        ttk.Label(battery_frame, text="Remaining:").grid(row=2, column=0, sticky=tk.W)
        self.remaining_label = ttk.Label(battery_frame, text="0%")
        self.remaining_label.grid(row=2, column=1, sticky=tk.W, padx=5)
    
    def create_control_frame(self, parent):
        """Создание фрейма управления"""
        control_frame = ttk.LabelFrame(parent, text="🎮 Управление", padding="10")
        control_frame.grid(row=2, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=5)
        
        # Режим работы
        ttk.Label(control_frame, text="Режим:").grid(row=0, column=0, sticky=tk.W)
        self.mode_var = tk.StringVar(value="joystick")
        mode_combo = ttk.Combobox(control_frame, textvariable=self.mode_var, 
                                 values=["joystick", "manual"], state="readonly")
        mode_combo.grid(row=0, column=1, padx=5)
        
        # Кнопка переключения режима
        ttk.Button(control_frame, text="Переключить режим", 
                  command=self.toggle_mode).grid(row=0, column=2, padx=5)
        
        # RC каналы
        self.create_channels_frame(control_frame)
    
    def create_channels_frame(self, parent):
        """Создание фрейма RC каналов"""
        channels_frame = ttk.LabelFrame(parent, text="📡 RC Каналы", padding="5")
        channels_frame.grid(row=1, column=0, columnspan=3, sticky=(tk.W, tk.E), pady=5)
        
        self.channel_labels = []
        for i in range(16):
            row = i // 4
            col = (i % 4) * 2
            
            ttk.Label(channels_frame, text=f"CH{i+1}:").grid(row=row, column=col, sticky=tk.W, padx=2)
            label = ttk.Label(channels_frame, text="1500")
            label.grid(row=row, column=col+1, sticky=tk.W, padx=2)
            self.channel_labels.append(label)
    
    def start_data_update(self):
        """Запуск обновления данных"""
        self.is_running = True
        self.update_thread = threading.Thread(target=self.data_update_worker, daemon=True)
        self.update_thread.start()
        self.update_interface()
    
    def data_update_worker(self):
        """Поток обновления данных"""
        while self.is_running:
            try:
                response = requests.get(f"{self.api_url}/api/telemetry", timeout=2)
                if response.status_code == 200:
                    self.current_data = response.json()
                else:
                    self.current_data = None
            except Exception as e:
                print(f"Ошибка получения данных: {e}")
                self.current_data = None
            
            time.sleep(self.update_interval / 1000.0)
    
    def update_interface(self):
        """Обновление интерфейса"""
        if self.current_data:
            # Обновление статуса
            if self.current_data.get('linkUp', False):
                self.status_label.config(text="🟢 Связь активна", foreground="green")
            else:
                self.status_label.config(text="🔴 Связь потеряна", foreground="red")
            
            # Обновление attitude
            attitude = self.current_data.get('attitude', {})
            self.roll_label.config(text=f"{attitude.get('roll', 0):.2f}°")
            self.pitch_label.config(text=f"{attitude.get('pitch', 0):.2f}°")
            self.yaw_label.config(text=f"{attitude.get('yaw', 0):.2f}°")
            
            # Обновление battery
            battery = self.current_data.get('battery', {})
            self.voltage_label.config(text=f"{battery.get('voltage', 0):.2f}V")
            self.current_label.config(text=f"{battery.get('current', 0):.0f}mA")
            self.remaining_label.config(text=f"{battery.get('remaining', 0):.0f}%")
            
            # Обновление каналов
            channels = self.current_data.get('channels', [])
            for i, label in enumerate(self.channel_labels):
                if i < len(channels):
                    label.config(text=str(channels[i]))
        
        # Планирование следующего обновления
        self.root.after(self.update_interval, self.update_interface)
    
    def toggle_mode(self):
        """Переключение режима работы"""
        new_mode = "manual" if self.mode_var.get() == "joystick" else "joystick"
        
        try:
            response = requests.get(f"{self.api_url}/api/command?cmd=setMode&value={new_mode}")
            if response.status_code == 200:
                self.mode_var.set(new_mode)
                messagebox.showinfo("Успех", f"Режим изменен на: {new_mode}")
            else:
                messagebox.showerror("Ошибка", "Не удалось изменить режим")
        except Exception as e:
            messagebox.showerror("Ошибка", f"Ошибка подключения: {e}")

def main():
    root = tk.Tk()
    app = CRSFRealtimeInterface(root)
    root.mainloop()

if __name__ == "__main__":
    main()
```

**Запуск:**

```bash
# Установка зависимостей
pip3 install requests

# Запуск GUI
python3 python/crsf_realtime_interface.py
```

### Demo Interface (`demo_interface.py`)

Демонстрационный интерфейс с примерами использования API

```python
#!/usr/bin/env python3
"""
Demo Interface для CRSF API
Демонстрация возможностей системы
"""

import requests
import time
import json

class CRSFDemo:
    def __init__(self, api_url="http://localhost:8081"):
        self.api_url = api_url
    
    def get_telemetry(self):
        """Получение телеметрии"""
        try:
            response = requests.get(f"{self.api_url}/api/telemetry")
            return response.json() if response.status_code == 200 else None
        except Exception as e:
            print(f"Ошибка получения телеметрии: {e}")
            return None
    
    def set_mode(self, mode):
        """Установка режима работы"""
        try:
            response = requests.get(f"{self.api_url}/api/command?cmd=setMode&value={mode}")
            return response.status_code == 200
        except Exception as e:
            print(f"Ошибка установки режима: {e}")
            return False
    
    def set_channel(self, channel, value):
        """Установка значения канала"""
        try:
            response = requests.get(f"{self.api_url}/api/command?cmd=setChannel&value={channel}={value}")
            return response.status_code == 200
        except Exception as e:
            print(f"Ошибка установки канала: {e}")
            return False
    
    def demo_manual_mode(self):
        """Демонстрация ручного режима"""
        print("🎮 Демонстрация ручного режима")
        
        # Переключение в ручной режим
        if not self.set_mode("manual"):
            print("❌ Не удалось переключиться в ручной режим")
            return
        
        print("✅ Переключены в ручной режим")
        
        # Безопасные значения
        safe_channels = {
            1: 1500,  # Roll - центр
            2: 1500,  # Pitch - центр  
            3: 1000,  # Throttle - минимум (безопасно!)
            4: 1500,  # Yaw - центр
        }
        
        print("🔒 Установка безопасных значений...")
        for ch, val in safe_channels.items():
            if self.set_channel(ch, val):
                print(f"  CH{ch}: {val} мкс")
            else:
                print(f"  ❌ Ошибка установки CH{ch}")
        
        time.sleep(1)
        
        # Демонстрация управления
        print("🎯 Демонстрация управления...")
        
        # Roll вправо
        print("  Roll вправо...")
        self.set_channel(1, 1800)
        time.sleep(2)
        
        # Roll влево  
        print("  Roll влево...")
        self.set_channel(1, 1200)
        time.sleep(2)
        
        # Возврат в центр
        print("  Возврат в центр...")
        self.set_channel(1, 1500)
        time.sleep(1)
        
        # Pitch вперед
        print("  Pitch вперед...")
        self.set_channel(2, 1700)
        time.sleep(2)
        
        # Возврат в центр
        print("  Возврат в центр...")
        self.set_channel(2, 1500)
        time.sleep(1)
        
        # Возврат к джойстику
        print("🔄 Возврат к джойстику...")
        if self.set_mode("joystick"):
            print("✅ Переключены обратно в режим джойстика")
        else:
            print("❌ Ошибка переключения в режим джойстика")
    
    def demo_telemetry_monitoring(self):
        """Демонстрация мониторинга телеметрии"""
        print("📊 Демонстрация мониторинга телеметрии")
        
        for i in range(10):
            data = self.get_telemetry()
            if data:
                print(f"\n--- Обновление {i+1} ---")
                print(f"Связь: {'🟢 Активна' if data.get('linkUp') else '🔴 Потеряна'}")
                
                attitude = data.get('attitude', {})
                print(f"Attitude: Roll={attitude.get('roll', 0):.1f}°, "
                      f"Pitch={attitude.get('pitch', 0):.1f}°, "
                      f"Yaw={attitude.get('yaw', 0):.1f}°")
                
                battery = data.get('battery', {})
                print(f"Battery: {battery.get('voltage', 0):.2f}V, "
                      f"{battery.get('current', 0):.0f}mA, "
                      f"{battery.get('remaining', 0):.0f}%")
                
                channels = data.get('channels', [])
                print(f"Каналы: {channels[:4]}...")  # Показываем первые 4
            else:
                print(f"❌ Ошибка получения данных {i+1}")
            
            time.sleep(1)
    
    def run_full_demo(self):
        """Запуск полной демонстрации"""
        print("🚀 Запуск полной демонстрации CRSF-IO-3_2")
        print("=" * 50)
        
        # Проверка подключения
        print("🔍 Проверка подключения к API...")
        data = self.get_telemetry()
        if not data:
            print("❌ Не удается подключиться к API серверу")
            print("💡 Убедитесь, что crsf_io_rpi запущен")
            return
        
        print("✅ API сервер доступен")
        
        # Демонстрация телеметрии
        self.demo_telemetry_monitoring()
        
        # Демонстрация ручного режима
        input("\n⏸️ Нажмите Enter для демонстрации ручного режима...")
        self.demo_manual_mode()
        
        print("\n🎉 Демонстрация завершена!")

if __name__ == "__main__":
    demo = CRSFDemo()
    demo.run_full_demo()
```

**Запуск демо:**

```bash
python3 python/demo_interface.py
```

## 📦 Установка зависимостей

```bash
# Основные зависимости
pip3 install requests

# Для GUI интерфейса
pip3 install tkinter  # Обычно уже установлен

# Для работы с UART
pip3 install pyserial

# Для дополнительных возможностей
pip3 install matplotlib numpy  # Графики телеметрии
```

## 🔧 Использование

### Тестирование UART

```bash
# Базовый тест
python3 python/uart3_test.py

# Тест с параметрами
python3 -c "
import sys
sys.path.append('python')
from uart3_test import test_uart_port
test_uart_port('/dev/ttyAMA0', 420000)
"
```

### Мониторинг телеметрии

```bash
# GUI интерфейс
python3 python/crsf_realtime_interface.py

# Консольный мониторинг
python3 -c "
import sys
sys.path.append('python')
from demo_interface import CRSFDemo
demo = CRSFDemo()
demo.demo_telemetry_monitoring()
"
```

### Демонстрация API

```bash
# Полная демонстрация
python3 python/demo_interface.py

# Только ручной режим
python3 -c "
import sys
sys.path.append('python')
from demo_interface import CRSFDemo
demo = CRSFDemo()
demo.demo_manual_mode()
"
```

## 🐛 Отладка

### Проверка подключения

```python
import requests

try:
    response = requests.get("http://localhost:8081/api/telemetry", timeout=5)
    print(f"Статус: {response.status_code}")
    print(f"Данные: {response.json()}")
except requests.exceptions.ConnectionError:
    print("❌ Сервер недоступен")
except requests.exceptions.Timeout:
    print("⏰ Таймаут подключения")
except Exception as e:
    print(f"❌ Ошибка: {e}")
```

### Логирование

```python
import logging

# Настройка логирования
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

# Использование в коде
logger.debug("Отправка запроса к API")
logger.info("Получены данные телеметрии")
logger.warning("Потеря связи с полетником")
logger.error("Критическая ошибка")
```

## 📚 Связанные файлы

- `main.cpp` - Основное приложение
- `telemetry_server.cpp` - Веб-сервер API
- `API_README.md` - Документация API
- `MANUAL_MODE_GUIDE.md` - Руководство по ручному режиму
