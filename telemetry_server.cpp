#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <map>
#include <iomanip>
#include <vector>
#include <algorithm>
#include "libs/crsf/CrsfSerial.h"
#include "libs/log.h"

// Глобальные переменные для хранения данных телеметрии
struct GlobalTelemetryData {
    double battery_voltage = 0.0;
    int battery_current = 0;
    int battery_capacity = 0;
    int battery_remaining = 0;
    double attitude_pitch = 0.0;
    double attitude_roll = 0.0;
    double attitude_yaw = 0.0;
    std::string flight_mode = "Неизвестно";
    std::mutex data_mutex;
} global_telemetry;

class TelemetryServer {
private:
    int server_fd;
    int port;
    std::mutex telemetry_mutex;
    
    // Данные телеметрии
    struct TelemetryData {
        std::string timestamp;
        bool link_up = false;
        std::string active_port = "Порт 1";
        std::string flight_mode = "Неизвестно";
        double battery_voltage = 0.0;
        int battery_current = 0;
        int battery_capacity = 0;
        int battery_remaining = 0;
        double attitude_pitch = 0.0;
        double attitude_roll = 0.0;
        double attitude_yaw = 0.0;
        int channels[16] = {0};
        std::string last_update = "Никогда";
    } telemetry_data;
    
    CrsfSerial* crsf_ptr = nullptr;

public:
    TelemetryServer(int port = 8080) : port(port) {}
    
    void setCrsfPointer(CrsfSerial* crsf) {
        crsf_ptr = crsf;
    }
    
    void updateTelemetry() {
        std::lock_guard<std::mutex> lock(telemetry_mutex);
        
        if (crsf_ptr) {
            // Обновляем данные телеметрии
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            telemetry_data.timestamp = ss.str();
            
            // Получаем статус связи
            telemetry_data.link_up = crsf_ptr->isLinkUp();
            
            // Получаем каналы
            for (int i = 0; i < 16; i++) {
                telemetry_data.channels[i] = crsf_ptr->getChannel(i + 1);
            }
            
            // Получаем данные из глобальных переменных
            {
                std::lock_guard<std::mutex> lock(global_telemetry.data_mutex);
                telemetry_data.battery_voltage = global_telemetry.battery_voltage;
                telemetry_data.battery_current = global_telemetry.battery_current;
                telemetry_data.battery_capacity = global_telemetry.battery_capacity;
                telemetry_data.battery_remaining = global_telemetry.battery_remaining;
                telemetry_data.attitude_pitch = global_telemetry.attitude_pitch;
                telemetry_data.attitude_roll = global_telemetry.attitude_roll;
                telemetry_data.attitude_yaw = global_telemetry.attitude_yaw;
                telemetry_data.flight_mode = global_telemetry.flight_mode;
            }
            
            // Устанавливаем активный порт
            telemetry_data.active_port = "Порт 1"; // Можно улучшить логику определения
            
            telemetry_data.last_update = telemetry_data.timestamp;
            
        }
    }
    
    
    std::string generateHTML() {
        std::lock_guard<std::mutex> lock(telemetry_mutex);
        
        std::stringstream html;
        html << R"(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CRSF Телеметрия</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: #333;
            min-height: 100vh;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            border-radius: 15px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        .header {
            background: linear-gradient(45deg, #2c3e50, #34495e);
            color: white;
            padding: 20px;
            text-align: center;
        }
        .header h1 {
            margin: 0;
            font-size: 2.5em;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        .status-bar {
            background: #ecf0f1;
            padding: 15px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-bottom: 1px solid #bdc3c7;
        }
        .status-item {
            display: flex;
            align-items: center;
            gap: 10px;
        }
        .status-indicator {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            background: #e74c3c;
        }
        .status-indicator.online {
            background: #27ae60;
            box-shadow: 0 0 10px rgba(39, 174, 96, 0.5);
        }
        .content {
            padding: 30px;
        }
        .grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        .card {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            border-left: 4px solid #3498db;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        .card h3 {
            margin: 0 0 15px 0;
            color: #2c3e50;
            font-size: 1.3em;
        }
        .data-row {
            display: flex;
            justify-content: space-between;
            margin: 8px 0;
            padding: 5px 0;
            border-bottom: 1px solid #ecf0f1;
        }
        .data-row:last-child {
            border-bottom: none;
        }
        .data-label {
            font-weight: 600;
            color: #34495e;
        }
        .data-value {
            color: #2c3e50;
            font-family: 'Courier New', monospace;
        }
        .channels-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));
            gap: 10px;
            margin-top: 15px;
        }
        .channel-item {
            background: white;
            padding: 10px;
            border-radius: 5px;
            text-align: center;
            border: 1px solid #bdc3c7;
        }
        .channel-number {
            font-weight: bold;
            color: #3498db;
        }
        .channel-value {
            font-family: 'Courier New', monospace;
            color: #2c3e50;
        }
        .refresh-info {
            text-align: center;
            color: #7f8c8d;
            font-style: italic;
            margin-top: 20px;
        }
        .auto-refresh {
            position: fixed;
            top: 20px;
            right: 20px;
            background: rgba(255,255,255,0.9);
            padding: 10px;
            border-radius: 5px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.2);
        }
    </style>
