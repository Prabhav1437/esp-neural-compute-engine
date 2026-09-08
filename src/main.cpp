#include <Arduino.h>
#include "CommandProcessor.h"

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  delay(1000);
  
  Serial.println("\n==================================================");
  Serial.println(" ESP32 Modular Unified Math & Matrix Engine      ");
  Serial.println("==================================================");
  Serial.println("Supports Normal Operations (+, -, *, /, %, ^, sqrt, abs, sin, cos)");
  Serial.println("Supports Matrix Operations (+, -, *, scalar mult)");
  Serial.println("--------------------------------------------------");
}

void loop() {
  if (Serial.available() > 0) {
    String inputStr = Serial.readStringUntil('\n');
    processCommand(inputStr);
  }
}
