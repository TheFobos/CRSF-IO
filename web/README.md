# Веб-интерфейс телеметрии

Веб-интерфейс для мониторинга телеметрии дрона в реальном времени через браузер.

## 📁 Структура

```
web/
├── index.html           # Главная страница
├── style.css           # Стили CSS
├── script.js           # JavaScript логика
└── README.md           # Эта документация
```

## 🌐 Веб-интерфейс

### Главная страница (`index.html`)

HTML страница с интерфейсом мониторинга телеметрии

```html
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CRSF Telemetry Monitor</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <div class="container">
        <header>
            <h1>🚁 CRSF Telemetry Monitor</h1>
            <div class="status" id="status">Подключение...</div>
        </header>
        
        <main>
            <!-- Телеметрия -->
            <section class="telemetry">
                <h2>📊 Телеметрия</h2>
                
                <div class="data-grid">
                    <!-- Attitude -->
                    <div class="data-group">
                        <h3>Положение</h3>
                        <div class="data-item">
                            <label>Roll:</label>
                            <span id="roll">0.00°</span>
                        </div>
                        <div class="data-item">
                            <label>Pitch:</label>
                            <span id="pitch">0.00°</span>
                        </div>
                        <div class="data-item">
                            <label>Yaw:</label>
                            <span id="yaw">0.00°</span>
                        </div>
                    </div>
                    
                    <!-- Battery -->
                    <div class="data-group">
                        <h3>Батарея</h3>
                        <div class="data-item">
                            <label>Напряжение:</label>
                            <span id="voltage">0.00V</span>
                        </div>
                        <div class="data-item">
                            <label>Ток:</label>
                            <span id="current">0mA</span>
                        </div>
                        <div class="data-item">
                            <label>Остаток:</label>
                            <span id="remaining">0%</span>
                        </div>
                    </div>
                    
                    <!-- Link Stats -->
                    <div class="data-group">
                        <h3>Связь</h3>
                        <div class="data-item">
                            <label>Статус:</label>
                            <span id="link-status">Неизвестно</span>
                        </div>
                        <div class="data-item">
                            <label>Пакеты:</label>
                            <span id="packets">0</span>
                        </div>
                        <div class="data-item">
                            <label>Потери:</label>
                            <span id="packets-lost">0</span>
                        </div>
                    </div>
                </div>
            </section>
            
            <!-- RC Каналы -->
            <section class="channels">
                <h2>📡 RC Каналы</h2>
                <div class="channels-grid" id="channels-grid">
                    <!-- Каналы будут добавлены через JavaScript -->
                </div>
            </section>
            
            <!-- Управление -->
            <section class="control">
                <h2>🎮 Управление</h2>
                
                <div class="control-group">
                    <label for="mode-select">Режим работы:</label>
                    <select id="mode-select">
                        <option value="joystick">Джойстик</option>
                        <option value="manual">Ручной</option>
                    </select>
                    <button id="mode-btn">Переключить</button>
                </div>
                
                <div class="control-group" id="manual-controls" style="display: none;">
                    <h3>Ручное управление</h3>
                    <div class="channel-controls" id="channel-controls">
                        <!-- Элементы управления каналами -->
                    </div>
                </div>
            </section>
        </main>
        
        <footer>
            <p>CRSF-IO-3_2 Telemetry Monitor | Обновление каждые 20мс</p>
        </footer>
    </div>
    
    <script src="script.js"></script>
</body>
</html>
```

### Стили (`style.css`)

CSS стили для современного интерфейса

