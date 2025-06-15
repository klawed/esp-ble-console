#include <Arduino.h>
#include "BluetoothSerial.h"

// Pin definitions
#define STATUS_LED_PIN 2
#define RX_PIN 16  // ESP32 RX (connects to Pi TX)
#define TX_PIN 17  // ESP32 TX (connects to Pi RX)

// Bluetooth setup
BluetoothSerial SerialBT;
HardwareSerial PiSerial(2);  // Use UART2

// Status indicators
bool btConnected = false;
unsigned long lastStatusBlink = 0;
bool statusLedState = false;

// Buffer for data throughput
#define BUFFER_SIZE 1024
uint8_t buffer[BUFFER_SIZE];

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize status LED
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  
  // Initialize Pi UART (115200 is standard for Pi console)
  PiSerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  
  // Initialize Bluetooth with a descriptive name
  if (!SerialBT.begin("Pi Console Bridge")) {
    Serial.println("Failed to initialize Bluetooth");
    // Blink error pattern
    while(1) {
      digitalWrite(STATUS_LED_PIN, HIGH);
      delay(100);
      digitalWrite(STATUS_LED_PIN, LOW);
      delay(100);
    }
  }
  
  Serial.println("ESP32 Bluetooth UART Bridge");
  Serial.println("=============================");
  Serial.println("Device: Pi Console Bridge");
  Serial.println("Waiting for Bluetooth connection...");
  Serial.println("");
  Serial.println("Hardware connections:");
  Serial.println("ESP32 GPIO16 (RX) -> Pi GPIO14 (TX, Pin 8)");
  Serial.println("ESP32 GPIO17 (TX) -> Pi GPIO15 (RX, Pin 10)");
  Serial.println("ESP32 GND        -> Pi GND (Pin 6)");
  
  // Register Bluetooth callback
  SerialBT.register_callback([](esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    if (event == ESP_SPP_SRV_OPEN_EVT) {
      btConnected = true;
      Serial.println("Bluetooth client connected");
    } else if (event == ESP_SPP_CLOSE_EVT) {
      btConnected = false;
      Serial.println("Bluetooth client disconnected");
    }
  });
}

void updateStatusLED() {
  unsigned long currentTime = millis();
  
  if (btConnected) {
    // Solid on when connected
    digitalWrite(STATUS_LED_PIN, HIGH);
  } else {
    // Slow blink when waiting for connection
    if (currentTime - lastStatusBlink >= 1000) {
      statusLedState = !statusLedState;
      digitalWrite(STATUS_LED_PIN, statusLedState);
      lastStatusBlink = currentTime;
    }
  }
}

void loop() {
  updateStatusLED();
  
  // Pi -> Bluetooth (forward Pi output to BT client)
  if (PiSerial.available()) {
    size_t bytesRead = PiSerial.readBytes(buffer, min(PiSerial.available(), BUFFER_SIZE));
    if (btConnected && bytesRead > 0) {
      SerialBT.write(buffer, bytesRead);
    }
    // Also echo to USB serial for debugging
    Serial.write(buffer, bytesRead);
  }
  
  // Bluetooth -> Pi (forward BT client input to Pi)
  if (SerialBT.available()) {
    size_t bytesRead = SerialBT.readBytes(buffer, min(SerialBT.available(), BUFFER_SIZE));
    if (bytesRead > 0) {
      PiSerial.write(buffer, bytesRead);
      // Echo to USB serial for debugging
      Serial.print("BT->Pi: ");
      Serial.write(buffer, bytesRead);
    }
  }
  
  // USB Serial -> Pi (for local debugging)
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'i') {
      Serial.println("\nESP32 Bluetooth Console Bridge Status:");
      Serial.println("=====================================");
      Serial.printf("Bluetooth connected: %s\n", btConnected ? "Yes" : "No");
      Serial.printf("Pi UART baud: %d\n", 115200);
      Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
      Serial.println("Send 'i' for info, 'r' to restart");
    } else if (cmd == 'r') {
      Serial.println("Restarting...");
      ESP.restart();
    } else {
      // Forward other commands to Pi
      PiSerial.write(cmd);
    }
  }
  
  delay(1);  // Small delay to prevent watchdog issues
}