</head>
<body>
    <div class="auto-refresh">
        <small>🔄 AJAX обновление: 1 сек</small>
    </div>
    
    <div class="container">
        <div class="header">
            <h1>🚁 CRSF Телеметрия</h1>
        </div>
        
        <div class="status-bar">
            <div class="status-item">
                <div class="status-indicator" id="link-status"></div>
                <span>Связь: <strong id="link-text">Проверка...</strong></span>
            </div>
            <div class="status-item">
                <span>Активный порт: <strong>)" << telemetry_data.active_port << R"(</strong></span>
            </div>
            <div class="status-item">
                <span>Обновлено: <strong>)" << telemetry_data.last_update << R"(</strong></span>
            </div>
        </div>
        
        <div class="content">
            <div class="grid">
                <div class="card">
                    <h3>🔋 Батарея</h3>
                    <div class="data-row">
                        <span class="data-label">Напряжение:</span>
                        <span class="data-value">)" << std::fixed << std::setprecision(2) << telemetry_data.battery_voltage << R"( В</span>
                    </div>
                    <div class="data-row">
                        <span class="data-label">Ток:</span>
                        <span class="data-value">)" << telemetry_data.battery_current << R"( мА</span>
                    </div>
                    <div class="data-row">
                        <span class="data-label">Емкость:</span>
                        <span class="data-value">)" << telemetry_data.battery_capacity << R"( мАч</span>
                    </div>
                    <div class="data-row">
                        <span class="data-label">Остаток:</span>
                        <span class="data-value">)" << telemetry_data.battery_remaining << R"( %</span>
                    </div>
                </div>
                
                
                <div class="card">
                    <h3>✈️ Положение</h3>
                    <div class="data-row">
                        <span class="data-label">Крен:</span>
                        <span class="data-value">)" << std::fixed << std::setprecision(1) << telemetry_data.attitude_roll << R"(°</span>
                    </div>
                    <div class="data-row">
                        <span class="data-label">Тангаж:</span>
                        <span class="data-value">)" << std::fixed << std::setprecision(1) << telemetry_data.attitude_pitch << R"(°</span>
                    </div>
                    <div class="data-row">
                        <span class="data-label">Рысканье:</span>
                        <span class="data-value">)" << std::fixed << std::setprecision(1) << telemetry_data.attitude_yaw << R"(°</span>
                    </div>
                    <div class="data-row">
                        <span class="data-label">Режим полета:</span>
                        <span class="data-value">)" << telemetry_data.flight_mode << R"(</span>
                    </div>
                </div>
            </div>
            
            <div class="card">
                <h3>🎮 RC Каналы</h3>
                <div class="channels-grid">)";
        
        for (int i = 0; i < 16; i++) {
            html << R"(
                    <div class="channel-item">
                        <div class="channel-number">CH)" << (i + 1) << R"(</div>
                        <div class="channel-value">)" << telemetry_data.channels[i] << R"(</div>
                    </div>)";
        }
        
        html << R"(
                </div>
            </div>
            
            <div class="refresh-info">
                <p>Данные обновляются автоматически каждую секунду без перезагрузки страницы</p>
                <p>Время сервера: )" << telemetry_data.timestamp << R"(</p>
            </div>
        </div>
    </div>
    
    <script>
        // Простое AJAX обновление каждую секунду
        function updateData() {
            fetch('/api/telemetry')
                .then(response => response.json())
                .then(data => {
                    console.log('Получены данные:', data);
                    
                    // Обновляем статус связи
                    const linkStatus = document.getElementById('link-status');
                    const linkText = document.getElementById('link-text');
                    if (linkStatus && linkText) {
                        linkStatus.className = 'status-indicator ' + (data.link_up ? 'online' : '');
                        linkText.textContent = data.link_up ? 'Активна' : 'Потеряна';
                    }
                    
                    // Обновляем активный порт
                    const portElement = document.querySelector('.status-item:nth-child(2) strong');
                    if (portElement) {
                        portElement.textContent = data.active_port;
                    }
                    
                    // Обновляем время последнего обновления
                    const updateElement = document.querySelector('.status-item:nth-child(3) strong');
                    if (updateElement) {
                        updateElement.textContent = data.last_update;
                    }
                    
                    // Обновляем данные батареи
                    const allCards = document.querySelectorAll('.card');
                    for (let card of allCards) {
                        const h3 = card.querySelector('h3');
                        if (h3 && h3.textContent.includes('Батарея')) {
                            const batteryRows = card.querySelectorAll('.data-row');
                            if (batteryRows.length >= 4) {
                                batteryRows[0].querySelector('.data-value').textContent = data.battery.voltage + ' В';
                                batteryRows[1].querySelector('.data-value').textContent = data.battery.current + ' мА';
                                batteryRows[2].querySelector('.data-value').textContent = data.battery.capacity + ' мАч';
                                batteryRows[3].querySelector('.data-value').textContent = data.battery.remaining + ' %';
                            }
                            break;
                        }
                    }
                    
                    // Обновляем данные положения
                    for (let card of allCards) {
                        const h3 = card.querySelector('h3');
                        if (h3 && h3.textContent.includes('Положение')) {
                            const attitudeRows = card.querySelectorAll('.data-row');
                            if (attitudeRows.length >= 4) {
                                attitudeRows[0].querySelector('.data-value').textContent = data.attitude.roll + '°';
                                attitudeRows[1].querySelector('.data-value').textContent = data.attitude.pitch + '°';
                                attitudeRows[2].querySelector('.data-value').textContent = data.attitude.yaw + '°';
                                attitudeRows[3].querySelector('.data-value').textContent = data.flight_mode;
                            }
                            break;
                        }
                    }
                    
                    // Обновляем RC каналы
                    const channelItems = document.querySelectorAll('.channel-item');
                    for (let i = 0; i < Math.min(16, channelItems.length); i++) {
                        const channelValue = channelItems[i].querySelector('.channel-value');
                        if (channelValue) {
                            channelValue.textContent = data.channels[i];
                        }
                    }
                })
                .catch(error => {
                    console.error('Ошибка получения данных:', error);
                });
        }
        
        // Запускаем обновление каждую секунду
        setInterval(updateData, 1000);
        
        // Первое обновление сразу
        updateData();
    </script>
</body>
</html>)";
        
        return html.str();
    }
    
    std::string generateJSON() {
        std::lock_guard<std::mutex> lock(telemetry_mutex);
        
        std::stringstream json;
        json << "{";
        json << "\"timestamp\":\"" << telemetry_data.timestamp << "\",";
        json << "\"link_up\":" << (telemetry_data.link_up ? "true" : "false") << ",";
        json << "\"active_port\":\"" << telemetry_data.active_port << "\",";
        json << "\"flight_mode\":\"" << telemetry_data.flight_mode << "\",";
        json << "\"battery\":{";
        json << "\"voltage\":" << std::fixed << std::setprecision(2) << telemetry_data.battery_voltage << ",";
        json << "\"current\":" << telemetry_data.battery_current << ",";
        json << "\"capacity\":" << telemetry_data.battery_capacity << ",";
        json << "\"remaining\":" << telemetry_data.battery_remaining;
        json << "},";
        json << "\"attitude\":{";
        json << "\"pitch\":" << std::fixed << std::setprecision(1) << telemetry_data.attitude_pitch << ",";
        json << "\"roll\":" << std::fixed << std::setprecision(1) << telemetry_data.attitude_roll << ",";
        json << "\"yaw\":" << std::fixed << std::setprecision(1) << telemetry_data.attitude_yaw;
        json << "},";
        json << "\"channels\":[";
        for (int i = 0; i < 16; i++) {
            json << telemetry_data.channels[i];
            if (i < 15) json << ",";
        }
        json << "],";
        json << "\"last_update\":\"" << telemetry_data.last_update << "\"";
        json << "}";
        
        return json.str();
    }
    
    void start() {
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);
        
        // Создаем сокет
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            log_error("Ошибка создания сокета");
            return;
        }
        
        // Настраиваем сокет
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            log_error("Ошибка настройки сокета");
            return;
        }
        
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        
        // Привязываем сокет
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            log_error("Ошибка привязки сокета к порту " + std::to_string(port));
            return;
        }
        
        // Слушаем соединения
        if (listen(server_fd, 3) < 0) {
            log_error("Ошибка прослушивания порта");
            return;
        }
        
        log_info("Веб-сервер телеметрии запущен на порту " + std::to_string(port));
        log_info("Откройте браузер: http://localhost:" + std::to_string(port));
        
        // Запускаем фоновый поток для обновления телеметрии
        std::thread telemetry_thread([this]() {
            while (true) {
                updateTelemetry();
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Обновляем каждые 100мс
            }
        });
        telemetry_thread.detach();
        
        // Основной цикл сервера
        while (true) {
            int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            if (new_socket < 0) {
                log_error("Ошибка принятия соединения");
                continue;
            }
            
            // Читаем HTTP запрос
            char buffer[1024] = {0};
            read(new_socket, buffer, 1024);
            std::string request(buffer);
            
            std::string response;
            
            // Проверяем, это API запрос или обычная страница
            if (request.find("GET /api/telemetry") != std::string::npos) {
                // API запрос - возвращаем JSON
                std::string json = generateJSON();
                response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: application/json; charset=utf-8\r\n"
                          "Access-Control-Allow-Origin: *\r\n"
                          "Content-Length: " + std::to_string(json.length()) + "\r\n"
                          "Connection: close\r\n\r\n" + json;
            } else {
                // Обычный запрос - возвращаем HTML
                std::string html = generateHTML();
                response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/html; charset=utf-8\r\n"
                          "Content-Length: " + std::to_string(html.length()) + "\r\n"
                          "Connection: close\r\n\r\n" + html;
            }
            
            send(new_socket, response.c_str(), response.length(), 0);
            close(new_socket);
        }
    }
    
    ~TelemetryServer() {
        if (server_fd > 0) {
            close(server_fd);
        }
    }
};