```css
/* Основные стили */
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    min-height: 100vh;
    color: #333;
}

.container {
    max-width: 1200px;
    margin: 0 auto;
    padding: 20px;
}

/* Заголовок */
header {
    text-align: center;
    margin-bottom: 30px;
    background: rgba(255, 255, 255, 0.95);
    padding: 20px;
    border-radius: 15px;
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
}

header h1 {
    color: #2c3e50;
    margin-bottom: 10px;
    font-size: 2.5em;
}

.status {
    font-size: 1.2em;
    font-weight: bold;
    padding: 10px 20px;
    border-radius: 25px;
    display: inline-block;
}

.status.connected {
    background: #27ae60;
    color: white;
}

.status.disconnected {
    background: #e74c3c;
    color: white;
}

/* Основной контент */
main {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 20px;
    margin-bottom: 30px;
}

section {
    background: rgba(255, 255, 255, 0.95);
    padding: 25px;
    border-radius: 15px;
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
}

section h2 {
    color: #2c3e50;
    margin-bottom: 20px;
    font-size: 1.5em;
    border-bottom: 2px solid #3498db;
    padding-bottom: 10px;
}

/* Телеметрия */
.data-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    gap: 20px;
}

.data-group {
    background: #f8f9fa;
    padding: 15px;
    border-radius: 10px;
    border-left: 4px solid #3498db;
}

.data-group h3 {
    color: #2c3e50;
    margin-bottom: 10px;
    font-size: 1.1em;
}

.data-item {
    display: flex;
    justify-content: space-between;
    margin-bottom: 8px;
    padding: 5px 0;
}

.data-item label {
    font-weight: 600;
    color: #555;
}

.data-item span {
    font-weight: bold;
    color: #2c3e50;
}

/* RC Каналы */
.channels-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));
    gap: 10px;
}

.channel-item {
    background: #f8f9fa;
    padding: 10px;
    border-radius: 8px;
    text-align: center;
    border: 2px solid #e9ecef;
    transition: all 0.3s ease;
}

.channel-item:hover {
    border-color: #3498db;
    transform: translateY(-2px);
}

.channel-item.active {
    border-color: #27ae60;
    background: #d5f4e6;
}

.channel-label {
    font-weight: bold;
    color: #2c3e50;
    margin-bottom: 5px;
}

.channel-value {
    font-size: 1.2em;
    font-weight: bold;
    color: #27ae60;
}

/* Управление */
.control-group {
    margin-bottom: 20px;
    padding: 15px;
    background: #f8f9fa;
    border-radius: 10px;
}

.control-group label {
    display: block;
    margin-bottom: 5px;
    font-weight: 600;
    color: #2c3e50;
}

select, button {
    padding: 10px 15px;
    border: none;
    border-radius: 5px;
    font-size: 1em;
    margin-right: 10px;
}

select {
    background: white;
    border: 2px solid #e9ecef;
    color: #2c3e50;
}

button {
    background: #3498db;
    color: white;
    cursor: pointer;
    transition: background 0.3s ease;
}

button:hover {
    background: #2980b9;
}

button:disabled {
    background: #bdc3c7;
    cursor: not-allowed;
}

/* Ручное управление */
.channel-controls {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    gap: 15px;
}

.channel-control {
    background: white;
    padding: 15px;
    border-radius: 8px;
    border: 2px solid #e9ecef;
}

.channel-control label {
    display: block;
    margin-bottom: 5px;
    font-weight: 600;
}

.channel-control input[type="range"] {
    width: 100%;
    margin: 10px 0;
}

.channel-control input[type="number"] {
    width: 100%;
    padding: 5px;
    border: 1px solid #ddd;
    border-radius: 3px;
}

/* Подвал */
footer {
    text-align: center;
    background: rgba(255, 255, 255, 0.95);
    padding: 15px;
    border-radius: 15px;
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
}

footer p {
    color: #7f8c8d;
    font-size: 0.9em;
}

/* Адаптивность */
@media (max-width: 768px) {
    main {
        grid-template-columns: 1fr;
    }
    
    .data-grid {
        grid-template-columns: 1fr;
    }
    
    .channels-grid {
        grid-template-columns: repeat(2, 1fr);
    }
    
    header h1 {
        font-size: 2em;
    }
}

/* Анимации */
@keyframes pulse {
    0% { opacity: 1; }
    50% { opacity: 0.5; }
    100% { opacity: 1; }
}

.status.disconnected {
    animation: pulse 2s infinite;
}

/* Индикаторы состояния */
.indicator {
    display: inline-block;
    width: 10px;
    height: 10px;
    border-radius: 50%;
    margin-right: 5px;
}

.indicator.green {
    background: #27ae60;
}

.indicator.red {
    background: #e74c3c;
}

.indicator.yellow {
    background: #f39c12;
}
```

### JavaScript (`script.js`)

JavaScript логика для обновления данных и управления

