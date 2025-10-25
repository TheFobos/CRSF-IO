#!/usr/bin/env python3
"""
CRSF Realtime Interface
Интерфейс для отображения данных CRSF API в реальном времени
"""

import tkinter as tk
from tkinter import ttk, messagebox
import requests
import json
import threading
import time
from datetime import datetime
import queue

class CRSFRealtimeInterface:
    def __init__(self, root):
        self.root = root
        self.root.title("CRSF Realtime Interface")
        self.root.geometry("1200x800")
        self.root.configure(bg='#2b2b2b')
        
        # Настройки
        self.api_url = "http://localhost:8081"  # Исправляем порт
        self.update_interval = 20  # Уменьшаем до 20мс для реалтайма
        self.is_running = False
        self.data_queue = queue.Queue()
        
        # Данные
        self.current_data = None
        self.connection_status = False
        
        # Создаем интерфейс
        self.create_interface()
        
        # Запускаем обновление данных
        self.start_data_update()
    
    def create_interface(self):
        """Создание интерфейса"""
        # Главный фрейм
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # Настройка растягивания
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
        main_frame.columnconfigure(1, weight=1)
        
        # Заголовок
        title_label = ttk.Label(main_frame, text="CRSF Realtime Interface", 
                               font=('Arial', 16, 'bold'))
        title_label.grid(row=0, column=0, columnspan=2, pady=(0, 20))
        
        # Панель управления
        self.create_control_panel(main_frame)
        
        # Панель статуса
        self.create_status_panel(main_frame)
        
        # Панель данных
        self.create_data_panel(main_frame)
        
        # Панель каналов
        self.create_channels_panel(main_frame)
        
        # Панель телеметрии
        self.create_telemetry_panel(main_frame)
    
    def create_control_panel(self, parent):
        """Панель управления"""
        control_frame = ttk.LabelFrame(parent, text="Управление", padding="10")
        control_frame.grid(row=1, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        # URL API
        ttk.Label(control_frame, text="API URL:").grid(row=0, column=0, sticky=tk.W)
        self.url_var = tk.StringVar(value=self.api_url)
        url_entry = ttk.Entry(control_frame, textvariable=self.url_var, width=30)
        url_entry.grid(row=0, column=1, padx=(5, 10))
        
        # Интервал обновления
        ttk.Label(control_frame, text="Интервал (мс):").grid(row=0, column=2, sticky=tk.W)
        self.interval_var = tk.StringVar(value=str(self.update_interval))
        interval_entry = ttk.Entry(control_frame, textvariable=self.interval_var, width=10)
        interval_entry.grid(row=0, column=3, padx=(5, 10))
        
        # Кнопки
        self.start_button = ttk.Button(control_frame, text="Старт", command=self.start_monitoring)
        self.start_button.grid(row=0, column=4, padx=(5, 5))
        
        self.stop_button = ttk.Button(control_frame, text="Стоп", command=self.stop_monitoring, state='disabled')
        self.stop_button.grid(row=0, column=5, padx=(5, 5))
        
        # Режим работы
        ttk.Label(control_frame, text="Режим:").grid(row=1, column=0, sticky=tk.W, pady=(10, 0))
        self.mode_var = tk.StringVar(value="joystick")
        mode_combo = ttk.Combobox(control_frame, textvariable=self.mode_var, 
                                 values=["joystick", "manual"], width=15)
        mode_combo.grid(row=1, column=1, padx=(5, 10), pady=(10, 0))
        
        mode_button = ttk.Button(control_frame, text="Установить режим", command=self.set_mode)
        mode_button.grid(row=1, column=2, padx=(5, 5), pady=(10, 0))
    
    def create_status_panel(self, parent):
        """Панель статуса"""
        status_frame = ttk.LabelFrame(parent, text="Статус", padding="10")
        status_frame.grid(row=2, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        # Статус подключения
        self.status_label = ttk.Label(status_frame, text="Отключено", foreground="red")
        self.status_label.grid(row=0, column=0, sticky=tk.W)
        
        # Время последнего обновления
        self.last_update_label = ttk.Label(status_frame, text="Последнее обновление: Никогда")
        self.last_update_label.grid(row=0, column=1, sticky=tk.W, padx=(20, 0))
        
        # Активный порт
        self.port_label = ttk.Label(status_frame, text="Порт: Неизвестно")
        self.port_label.grid(row=1, column=0, sticky=tk.W)
        
        # Статистика пакетов
        self.packets_label = ttk.Label(status_frame, text="Пакеты: 0/0/0")
        self.packets_label.grid(row=1, column=1, sticky=tk.W, padx=(20, 0))
    
    def create_data_panel(self, parent):
        """Панель основных данных"""
        data_frame = ttk.LabelFrame(parent, text="Основные данные", padding="10")
        data_frame.grid(row=3, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        # Время
        ttk.Label(data_frame, text="Время:").grid(row=0, column=0, sticky=tk.W)
        self.time_label = ttk.Label(data_frame, text="--:--:--", font=('Arial', 12, 'bold'))
        self.time_label.grid(row=0, column=1, sticky=tk.W, padx=(10, 0))
        
        # Режим работы
        ttk.Label(data_frame, text="Режим работы:").grid(row=0, column=2, sticky=tk.W, padx=(20, 0))
        self.work_mode_label = ttk.Label(data_frame, text="Неизвестно")
        self.work_mode_label.grid(row=0, column=3, sticky=tk.W, padx=(10, 0))
        
        # Авто режим
        ttk.Label(data_frame, text="Авто режим:").grid(row=1, column=0, sticky=tk.W)
        self.auto_mode_label = ttk.Label(data_frame, text="Выключен")
        self.auto_mode_label.grid(row=1, column=1, sticky=tk.W, padx=(10, 0))
        
        # Авто шаг
        ttk.Label(data_frame, text="Авто шаг:").grid(row=1, column=2, sticky=tk.W, padx=(20, 0))
        self.auto_step_label = ttk.Label(data_frame, text="0")
        self.auto_step_label.grid(row=1, column=3, sticky=tk.W, padx=(10, 0))
    
    def create_channels_panel(self, parent):
        """Панель каналов"""
        channels_frame = ttk.LabelFrame(parent, text="RC Каналы", padding="10")
        channels_frame.grid(row=4, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        # Создаем сетку для каналов
        self.channel_labels = []
        self.channel_bars = []
        
        for i in range(16):
            row = i // 4
            col = (i % 4) * 3
            
            # Номер канала
            ttk.Label(channels_frame, text=f"CH{i+1}:").grid(row=row, column=col, sticky=tk.W, padx=(0, 5))
            
            # Прогресс бар
            bar = ttk.Progressbar(channels_frame, length=100, mode='determinate')
            bar.grid(row=row, column=col+1, padx=(0, 5))
            self.channel_bars.append(bar)
            
            # Значение
            label = ttk.Label(channels_frame, text="1500", width=6)
            label.grid(row=row, column=col+2, sticky=tk.W)
            self.channel_labels.append(label)
    
    def create_telemetry_panel(self, parent):
        """Панель телеметрии"""
        telemetry_frame = ttk.LabelFrame(parent, text="Телеметрия", padding="10")
        telemetry_frame.grid(row=5, column=0, columnspan=2, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # Настройка растягивания
        parent.rowconfigure(5, weight=1)
        
        # Создаем notebook для вкладок
        notebook = ttk.Notebook(telemetry_frame)
        notebook.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # Настройка растягивания
        telemetry_frame.columnconfigure(0, weight=1)
        telemetry_frame.rowconfigure(0, weight=1)
        
        # Вкладка GPS
        gps_frame = ttk.Frame(notebook)
        notebook.add(gps_frame, text="GPS")
        self.create_gps_tab(gps_frame)
        
        # Вкладка Батарея
        battery_frame = ttk.Frame(notebook)
        notebook.add(battery_frame, text="Батарея")
        self.create_battery_tab(battery_frame)
        
        # Вкладка Положение
        attitude_frame = ttk.Frame(notebook)
        notebook.add(attitude_frame, text="Положение")
        self.create_attitude_tab(attitude_frame)
    
    def create_gps_tab(self, parent):
        """Вкладка GPS"""
        # Широта
        ttk.Label(parent, text="Широта:").grid(row=0, column=0, sticky=tk.W, padx=10, pady=5)
        self.lat_label = ttk.Label(parent, text="0.000000", font=('Arial', 10, 'bold'))
        self.lat_label.grid(row=0, column=1, sticky=tk.W, padx=10, pady=5)
        
        # Долгота
        ttk.Label(parent, text="Долгота:").grid(row=1, column=0, sticky=tk.W, padx=10, pady=5)
        self.lon_label = ttk.Label(parent, text="0.000000", font=('Arial', 10, 'bold'))
        self.lon_label.grid(row=1, column=1, sticky=tk.W, padx=10, pady=5)
        
        # Высота
        ttk.Label(parent, text="Высота:").grid(row=2, column=0, sticky=tk.W, padx=10, pady=5)
        self.alt_label = ttk.Label(parent, text="0.0 м", font=('Arial', 10, 'bold'))
        self.alt_label.grid(row=2, column=1, sticky=tk.W, padx=10, pady=5)
        
        # Скорость
        ttk.Label(parent, text="Скорость:").grid(row=3, column=0, sticky=tk.W, padx=10, pady=5)
        self.speed_label = ttk.Label(parent, text="0.0 км/ч", font=('Arial', 10, 'bold'))
        self.speed_label.grid(row=3, column=1, sticky=tk.W, padx=10, pady=5)
    
    def create_battery_tab(self, parent):
        """Вкладка Батарея"""
        # Напряжение
        ttk.Label(parent, text="Напряжение:").grid(row=0, column=0, sticky=tk.W, padx=10, pady=5)
        self.voltage_label = ttk.Label(parent, text="0.0 В", font=('Arial', 10, 'bold'))
        self.voltage_label.grid(row=0, column=1, sticky=tk.W, padx=10, pady=5)
        
        # Ток
        ttk.Label(parent, text="Ток:").grid(row=1, column=0, sticky=tk.W, padx=10, pady=5)
        self.current_label = ttk.Label(parent, text="0 мА", font=('Arial', 10, 'bold'))
        self.current_label.grid(row=1, column=1, sticky=tk.W, padx=10, pady=5)
        
        # Емкость
        ttk.Label(parent, text="Емкость:").grid(row=2, column=0, sticky=tk.W, padx=10, pady=5)
        self.capacity_label = ttk.Label(parent, text="0 мАч", font=('Arial', 10, 'bold'))
        self.capacity_label.grid(row=2, column=1, sticky=tk.W, padx=10, pady=5)
        
        # Остаток
        ttk.Label(parent, text="Остаток:").grid(row=3, column=0, sticky=tk.W, padx=10, pady=5)
        self.remaining_label = ttk.Label(parent, text="0%", font=('Arial', 10, 'bold'))
        self.remaining_label.grid(row=3, column=1, sticky=tk.W, padx=10, pady=5)
        
        # Прогресс бар для остатка
        self.battery_bar = ttk.Progressbar(parent, length=200, mode='determinate')
        self.battery_bar.grid(row=4, column=0, columnspan=2, padx=10, pady=10)
    
    def create_attitude_tab(self, parent):
        """Вкладка Положение"""
        # Roll
        ttk.Label(parent, text="Roll (Крен):").grid(row=0, column=0, sticky=tk.W, padx=10, pady=5)
        self.roll_label = ttk.Label(parent, text="0.0°", font=('Arial', 10, 'bold'))
        self.roll_label.grid(row=0, column=1, sticky=tk.W, padx=10, pady=5)
        
        # Pitch
        ttk.Label(parent, text="Pitch (Тангаж):").grid(row=1, column=0, sticky=tk.W, padx=10, pady=5)
        self.pitch_label = ttk.Label(parent, text="0.0°", font=('Arial', 10, 'bold'))
        self.pitch_label.grid(row=1, column=1, sticky=tk.W, padx=10, pady=5)
        
        # Yaw
        ttk.Label(parent, text="Yaw (Рысканье):").grid(row=2, column=0, sticky=tk.W, padx=10, pady=5)
        self.yaw_label = ttk.Label(parent, text="0.0°", font=('Arial', 10, 'bold'))
        self.yaw_label.grid(row=2, column=1, sticky=tk.W, padx=10, pady=5)
        
        # Прогресс бары для углов
        ttk.Label(parent, text="Roll:").grid(row=3, column=0, sticky=tk.W, padx=10, pady=5)
        self.roll_bar = ttk.Progressbar(parent, length=200, mode='determinate')
        self.roll_bar.grid(row=3, column=1, padx=10, pady=5)
        
        ttk.Label(parent, text="Pitch:").grid(row=4, column=0, sticky=tk.W, padx=10, pady=5)
        self.pitch_bar = ttk.Progressbar(parent, length=200, mode='determinate')
        self.pitch_bar.grid(row=4, column=1, padx=10, pady=5)
        
        ttk.Label(parent, text="Yaw:").grid(row=5, column=0, sticky=tk.W, padx=10, pady=5)
        self.yaw_bar = ttk.Progressbar(parent, length=200, mode='determinate')
        self.yaw_bar.grid(row=5, column=1, padx=10, pady=5)
    
    def start_monitoring(self):
        """Запуск мониторинга"""
        try:
            self.update_interval = int(self.interval_var.get())
            self.api_url = self.url_var.get()
            
            self.is_running = True
            self.start_button.config(state='disabled')
            self.stop_button.config(state='normal')
            
            # Запускаем поток обновления данных
            self.update_thread = threading.Thread(target=self.data_update_worker, daemon=True)
            self.update_thread.start()
            
        except ValueError:
            messagebox.showerror("Ошибка", "Неверный интервал обновления")
    
    def stop_monitoring(self):
        """Остановка мониторинга"""
        self.is_running = False
        self.start_button.config(state='normal')
        self.stop_button.config(state='disabled')
        
        # Обновляем статус
        self.status_label.config(text="Остановлено", foreground="orange")
    
    def set_mode(self):
        """Установка режима работы"""
        if not self.is_running:
            messagebox.showwarning("Предупреждение", "Сначала запустите мониторинг")
            return
        
        mode = self.mode_var.get()
        try:
            response = requests.get(f"{self.api_url}/api/command?cmd=setMode&value={mode}", timeout=5)
            if response.status_code == 200:
                messagebox.showinfo("Успех", f"Режим установлен: {mode}")
            else:
                messagebox.showerror("Ошибка", f"Не удалось установить режим: {response.status_code}")
        except Exception as e:
            messagebox.showerror("Ошибка", f"Ошибка подключения: {e}")
    
    def data_update_worker(self):
        """Поток обновления данных"""
        while self.is_running:
            try:
                response = requests.get(f"{self.api_url}/api/telemetry", timeout=2)
                if response.status_code == 200:
                    data = response.json()
                    self.data_queue.put(data)
                else:
                    self.data_queue.put(None)  # Ошибка
            except Exception as e:
                print(f"Ошибка получения данных: {e}")
                self.data_queue.put(None)
            
            time.sleep(self.update_interval / 1000.0)
    
    def start_data_update(self):
        """Запуск обновления интерфейса"""
        self.update_interface()
        self.root.after(20, self.start_data_update)  # Обновляем каждые 20мс для реалтайма
    
    def update_interface(self):
        """Обновление интерфейса"""
        # Обрабатываем очередь данных
        while not self.data_queue.empty():
            data = self.data_queue.get()
            if data:
                self.current_data = data
                self.connection_status = True
                self.update_display()
            else:
                self.connection_status = False
                self.status_label.config(text="Ошибка подключения", foreground="red")
    
    def update_display(self):
        """Обновление отображения данных"""
        if not self.current_data:
            return
        
        data = self.current_data
        
        # Статус подключения
        if data.get('linkUp', False):
            self.status_label.config(text="Подключено", foreground="green")
        else:
            self.status_label.config(text="Отключено", foreground="red")
        
        # Время последнего обновления
        current_time = datetime.now().strftime("%H:%M:%S")
        self.last_update_label.config(text=f"Последнее обновление: {current_time}")
        
        # Активный порт
        self.port_label.config(text=f"Порт: {data.get('activePort', 'Неизвестно')}")
        
        # Статистика пакетов
        packets_text = f"Пакеты: {data.get('packetsReceived', 0)}/{data.get('packetsSent', 0)}/{data.get('packetsLost', 0)}"
        self.packets_label.config(text=packets_text)
        
        # Время
        self.time_label.config(text=data.get('timestamp', '--:--:--'))
        
        # Режим работы
        self.work_mode_label.config(text=data.get('workMode', 'Неизвестно'))
        
        # Каналы
        channels = data.get('channels', [])
        for i, channel_value in enumerate(channels[:16]):
            if i < len(self.channel_labels):
                # Обновляем значение
                self.channel_labels[i].config(text=str(channel_value))
                
                # Обновляем прогресс бар (1000-2000 -> 0-100)
                progress = ((channel_value - 1000) / 1000) * 100
                self.channel_bars[i].config(value=progress)
        
        # GPS
        gps = data.get('gps', {})
        if gps:
            self.lat_label.config(text=f"{gps.get('latitude', 0):.6f}")
            self.lon_label.config(text=f"{gps.get('longitude', 0):.6f}")
            self.alt_label.config(text=f"{gps.get('altitude', 0):.1f} м")
            self.speed_label.config(text=f"{gps.get('speed', 0):.1f} км/ч")
        
        # Батарея
        battery = data.get('battery', {})
        if battery:
            voltage = battery.get('voltage', 0)
            current = battery.get('current', 0)
            capacity = battery.get('capacity', 0)
            remaining = battery.get('remaining', 0)
            
            self.voltage_label.config(text=f"{voltage:.1f} В")
            self.current_label.config(text=f"{current:.0f} мА")
            self.capacity_label.config(text=f"{capacity:.0f} мАч")
            self.remaining_label.config(text=f"{remaining}%")
            
            # Обновляем прогресс бар батареи
            self.battery_bar.config(value=remaining)
        
        # Положение
        attitude = data.get('attitude', {})
        if attitude:
            roll = attitude.get('roll', 0)
            pitch = attitude.get('pitch', 0)
            yaw = attitude.get('yaw', 0)
            
            self.roll_label.config(text=f"{roll:.1f}°")
            self.pitch_label.config(text=f"{pitch:.1f}°")
            self.yaw_label.config(text=f"{yaw:.1f}°")
            
            # Обновляем прогресс бары углов
            # Roll: -180 до 180 -> 0 до 100
            roll_progress = ((roll + 180) / 360) * 100
            self.roll_bar.config(value=roll_progress)
            
            # Pitch: -180 до 180 -> 0 до 100
            pitch_progress = ((pitch + 180) / 360) * 100
            self.pitch_bar.config(value=pitch_progress)
            
            # Yaw: 0 до 360 -> 0 до 100
            yaw_progress = (yaw / 360) * 100
            self.yaw_bar.config(value=yaw_progress)

def main():
    """Главная функция"""
    root = tk.Tk()
    app = CRSFRealtimeInterface(root)
    
    # Обработка закрытия окна
    def on_closing():
        app.stop_monitoring()
        root.destroy()
    
    root.protocol("WM_DELETE_WINDOW", on_closing)
    root.mainloop()

if __name__ == "__main__":
    main()
