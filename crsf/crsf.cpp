// Реализация логики CRSF: приём каналов, переключение линка, телеметрия
/* Arduino-специфичный код. Отключено для сборки на Raspberry Pi.
#include "crsf.h"

// Компилируем только если задействован приём или передача CRSF
// Компилируем только если задействован приём или передача CRSF
#if USE_CRSF_RECV == true || USE_CRSF_SEND == true
#include "libs/crsf/CrsfSerial.h"

HardwareTimer timer(TIM4);
HardwareSerial Serial_1(USART1);
#define softSerial Serial_1

// Два интерфейса CRSF: основной (Serial2) и резервный (softSerial)
CrsfSerial crsf_1(Serial2, CRSF_BAUDRATE);
CrsfSerial crsf_2(softSerial, CRSF_BAUDRATE);
// Текущий активный линк (по умолчанию основной)
CrsfSerial *crsf = &crsf_1;

#if PIN_INIT == true
uint32_t old_time_rele1 = 0;
uint32_t old_time_rele2 = 0;
/* конец Arduino-части */
#endif
*/
// Метка времени для контроля активности по UART/CRSF
uint32_t old_time_uart = 0;

// Колбэк: обработка входящих каналов CRSF
void packetChannels()
{
  static int16_t origCh1;
  static int16_t origCh2;
  static int16_t origCh5;
  static int16_t origCh8;
  static int16_t ch1;
  static int16_t ch2;

  // Читаем каналы стиков/переключателей
  origCh1 = crsf->getChannel(1);
  origCh2 = crsf->getChannel(2);
  origCh5 = crsf->getChannel(5);
  origCh8 = crsf->getChannel(8);
  old_time_uart = millis(); // обновляем время последней активности

#if DEVICE_1 == true
  // Нормализация диапазона под управление дифференциальным приводом
  origCh2 = origCh2 / 2 - 750;
  origCh1
  
   = origCh1 / 2 - 750;
  ch1 = (origCh1 - origCh2) / 2;
  ch2 = -
  (origCh1 + origCh2) / 2;

  // Мёртвая зона для подавления шума вокруг центра
#define DEAD_ZONE 50
  if (ch1 > -DEAD_ZONE && ch1 < DEAD_ZONE)
  {
    ch1 = 0;
  }

  if (ch2 > -DEAD_ZONE && ch2 < DEAD_ZONE)
  {
    ch2 = 0;
  }

  // Управление мотором 1: направление через digital, величина через analogWrite
  if (ch1 < 0)
  {
    digitalWrite(motor_1_digital, HIGH);
    analogWrite(motor_1_analog, -ch1);
  }
  else
  {
    digitalWrite(motor_1_digital, LOW);
    analogWrite(motor_1_analog, ch1);
  }

  // Управление мотором 2 аналогично
  if (ch2 < 0)
  {
    digitalWrite(motor_2_digital, HIGH);
    analogWrite(motor_2_analog, -ch2);
  }
  else
  {
    digitalWrite(motor_2_digital, LOW);
    analogWrite(motor_2_analog, ch2);
  }
#elif DEVICE_2 == true
  // Аппаратный PWM: задаём импульс в мкс (1000..2000)
  timer.setCaptureCompare(timer_ch_motor_1, origCh2, MICROSEC_COMPARE_FORMAT);
  timer.setCaptureCompare(timer_ch_motor_2, origCh1, MICROSEC_COMPARE_FORMAT);
#endif
#if PIN_INIT == true
  // Логика удержания кнопки: если переключатель активен > 3с — включить реле
  if (origCh5 > 1800)
  {
    if (old_time_rele1 == 0)
    {
      old_time_rele1 = millis();
    }
  }
  else
  {
    if (old_time_rele1 > 0)
    {
      old_time_rele1 = 0;
      digitalWrite(rele_1, LOW);
    }
  }
  if (origCh8 > 1800)
  {
    if (old_time_rele2 == 0)
    {
      old_time_rele2 = millis();
    }
  }
  else
  {
    if (old_time_rele2 > 0)
    {
      old_time_rele2 = 0;
      digitalWrite(rele_2, LOW);
    }
  }
#endif
}

// Индикация поднятия линка: гасим LED (активен низкий уровень)
static void crsfLinkUp()
{
  digitalWrite(LED_BUILTIN, LOW);
}

// Падение линка на резервном канале: переключаемся на основной
static void crsfLinkDown_2()
{
  crsf = &crsf_1;
  digitalWrite(camera, LOW);
  old_time_uart = millis();
}

// Падение линка на основном канале: переключаемся на резервный
static void crsfLinkDown()
{
  crsf = &crsf_2;
  digitalWrite(camera, HIGH);
  old_time_uart = millis();
}

// Поставить значение канала для отправки на основной линк
void crsfSetChannel(unsigned int ch, int value)
{
  crsf_1.setChannel(ch, value);
}

// Отправить пакет с каналами на приёмник
void crsfSendChannels()
{
  crsf_1.packetChannelsSend();
}

