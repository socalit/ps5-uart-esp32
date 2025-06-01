# ps5-uart-esp32

A compact UART diagnostic tool for PlayStation 5, based on ESP32 and SSD1306 OLED. Captures PS5 serial output, displays recent log lines on a 128×32 OLED, and provides button-controlled scrolling, log clearing, and reset functionality. LEDs give visual feedback for UART RX/TX and power status.

## Features

- **PS5 UART Sniffing**  
  Listens on UART2 (RX pin 16) at 115200 baud. Buffers incoming lines (up to 10) and shows timestamps.

- **OLED Display**  
  128×32 monochrome SSD1306 shows:
  - “Waiting for data…” loading bar until first line arrives  
  - Most recent two log lines with horizontal scrolling for long text  
  - A brief “RX Data Feed” indicator when a new line arrives  
  - A fixed bottom line “PS Monitor UART”  

- **Button Controls**  
  - **Up (GPIO 32)** / **Down (GPIO 33)**: Scroll through buffered log entries  
  - **Clear (GPIO 25)**: Hold 2 seconds to erase all logs and return to splash screen  
  - **Reset (GPIO 26)**: Show splash animation and redisplay last logs  

- **Splash Animation**  
  On power-up or Reset, draws simple PlayStation-style icons (triangle, circle, “X”, square) in sequence.

- **LED Feedback**  
  - **LED_TX (GPIO 12)**: Brief blink when forwarding each byte to USB serial  
  - **LED_RX (GPIO 13)**: Brief blink on each incoming UART byte  
  - **LED_PWR (GPIO 14)**: Steady ON to indicate power  

- **USB Passthrough**  
  Mirrors all incoming UART data to USB Serial at 115200 baud (for PC logging).

## Hardware Requirements

- **ESP32 Dev Board** (e.g., ESP-WROOM-32)  
- **0.91" SSD1306 OLED** (I2C, 128×32)  
- **Four Pushbuttons**  
  - Up → GPIO 32  
  - Down → GPIO 33  
  - Clear → GPIO 25  
  - Reset → GPIO 26  
- **Three LEDs** (with current-limiting resistors)  
  - LED_TX → GPIO 12  
  - LED_RX → GPIO 13  
  - LED_PWR → GPIO 14  
- **Optional Level Shifter**  
  - If PS5 serial runs at 3.3 V for ESP32 RX.  
- **Wiring Wires, Breadboard or PCB** as needed.

## Wiring

1. **OLED Display (I2C)**  
   - VCC → 3.3 V  
   - GND → GND  
   - SCL → GPIO 22 (default I2C SCL on most ESP32 boards)  
   - SDA → GPIO 21 (default I2C SDA)  

2. **Buttons** (configured as INPUT_PULLUP)  
   - _BTN_UP_ (scroll up): GPIO 32 → one side of pushbutton; other side → GND  
   - _BTN_DOWN_ (scroll down): GPIO 33 → one side of pushbutton; other side → GND  
   - _BTN_CLEAR_ (hold to clear): GPIO 25 → one side of pushbutton; other side → GND  
   - _BTN_RESET_ (reset display): GPIO 26 → one side of pushbutton; other side → GND  

3. **LEDs** (active HIGH, use ~220 Ω resistor in series)  
   - _LED_TX_: GPIO 12 → resistor → LED → GND  
   - _LED_RX_: GPIO 13 → resistor → LED → GND  
   - _LED_PWR_: GPIO 14 → resistor → LED → GND  

4. **PS5 UART Source**  
   - Connect PS5 UART TX pin (3.3 V TTL) → ESP32 RX2 (GPIO 16) 

5. **USB Serial**  
   - ESP32’s built-in USB port is used for power and USB Serial passthrough (TX/RX). No extra wiring needed.

## Software Setup

1. **Libraries**  
   - Install **Adafruit SSD1306** (version >= 2.x)  
   - Install **Adafruit GFX Library** (version >= 1.x)  

   In Arduino IDE:
   1. Go to **Sketch → Include Library → Manage Libraries…**  
   2. Search for “SSD1306” and install “Adafruit SSD1306 by Adafruit”  
   3. Search for “GFX” and install “Adafruit GFX Library by Adafruit”  

2. **Clone or Download**  
   ```bash
   git clone https://github.com/socalit/ps5-uart-esp32.git
   cd ps5-uart-esp32
