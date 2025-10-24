#!/usr/bin/env python3
"""
Тестовый скрипт для демонстрации CRSF API
Запускает простой HTTP сервер только с API endpoints
"""

import http.server
import socketserver
import json
import time
import random
from urllib.parse import urlparse, parse_qs

class APIHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/api/telemetry':
            # Генерируем тестовые данные телеметрии
            telemetry_data = {
                "linkUp": True,
                "activePort": "UART Test",
                "lastReceive": int(time.time() * 1000),
                "timestamp": time.strftime("%H:%M:%S"),
                "channels": [random.randint(1000, 2000) for _ in range(16)],
                "packetsReceived": random.randint(100, 1000),
                "packetsSent": random.randint(50, 500),
                "packetsLost": random.randint(0, 10),
                "gps": {
                    "latitude": 55.7558 + random.uniform(-0.01, 0.01),
                    "longitude": 37.6176 + random.uniform(-0.01, 0.01),
                    "altitude": 100 + random.uniform(-50, 50),
                    "speed": random.uniform(0, 50)
                },
                "battery": {
                    "voltage": 12.6 + random.uniform(-0.5, 0.5),
                    "current": 1500 + random.uniform(-200, 200),
                    "capacity": 5000 + random.uniform(-500, 500),
                    "remaining": random.randint(20, 100)
                },
                "attitude": {
                    "roll": random.uniform(-30, 30),
                    "pitch": random.uniform(-30, 30),
                    "yaw": random.uniform(0, 360)
                },
                "workMode": "joystick",
                "autoMode": False,
                "autoStep": 100,
                "autoInterval": 1000
            }
            
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(json.dumps(telemetry_data, indent=2).encode())
            
        elif self.path.startswith('/api/command'):
            # Обработка команд управления
            parsed_url = urlparse(self.path)
            query_params = parse_qs(parsed_url.query)
            
            cmd = query_params.get('cmd', [''])[0]
            value = query_params.get('value', [''])[0]
            
            print(f"Команда: {cmd} = {value}")
            
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(json.dumps({"status": "ok", "command": cmd, "value": value}).encode())
            
        elif self.path == '/' or self.path == '/index.html':
            # Информационная страница
            html = """<!DOCTYPE html>
<html><head><title>CRSF API</title></head>
<body>
<h1>CRSF Телеметрия API</h1>
<p>Доступные endpoints:</p>
<ul>
<li><a href="/api/telemetry">/api/telemetry</a> - JSON данные телеметрии</li>
<li><a href="/api/command">/api/command</a> - Команды управления</li>
</ul>
<h2>Примеры использования:</h2>
<pre>
# Получить телеметрию
curl http://localhost:8080/api/telemetry

# Установить режим
curl "http://localhost:8080/api/command?cmd=setMode&value=joystick"

# Установить канал
curl "http://localhost:8080/api/command?cmd=setChannel&value=1=1500"
</pre>
</body></html>"""
            
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(html.encode())
            
        else:
            self.send_response(404)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(b'404 Not Found')

def start_api_server(port=8080):
    """Запуск API сервера"""
    with socketserver.TCPServer(("", port), APIHandler) as httpd:
        print(f"🌐 CRSF API сервер запущен на порту {port}")
        print(f"📡 API доступен по адресу: http://localhost:{port}")
        print(f"📊 Телеметрия: http://localhost:{port}/api/telemetry")
        print("🛑 Нажмите Ctrl+C для остановки")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n🛑 Сервер остановлен")

if __name__ == "__main__":
    start_api_server()