// Периодический обработчик (вызывать часто): таймаут линка, failsafe, crsf->loop()
void loop_ch()
{
  static uint32_t newTime;
  newTime = millis();
  // Если долго нет активности — переключаем линк (попытка восстановления)
  if (newTime - old_time_uart > 60000)
  {
    if (crsf == &crsf_1)
    {
      crsfLinkDown();
    }
    else
    {
      crsfLinkDown_2();
    }
  }
  #if PIN_INIT == true
  // Включение реле после удержания > 3 секунд
  if (old_time_rele1 > 0)
  {
    if (newTime - old_time_rele1 > 3000)
    {
      digitalWrite(rele_1, HIGH);
    }
  }
  if (old_time_rele2 > 0)
  {
    if (newTime - old_time_rele2 > 3000)

    {
      digitalWrite(rele_2, HIGH);
    }
  }
  #endif
  // Failsafe: если нет пакетов >100 мс — остановить/задать нейтраль
  if (newTime - crsf->_lastReceive > 100)
  {
    #if DEVICE_1 == true
        analogWrite(motor_1_analog, 0);
        analogWrite(motor_2_analog, 0);
    #elif DEVICE_2 == true
        timer.setCaptureCompare(timer_ch_motor_1, 1500, MICROSEC_COMPARE_FORMAT);
        timer.setCaptureCompare(timer_ch_motor_2, 1500, MICROSEC_COMPARE_FORMAT);
    #endif
  }
  // Внутренний цикл CRSF (приём/парсинг/колбэки)
  crsf->loop();
}

// Настройка таймера TIM4 для генерации PWM 50 Гц на PB6/PB7
void PWMinit()
{
  pinMode(motor_1_analog, OUTPUT); // Канал 1 TIM4 → PB6
  pinMode(motor_2_analog, OUTPUT); // Канал 2 TIM4 → PB7
  timer.pause();
  timer.setPrescaleFactor(72);               // 1 МГц (72 МГц / 72)
  timer.setOverflow(20000, MICROSEC_FORMAT); // Период 20 мс (50 Гц)
  // Настройка каналов
  timer.setMode(timer_ch_motor_1, TIMER_OUTPUT_COMPARE_PWM1, motor_1_analog); // Канал 1 → PB6
  timer.setCaptureCompare(timer_ch_motor_1, 1500, MICROSEC_COMPARE_FORMAT);
  timer.setMode(timer_ch_motor_2, TIMER_OUTPUT_COMPARE_PWM1, motor_2_analog); // Канал 2 → PB7
  timer.setCaptureCompare(timer_ch_motor_2, 1500, MICROSEC_COMPARE_FORMAT);
  timer.resume(); // Запуск таймера                             // Запускаем таймер
}

// Инициализация программного управления моторами (обнуляем ШИМ, настраиваем направления)
void analogInit()
{
  analogWrite(motor_1_analog, 0);
  analogWrite(motor_2_analog, 0);
  pinMode(motor_2_digital, OUTPUT);
  pinMode(motor_1_digital, OUTPUT);
}

// Инициализация пинов реле и камеры, установка безопасных начальных уровней
void pinInit()
{
  pinMode(camera, OUTPUT);
  pinMode(rele_1, OUTPUT);
  pinMode(rele_2, OUTPUT);
  digitalWrite(camera, LOW);
  digitalWrite(rele_2, LOW);
  digitalWrite(rele_1, LOW);
}

// Настройка портов и колбэков CRSF для приёма каналов на двух UARTах
void crsfInitRecv()
{
  Serial2.begin(CRSF_BAUD);
  softSerial.begin(CRSF_BAUD);
  crsf_2.onPacketChannels = &packetChannels;
  crsf_2.onLinkUp = &crsfLinkUp;
  crsf_2.onLinkDown = &crsfLinkDown_2;
  crsf_1.onPacketChannels = &packetChannels;
  crsf_1.onLinkUp = &crsfLinkUp;
  crsf_1.onLinkDown = &crsfLinkDown;
}

// Инициализация UART для отправки телеметрии CRSF через Serial
void crsfInitSend()
{
  Serial.begin(CRSF_BAUD);
}

// Формат пакета CRSF для датчика батареи
struct packet_CRSF_FRAMETYPE_BATTERY_SENSOR
{
  uint16_t voltage;
  uint16_t current;
  uint capacity : 24;
  uint8_t remaining;
};

#define swap2Bytes(x) ((((x) >> 8) & 0xFF) | ((x) << 8)) // big-endian <-> little-endian

// Отправить телеметрию батареи по CRSF (напряжение/ток/ёмкость/процент)
void crsfTelemetrySend()
{
  static packet_CRSF_FRAMETYPE_BATTERY_SENSOR packet;
  static uint16_t kR = (47000 + 3300) / 33;
  packet.remaining = 0;
  uint16_t v = (uint16_t)((27.8 * 1000) / 100);

  v = analogRead(A1) * 3.3 * 10 * kR / 1024 / 100;
  uint16_t c = 0;
  packet.voltage = swap2Bytes(v);
  packet.current = swap2Bytes(c);
  uint32_t capacity = 0;
  packet.capacity = capacity;
  packet.current = (uint16_t)((7.3 * 1000) * 100);
  crsf_1.queuePacket(CRSF_ADDRESS_FLIGHT_CONTROLLER, CRSF_FRAMETYPE_BATTERY_SENSOR, (void *)&packet, 8);
}
#endif
