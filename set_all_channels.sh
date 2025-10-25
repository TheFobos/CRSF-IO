#!/bin/bash

# Команда для установки всех 16 каналов CRSF
# Канал 1: 1100 мкс
# Канал 16: 1160 мкс
# Остальные каналы: 1500 мкс (центр)

API_URL="http://localhost:8081"

echo "🎮 Установка всех 16 каналов CRSF"
echo "================================="

# Переключение в ручной режим
echo "🔄 Переключение в ручной режим..."
curl -s "${API_URL}/api/command?cmd=setMode&value=manual" > /dev/null
if [ $? -eq 0 ]; then
    echo "✅ Режим изменен на: manual"
else
    echo "❌ Ошибка переключения режима"
    exit 1
fi

# Небольшая пауза для стабилизации
sleep 0.5

echo ""
echo "📡 Установка значений каналов..."

# Канал 1: 1100 мкс
echo "  CH1: 1100 мкс"
curl -s "${API_URL}/api/command?cmd=setChannel&value=1=1100" > /dev/null

# Каналы 2-15: 1500 мкс (центр)
for ch in {2..15}; do
    echo "  CH${ch}: 1500 мкс"
    curl -s "${API_URL}/api/command?cmd=setChannel&value=${ch}=1500" > /dev/null
done

# Канал 16: 1160 мкс
echo "  CH16: 1160 мкс"
curl -s "${API_URL}/api/command?cmd=setChannel&value=16=1160" > /dev/null

echo ""
echo "✅ Все каналы установлены!"

# Проверка текущих значений
echo ""
echo "🔍 Проверка установленных значений..."
response=$(curl -s "${API_URL}/api/telemetry")
if [ $? -eq 0 ]; then
    echo "📊 Текущие значения каналов:"
    echo "$response" | python3 -c "
import sys, json
try:
    data = json.load(sys.stdin)
    channels = data.get('channels', [])
    for i, val in enumerate(channels[:16], 1):
        print(f'  CH{i}: {val} мкс')
except:
    print('  Ошибка парсинга JSON')
"
else
    echo "❌ Ошибка получения телеметрии"
fi

echo ""
echo "🎯 Команда выполнена успешно!"
echo "💡 Для возврата к джойстику выполните:"
echo "   curl \"${API_URL}/api/command?cmd=setMode&value=joystick\""
