# bmp180-Sensor-mit-Anzeige
In diesem kleinen Projekt wird auf 3 MAX7219 Matrizen die Temperatur oder die Altitude, gemessen von einem BMP180 Sensor, angezeigt. Ebenfalls werden die Werte der Temperatur, Luftdruck und Altitude parallel auf einem SSD1306 OLED Display parallel angezeigt. Mit einem Knopf kann man die Matrix-Anzeige zwischen °C/°F/Altitude wechseln. Die Altitude ist wegen der drei Matrizen auf 999 limitiert. Für das OLED-Display wurde die Library von afiskon verwendet. Die jeweiligen Librarys wurden gemäß den Anweisungen der Datasheets des MAX7219 und BMP180 erstellt.

Genutzte Hardware:

1X ST NUCLEO-F303RE MCU
3x MAX7219 8x8 LED MATRIX
1x SSD1306 OLED
1x BMP180 SENSOR
1x KNOPF

OLED Library:
https://github.com/afiskon/stm32-ssd1306