```javascript
class CRSFTelemetryMonitor {
    constructor() {
        this.apiUrl = 'http://localhost:8081';
        this.updateInterval = 20; // мс
        this.isRunning = false;
        this.currentData = null;
        
        this.init();
    }
    
    init() {
        this.setupEventListeners();
        this.createChannelControls();
        this.startDataUpdate();
    }
    
    setupEventListeners() {
        // Переключение режима
        document.getElementById('mode-btn').addEventListener('click', () => {
            this.toggleMode();
        });
        
        // Изменение режима в селекте
        document.getElementById('mode-select').addEventListener('change', (e) => {
            this.setMode(e.target.value);
        });
    }
    
    createChannelControls() {
        const channelsGrid = document.getElementById('channels-grid');
        const channelControls = document.getElementById('channel-controls');
        
        // Создание элементов для отображения каналов
        for (let i = 1; i <= 16; i++) {
            // Элемент отображения
            const channelItem = document.createElement('div');
            channelItem.className = 'channel-item';
            channelItem.id = `channel-display-${i}`;
            channelItem.innerHTML = `
                <div class="channel-label">CH${i}</div>
                <div class="channel-value" id="channel-value-${i}">1500</div>
            `;
            channelsGrid.appendChild(channelItem);
            
            // Элемент управления (для ручного режима)
            const channelControl = document.createElement('div');
            channelControl.className = 'channel-control';
            channelControl.innerHTML = `
                <label>CH${i}:</label>
                <input type="range" id="channel-slider-${i}" min="1000" max="2000" value="1500" step="10">
                <input type="number" id="channel-input-${i}" min="1000" max="2000" value="1500">
            `;
            channelControls.appendChild(channelControl);
            
            // Синхронизация слайдера и поля ввода
            const slider = document.getElementById(`channel-slider-${i}`);
            const input = document.getElementById(`channel-input-${i}`);
            
            slider.addEventListener('input', (e) => {
                input.value = e.target.value;
                this.setChannel(i, parseInt(e.target.value));
            });
            
            input.addEventListener('change', (e) => {
                slider.value = e.target.value;
                this.setChannel(i, parseInt(e.target.value));
            });
        }
    }
    
    async startDataUpdate() {
        this.isRunning = true;
        
        while (this.isRunning) {
            try {
                await this.updateTelemetry();
            } catch (error) {
                console.error('Ошибка обновления телеметрии:', error);
            }
            
            await this.sleep(this.updateInterval);
        }
    }
    
    async updateTelemetry() {
        try {
            const response = await fetch(`${this.apiUrl}/api/telemetry`);
            if (response.ok) {
                this.currentData = await response.json();
                this.updateInterface();
            } else {
                throw new Error(`HTTP ${response.status}`);
            }
        } catch (error) {
            console.error('Ошибка получения телеметрии:', error);
            this.currentData = null;
            this.updateConnectionStatus(false);
        }
    }
    
    updateInterface() {
        if (!this.currentData) return;
        
        // Обновление статуса подключения
        this.updateConnectionStatus(this.currentData.linkUp);
        
        // Обновление attitude
        const attitude = this.currentData.attitude || {};
        document.getElementById('roll').textContent = `${attitude.roll || 0}°`;
        document.getElementById('pitch').textContent = `${attitude.pitch || 0}°`;
        document.getElementById('yaw').textContent = `${attitude.yaw || 0}°`;
        
        // Обновление battery
        const battery = this.currentData.battery || {};
        document.getElementById('voltage').textContent = `${battery.voltage || 0}V`;
        document.getElementById('current').textContent = `${battery.current || 0}mA`;
        document.getElementById('remaining').textContent = `${battery.remaining || 0}%`;
        
        // Обновление статистики связи
        document.getElementById('link-status').textContent = 
            this.currentData.linkUp ? 'Активна' : 'Потеряна';
        document.getElementById('packets').textContent = 
            this.currentData.packetsReceived || 0;
        document.getElementById('packets-lost').textContent = 
            this.currentData.packetsLost || 0;
        
        // Обновление каналов
        this.updateChannels(this.currentData.channels || []);
        
        // Обновление режима работы
        const workMode = this.currentData.workMode || 'joystick';
        document.getElementById('mode-select').value = workMode;
        this.updateModeControls(workMode);
    }
    
    updateConnectionStatus(isConnected) {
        const statusElement = document.getElementById('status');
        if (isConnected) {
            statusElement.textContent = '🟢 Связь активна';
            statusElement.className = 'status connected';
        } else {
            statusElement.textContent = '🔴 Связь потеряна';
            statusElement.className = 'status disconnected';
        }
    }
    
    updateChannels(channels) {
        for (let i = 0; i < 16; i++) {
            const channelValue = channels[i] || 1500;
            const valueElement = document.getElementById(`channel-value-${i + 1}`);
            const displayElement = document.getElementById(`channel-display-${i + 1}`);
            
            if (valueElement) {
                valueElement.textContent = channelValue;
            }
            
            if (displayElement) {
                // Подсветка активных каналов
                if (channelValue !== 1500) {
                    displayElement.classList.add('active');
                } else {
                    displayElement.classList.remove('active');
                }
            }
        }
    }
    
    updateModeControls(mode) {
        const manualControls = document.getElementById('manual-controls');
        if (mode === 'manual') {
            manualControls.style.display = 'block';
        } else {
            manualControls.style.display = 'none';
        }
    }
    
    async toggleMode() {
        const currentMode = document.getElementById('mode-select').value;
        const newMode = currentMode === 'joystick' ? 'manual' : 'joystick';
        await this.setMode(newMode);
    }
    
    async setMode(mode) {
        try {
            const response = await fetch(`${this.apiUrl}/api/command?cmd=setMode&value=${mode}`);
            if (response.ok) {
                document.getElementById('mode-select').value = mode;
                this.updateModeControls(mode);
                this.showNotification(`Режим изменен на: ${mode}`, 'success');
            } else {
                throw new Error(`HTTP ${response.status}`);
            }
        } catch (error) {
            console.error('Ошибка установки режима:', error);
            this.showNotification('Ошибка изменения режима', 'error');
        }
    }
    
    async setChannel(channel, value) {
        try {
            const response = await fetch(`${this.apiUrl}/api/command?cmd=setChannel&value=${channel}=${value}`);
            if (!response.ok) {
                throw new Error(`HTTP ${response.status}`);
            }
        } catch (error) {
            console.error(`Ошибка установки канала ${channel}:`, error);
            this.showNotification(`Ошибка установки CH${channel}`, 'error');
        }
    }
    
    showNotification(message, type = 'info') {
        // Простое уведомление (можно заменить на toast)
        console.log(`[${type.toUpperCase()}] ${message}`);
        
        // Создание временного уведомления
        const notification = document.createElement('div');
        notification.style.cssText = `
            position: fixed;
            top: 20px;
            right: 20px;
            padding: 15px 20px;
            border-radius: 5px;
            color: white;
            font-weight: bold;
            z-index: 1000;
            animation: slideIn 0.3s ease;
        `;
        
        if (type === 'success') {
            notification.style.background = '#27ae60';
        } else if (type === 'error') {
            notification.style.background = '#e74c3c';
        } else {
            notification.style.background = '#3498db';
        }
        
        notification.textContent = message;
        document.body.appendChild(notification);
        
        // Удаление через 3 секунды
        setTimeout(() => {
            notification.remove();
        }, 3000);
    }
    
    sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }
    
    stop() {
        this.isRunning = false;
    }
}

// Инициализация при загрузке страницы
document.addEventListener('DOMContentLoaded', () => {
    window.telemetryMonitor = new CRSFTelemetryMonitor();
});

// Очистка при выгрузке страницы
window.addEventListener('beforeunload', () => {
    if (window.telemetryMonitor) {
        window.telemetryMonitor.stop();
    }
});
```

