# LedStripRoom
Smart room system.

Hello, it is test.




Language Polish:

Cześć, tutaj będę wrzucał pliczki dotyczące sterowania "inteligentnym" pokojem.
Może ktoś coś wykorzysta we własnym projekcie :)

Całe sterowanie pokojem oparte jest o komunikacje CAN pomiędzy ESP32.
Informacje z zewnątrz przekazywane są poprzez strone www hostowaną na jednym z ESP32,
oraz port szeregowy wykorzystywany głównie do efektu ambilight telewizora.

Funkcje projektu:
-Sterowanie:
--ledowym podświetleniem sufitu (12 taśm led, po dwie w każdej listwie),
--głównym światłem w pomieszczeniu wraz z wentylatorem oraz jego podświetlaną obudową,
--podświetlanym TV, szafką, łóżkiem, grzejnikiem, karniszem,
--sterowanie automatycznym karniszem np. po otwarciu drzi balkonowych,
--automatyczne rolety,
itd

Wykorzystane biblioteki:
-Adafruit_NeoPixel.h
-DallasTemperature.h
-OneWire.h
-ESP32CAN.h
-CAN_config.h
-Arduino.h

![alt text](https://github.com/neeproject/LedStripRoom/blob/main/Photo/room_1.jpg)
![alt text](https://github.com/neeproject/LedStripRoom/blob/main/Photo/room_2.jpg)
![alt text](https://github.com/neeproject/LedStripRoom/blob/main/Photo/room_3.jpg)