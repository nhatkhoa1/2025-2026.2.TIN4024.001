#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// --- CẤU HÌNH CHÂN THEO DIAGRAM CỦA KHOA ---
#define DHTPIN 16
#define DHTTYPE DHT22

#define LED_GREEN  15  // led1 (Cyan trong diagram)
#define LED_YELLOW 2   // led3
#define LED_RED    4   // led2

#define OLED_SDA 13
#define OLED_SCL 12

// Cấu hình OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  // Thiết lập chân LED
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // Khởi tạo I2C cho OLED với chân Khoa đã nối
  Wire.begin(OLED_SDA, OLED_SCL);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED failed"));
    for(;;);
  }

  dht.begin();
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void processAlert(float temp, String &status, int &activeLed) {
  if (temp < 13) {
    status = "TOO COLD";
    activeLed = LED_GREEN;
  } else if (temp < 20) {
    status = "COLD";
    activeLed = LED_GREEN;
  } else if (temp < 25) {
    status = "COOL";
    activeLed = LED_YELLOW;
  } else if (temp < 30) {
    status = "WARM";
    activeLed = LED_YELLOW;
  } else if (temp <= 35) {
    status = "HOT";
    activeLed = LED_RED;
  } else {
    status = "TOO HOT";
    activeLed = LED_RED;
  }
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    return;
  }

  String status = "";
  int activeLed = -1;
  processAlert(t, status, activeLed);

  // --- HIỂN THỊ OLED ---
  display.clearDisplay();
  
  // Hiển thị nhiệt độ & độ ẩm
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temp: "); display.print(t, 1); display.print(" C");
  display.setCursor(0, 15);
  display.print("Hum:  "); display.print(h, 1); display.print(" %");

  // Hiển thị trạng thái (Chữ to ở giữa)
  display.setTextSize(2);
  display.setCursor(0, 40);
  display.print(status);
  display.display();

  // --- HIỆU ỨNG LED NHẤP NHÁY ---
  // Tắt tất cả LED
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  // Nhấp nháy LED tương ứng với ngưỡng
  if (activeLed != -1) {
    digitalWrite(activeLed, HIGH);
    delay(300);
    digitalWrite(activeLed, LOW);
    delay(300);
  }
}