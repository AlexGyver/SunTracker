[![Foo](https://img.shields.io/badge/Version-1.2.1-brightgreen.svg?style=flat-square)](#versions)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD$%E2%82%AC%20%D0%9D%D0%B0%20%D0%BF%D0%B8%D0%B2%D0%BE-%D1%81%20%D1%80%D1%8B%D0%B1%D0%BA%D0%BE%D0%B9-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)

# EEManager
Менеджер EEPROM - библиотека для уменьшения износа памяти
- Отложенная запись (обновление) по таймеру
- Работает на базе стандартной EEPROM.h
- Встроенный механизм ключа первой записи

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)
- Для esp8266 не забудь вызвать EEPROM.begin(размер)!

## Содержание
- [Установка](#install)
- [Инициализация](#init)
- [Использование](#usage)
- [Пример](#example)
- [Версии](#versions)
- [Баги и обратная связь](#feedback)

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **EEManager** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/EEManager/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

<a id="init"></a>
## Инициализация
```cpp
// передаём нашу переменную любого типа (фактически её адрес) - структура, массив, что угодно
EEManager memory(data);

// можно задать таймаут обновления, по умолч. 5 секунд (5000мс)
EEManager memory(data, 1000);
```

<a id="usage"></a>
## Использование
```cpp
// Начать работу, прочитать данные в переменную. 
// Принимает адрес начала хранения даты и ключ
// Возвращает:
// 0 - ключ совпал, данные прочитаны из епром
// 1 - ключ не совпал (первый запуск), данные записаны в епром
// 2 - ошибка, в епроме не хватает места
uint8_t begin(uint8_t addr, uint8_t key);

void setTimeout(uint16_t tout = 5000);      // сменить таймаут
void updateNow();       // обновить данные в еепром сейчас
void update();          // отложить обновление и сбросить таймер
bool tick();            // тикер обновления
void reset();           // сбросить ключ запуска. При перезагрузке (или вызове begin) запишутся стандартные данные 
uint16_t dataSize();    // получить размер данных
uint16_t blockSize();   // получить размер всего блока (данные + ключ)
uint16_t startAddr();   // получить адрес первого байта в блоке
uint16_t endAddr();     // получить адрес последнего байта в блоке (включая ключ)
```

<a id="example"></a>
## Пример
```cpp
// структура для хранения данных
struct Data {
  char val = 'H';
  char str[15] = "ello kitty!";
};
Data data;  // переменная, с которой мы работаем в программе

#include <EEManager.h>  // подключаем либу
EEManager memory(data); // передаём нашу переменную (фактически её адрес)

// можно задать таймаут обновления, по умолч. 5 секунд (5000мс)
//EEManager memory(data, 1000);

void setup() {
  Serial.begin(9600);
  // для esp8266 не забудь вызвать EEPROM.begin(размер)!
  /*
    Запускаем менеджер, передаём:
    - Стартовый адрес в памяти для записи даты
    - Ключ хранения (0.. 255) или символ
  */
  byte stat = memory.begin(0, 'b');

  /*
    Коды возврата:
    0 - ключ совпал, данные прочитаны из епром
    1 - ключ не совпал (первый запуск), данные записаны в епром
    2 - ошибка, в епроме не хватает места
  */
  Serial.println(stat);

  // выведем в порт всё содержимое блока памяти, включая ключ
  for (uint16_t i = memory.startAddr(); i < memory.endAddr() + 1; i++)
    Serial.write(EEPROM.read(i));
  Serial.println();
  
  data.val = 'X';     // изменим данные в переменной
  memory.update();    // отложим обновление

  // примечание: если нужно создать ещё один блок следующим в памяти -
  // запускаем его на адресе memory.endAddr() + 1, это первый
  // свободный байт после первого блока
}

void loop() {
  // в лупе вызываем tick(), в нём по таймеру произойдёт обновление
  // функция вернёт true, когда это случится
  if (memory.tick()) Serial.println("Updated!");

  // выведем в порт всё содержимое блока памяти, включая ключ
  for (uint16_t i = memory.startAddr(); i < memory.endAddr() + 1; i++)
    Serial.write(EEPROM.read(i));
  Serial.println();

  // таймер сброса на 10 секунд: через 10 секунд
  // произойдёт сброс ключа данных, и при следующем запуске
  // в епром будут записаны данные по умолчанию (то что в ярлыке структуры)
  static uint32_t tmr;
  if (millis() - tmr >= 10000) {
    memory.reset();
    Serial.println("reset");
    for (;;);
  }
  
  delay(1000);    // чилим
}
```

<a id="versions"></a>
## Версии
- v1.0
- v1.1 - изменены коды возврата begin
- v1.2 - добавлена nextAddr()
- v1.2.1 - поддержка esp32

<a id="feedback"></a>
## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!
