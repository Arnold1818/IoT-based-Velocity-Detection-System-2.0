#include <Wire.h>
#include <SoftwareSerial.h>

// Define the pins for the SIM800L module
#define SIM800_TX 10
#define SIM800_RX 11

// Define the pins for the IR sensors
int ir_s1 = 2; // Adjust the pin numbers to match your IR sensor connections
int ir_s2 = 3;

// Initialize SoftwareSerial for SIM800L communication
SoftwareSerial sim800(SIM800_TX, SIM800_RX);

// Define variables for timing and calculations
int timer1;
int timer2;
float timeElapsed;
const float distance = 0.15; // Distance between the two IR sensors in meters
float speed;

void setup() {
  pinMode(ir_s1, INPUT);
  pinMode(ir_s2, INPUT);
  Serial.begin(9600);
  sim800.begin(9600); // Set the baud rate for the SIM800L module
  
  Serial.println("Speed and Direction Monitor");
  
  // Initialize SIM800L
  sendCommand("AT");
  delay(1000);
  sendCommand("AT+CMGF=1"); // Set SMS to text mode
  delay(1000);
}

void loop() {
  // Check if the train is moving from ir_s1 to ir_s2
  if (digitalRead(ir_s1) == LOW && digitalRead(ir_s2) == HIGH) {
    timer1 = millis();
    while (digitalRead(ir_s2) == HIGH) {
      // Wait for the train to pass
    }
    timer2 = millis();
    timeElapsed = (timer2 - timer1) / 1000.0; // Convert milliseconds to seconds

    // Calculate speed (v = d / t)
    speed = distance / timeElapsed;
    // Convert speed to Km/Hr
    speed = speed * 3600.0 / 1000.0;

    // Display direction and speed on Serial Monitor if speed is within the range (1 to 250 Km/Hr)
    if (speed >= 1 && speed <= 250) {
      Serial.print("Speed: ");
      Serial.print(speed, 1); // Display one decimal place
      Serial.println(" Km/Hr");
      Serial.println("Direction: -->LR");

      // Send SMS
      sendSMS(speed, "Direction: -->LR");
    }

    // Reset values
    resetValues();
  }

  // Check if the train is moving from ir_s2 to ir_s1
  if (digitalRead(ir_s1) == HIGH && digitalRead(ir_s2) == LOW) {
    timer2 = millis();
    while (digitalRead(ir_s1) == HIGH) {
      // Wait for the train to pass
    }
    timer1 = millis();
    timeElapsed = (timer1 - timer2) / 1000.0; // Convert milliseconds to seconds

    // Calculate speed (v = d / t)
    speed = distance / timeElapsed;
    // Convert speed to Km/Hr
    speed = speed * 3600.0 / 1000.0;

    // Display direction and speed on Serial Monitor if speed is within the range (1 to 250 Km/Hr)
    if (speed >= 1 && speed <= 250) {
      Serial.print("Speed: ");
      Serial.print(speed, 1); // Display one decimal place
      Serial.println(" Km/Hr");
      Serial.println("Direction: <--RL");

      // Send SMS
      sendSMS(speed, "Direction: <--RL");
    }

    // Reset values
    resetValues();
  }
}

void sendSMS(float speed, String direction) {
  String message = "Speed: " + String(speed, 1) + " Km/Hr " + direction;
  sim800.print("AT+CMGS=\"+91xxxxxxxxxx\"\r"); // Replace with the recipient's phone number
  delay(1000);
  sim800.print(message);
  delay(100);
  sim800.write(26); // ASCII code for CTRL+Z to send the SMS
  delay(1000);
}

void sendCommand(String command) {
  sim800.println(command);
  delay(100);
  while (sim800.available()) {
    Serial.write(sim800.read());
  }
}

void resetValues() {
  timer1 = 0;
  timer2 = 0;
  timeElapsed = 0;
  speed = 0;
  delay(1000);
}