// Глобальная переменная для сервера
TelemetryServer* telemetry_server = nullptr;

// Функции для обновления данных телеметрии (вызываются из обработчиков пакетов)
void updateTelemetryBattery(double voltage, int current, int capacity, int remaining) {
    std::lock_guard<std::mutex> lock(global_telemetry.data_mutex);
    global_telemetry.battery_voltage = voltage;
    global_telemetry.battery_current = current;
    global_telemetry.battery_capacity = capacity;
    global_telemetry.battery_remaining = remaining;
}

void updateTelemetryAttitude(double pitch, double roll, double yaw) {
    std::lock_guard<std::mutex> lock(global_telemetry.data_mutex);
    global_telemetry.attitude_pitch = pitch;
    global_telemetry.attitude_roll = roll;
    global_telemetry.attitude_yaw = yaw;
}

void updateTelemetryFlightMode(const std::string& mode) {
    std::lock_guard<std::mutex> lock(global_telemetry.data_mutex);
    global_telemetry.flight_mode = mode;
}

// Функция для запуска веб-сервера в отдельном потоке
void startTelemetryServer(CrsfSerial* crsf, int port = 8080) {
    telemetry_server = new TelemetryServer(port);
    telemetry_server->setCrsfPointer(crsf);
    
    // Запускаем сервер в отдельном потоке
    std::thread server_thread([telemetry_server]() {
        telemetry_server->start();
    });
    
    server_thread.detach();
    log_info("Веб-сервер телеметрии запущен в отдельном потоке");
}
