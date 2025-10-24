#pragma once

#include <cstdint>
#include <cstddef>

// Простая обёртка над Linux joystick API (/dev/input/jsX)
// Неблокирующее чтение событий, хранение текущих состояний осей/кнопок

// Открыть джойстик. path по умолчанию "/dev/input/js0". Возвращает true при успехе
bool js_open(const char* path = "/dev/input/js0");

// Закрыть джойстик
void js_close();

// Прочитать доступные события (неблокирующее). Возвращает true, если что-то обработано
bool js_poll();

// Получить текущее значение оси (диапазон примерно [-32767..32767]).
// Возвращает true, если ось присутствует
bool js_get_axis(int index, int16_t& outValue);

// Получить количество известных осей/кнопок (по данным из событий)
int js_num_axes();
int js_num_buttons();


