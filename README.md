# Demo projects

## [Color Sensor](./color_sensor)

A demo of color sensor functionality. The runtime is read from the sensor the RGBA color values and set to the LED RGB the color(RED, GREEN, BLUE).

[![Color Sensor Video](https://img.youtube.com/vi/yMlIg9nEQwc/maxresdefault.jpg)](https://www.youtube.com/watch?v=yMlIg9nEQwc)

## [HTPP Request](./http_request)

Demonstrates how to send an HTTP request to a esp32 server and get the response. In this case the response of request is success after send servo move command.

[![HTPP Request Video](https://img.youtube.com/vi/geptVXy13Kw/maxresdefault.jpg)](https://www.youtube.com/watch?v=geptVXy13Kw)

## [Snake Game](./snake_game)

A simple snake game made with OLED display and capacitive touch. You can eat the food and grow longer, incresing the score and speed. The game is over when you hit the wall or yourself.

[![Snake Game Video](https://img.youtube.com/vi/6zYovL3dtoI/maxresdefault.jpg)](https://www.youtube.com/watch?v=6zYovL3dtoI)

### Itens:

-   Esp32.
-   Protoboard 400.
-   Servo MG90S 360 degree.
-   TCS34725 color sensor.
-   OLED display SH1106 128x64.
-   Some flat jumpers and solid cables.

### Librarys:

-   https://github.com/nhatuan84/esp32-sh1106-oled
-   https://github.com/adafruit/Adafruit_TCS34725
-   https://github.com/adafruit/Adafruit-GFX-Library
-   https://github.com/me-no-dev/ESPAsyncWebServer
-   https://www.arduino.cc/reference/en/libraries/servo/
-   https://github.com/me-no-dev/AsyncTCP
