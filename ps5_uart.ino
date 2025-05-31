#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Buttons
#define BTN_UP 32
#define BTN_DOWN 33
#define BTN_CLEAR 25
#define BTN_RESET 26

// UART Pins
#define RXD2 16
#define TXD2 -1

// LED Feedback
#define LED_TX 12
#define LED_RX 13
#define LED_PWR 14

const int maxMessages = 10;
String messages[maxMessages];
String timestamps[maxMessages];
int messageIndex = 0;
int scrollOffset = 0;
int scrollStart[maxMessages] = {0};

unsigned long lastBtnUp = 0;
unsigned long lastBtnDown = 0;
const int debounceDelay = 200;

int loadWidth = 0;
unsigned long lastLoadUpdate = 0;
bool showRxFeed = false;

void splashScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  display.println("PlayStation");
  display.setCursor(38, 9);
  display.println("Monitor");

  display.drawTriangle(20, 31, 25, 22, 30, 31, WHITE); display.display(); delay(200);
  display.drawCircle(50, 26, 4, WHITE); display.display(); delay(200);
  display.drawLine(70, 22, 76, 30, WHITE); display.drawLine(70, 30, 76, 22, WHITE); display.display(); delay(200);
  display.drawRect(96, 22, 8, 8, WHITE); display.display(); delay(800);
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  // Button inputs
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_CLEAR, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);

  // LED outputs (grouped together)
  pinMode(LED_TX, OUTPUT);
  pinMode(LED_RX, OUTPUT);
  pinMode(LED_PWR, OUTPUT);
  digitalWrite(LED_TX, LOW);
  digitalWrite(LED_RX, LOW);
  digitalWrite(LED_PWR, HIGH);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1);
  }

  splashScreen();
  display.clearDisplay();
}

void blinkLED(int pin) {
  digitalWrite(pin, HIGH);
  delay(20);
  digitalWrite(pin, LOW);
}

void loop() {
  static String input = "";
  unsigned long now = millis();

  while (Serial2.available()) {
    char c = Serial2.read();
    if (Serial) { 
      Serial.write(c); 
      digitalWrite(LED_TX, HIGH); 
      delay(10); 
      digitalWrite(LED_TX, LOW); 
    }

    digitalWrite(LED_RX, HIGH);
    if (c == '\n') {
      messages[messageIndex] = input;
      timestamps[messageIndex] = formatTime(now);
      scrollStart[messageIndex] = 0;
      messageIndex = (messageIndex + 1) % maxMessages;
      scrollOffset = 0;
      input = "";
      showRxFeed = true;
      redraw();
    } else {
      input += c;
    }
    digitalWrite(LED_RX, LOW);
  }

  if (digitalRead(BTN_UP) == LOW && now - lastBtnUp > debounceDelay) {
    blinkLED(LED_PWR);
    lastBtnUp = now;
    if (scrollOffset < maxMessages - 2) scrollOffset++;
    redraw();
  }

  if (digitalRead(BTN_DOWN) == LOW && now - lastBtnDown > debounceDelay) {
    blinkLED(LED_PWR);
    lastBtnDown = now;
    if (scrollOffset > 0) scrollOffset--;
    redraw();
  }

  if (digitalRead(BTN_CLEAR) == LOW) {
    blinkLED(LED_PWR);
    unsigned long start = millis();
    display.clearDisplay();
    display.setCursor(10, 12);
    display.println("Hold to clear logs...");
    display.display();
    while (digitalRead(BTN_CLEAR) == LOW) {
      if (millis() - start > 2000) {
        for (int i = 0; i < maxMessages; i++) {
          messages[i] = "";
          timestamps[i] = "";
          scrollStart[i] = 0;
        }
        messageIndex = 0;
        scrollOffset = 0;
        splashScreen();
        return;
      }
    }
    redraw();
  }

  if (digitalRead(BTN_RESET) == LOW) {
    blinkLED(LED_PWR);
    splashScreen();
    redraw();
  }

  redraw();
  delay(50);
}

void redraw() {
  display.clearDisplay();

  if (messageIndex == 0) {
    display.setCursor(0, 0);
    display.println("Waiting for data...");

    if (millis() - lastLoadUpdate > 100) {
      loadWidth = (loadWidth + 4) % 126;
      lastLoadUpdate = millis();
    }
    display.drawRect(0, 10, 128, 4, WHITE);
    display.fillRect(1, 11, loadWidth, 2, WHITE);

  } else {
    int idx = messageIndex - 1 - scrollOffset;
    if (idx < 0) idx += maxMessages;

    for (int i = 0; i < 2; i++) {
      if (idx < 0 || timestamps[idx] == "") break;
      String full = timestamps[idx] + ": " + messages[idx];
      int maxLen = 21;
      int &startPos = scrollStart[idx];
      display.setCursor(0, i * 10);
      display.print(full.substring(startPos, (startPos + maxLen > full.length()) ? full.length() : startPos + maxLen));
      startPos = (startPos + 1) % (full.length() > maxLen ? full.length() - maxLen + 1 : 1);
      idx--;
      if (idx < 0) idx += maxMessages;
    }

    if (showRxFeed) {
      display.setCursor(0, 22);
      display.println("RX Data Feed");
      showRxFeed = false;
    }
  }

  display.setCursor(0, 24);
  display.println("PS Monitor UART");
  display.display();
}

String formatTime(unsigned long ms) {
  unsigned long s = ms / 1000;
  int h = (s / 3600) % 24;
  int m = (s / 60) % 60;
  int sec = s % 60;
  char buffer[9];
  sprintf(buffer, "%02d:%02d:%02d", h, m, sec);
  return String(buffer);
}