## 🚀 Использование

### Запуск веб-сервера

```bash
# Убедитесь, что crsf_io_rpi запущен
sudo ./crsf_io_rpi

# Веб-сервер автоматически запустится на порту 8081
```

### Доступ к интерфейсу

```bash
# Локальный доступ
http://localhost:8081

# Удаленный доступ (замените IP)
http://192.168.1.100:8081
```

### Функции интерфейса

1. **Мониторинг телеметрии** - обновление каждые 20мс
2. **Отображение RC каналов** - все 16 каналов
3. **Переключение режимов** - джойстик/ручной
4. **Ручное управление** - слайдеры для каналов
5. **Статистика связи** - пакеты, потери, качество

## 🔧 Настройка

### Изменение частоты обновления

```javascript
// В script.js
this.updateInterval = 50; // Изменить на 50мс (20 Гц)
```

### Изменение API URL

```javascript
// В script.js
this.apiUrl = 'http://192.168.1.100:8081'; // Удаленный сервер
```

### Кастомизация стилей

```css
/* В style.css - изменить цветовую схему */
:root {
    --primary-color: #3498db;
    --success-color: #27ae60;
    --error-color: #e74c3c;
    --background-gradient: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
}
```

## 📱 Мобильная версия

Интерфейс адаптивен и работает на мобильных устройствах:

- **Responsive дизайн** - автоматическая адаптация
- **Touch-friendly** - удобное управление на сенсоре
- **Оптимизированная сетка** - каналы в 2 колонки на мобильных

## 🔗 Связанные файлы

- `telemetry_server.cpp` - Веб-сервер API
- `API_README.md` - Документация API
- `crsf_realtime_interface.py` - Python GUI альтернатива
