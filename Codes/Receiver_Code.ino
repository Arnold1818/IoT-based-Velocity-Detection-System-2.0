#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Define the pins for the GSM module (SIM800L) and LCD
#define SIM800_TX 10
#define SIM800_RX 11
#define LCD_ADDRESS 0x27 // I2C address of the LCD module

// Initialize SoftwareSerial for SIM800L communication
SoftwareSerial sim800(SIM800_TX, SIM800_RX);

// Initialize LCD
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2); // Adjust the dimensions (16x2) based on your LCD

void setup() {
  Serial.begin(9600);
  sim800.begin(9600); // Set the baud rate for the SIM800L module
  
  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  
  Serial.println("Receiver System Initialized");
  
  // Initialize SIM800L
  sendCommand("AT");
  delay(1000);
  sendCommand("AT+CMGF=1"); // Set SMS to text mode
  delay(1000);
}

void loop() {
  // Check for incoming SMS
  if (sim800.available()) {
    String sms = sim800.readString();
    Serial.print("Received SMS: ");
    Serial.println(sms);
    
    // Parse speed and direction from SMS
    float speed = extractSpeed(sms);
    String direction = extractDirection(sms);
    
    // Display on Serial Monitor
    Serial.print("Speed: ");
    Serial.print(speed, 1);
    Serial.println(" Km/Hr");
    Serial.print("Direction: ");
    Serial.println(direction);
    
    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Speed: ");
    lcd.print(speed, 1);
    lcd.print(" Km/Hr");
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print(direction);
    
    delay(5000); // Display for 5 seconds
  }
}

// Function to extract speed from SMS message
float extractSpeed(String sms) {
  int index = sms.indexOf("Speed:");
  if (index != -1) {
    String speedStr = sms.substring(index + 6); // Skip "Speed: "
    return speedStr.toFloat();
  }
  return 0.0; // Default value if extraction fails
}

// Function to extract direction from SMS message
String extractDirection(String sms) {
  int index = sms.indexOf("Direction:");
  if (index != -1) {
    return sms.substring(index + 11); // Skip "Direction: "
  }
  return ""; // Default value if extraction fails
}

void sendCommand(String command) {
  sim800.println(command);
  delay(100);
  while (sim800.available()) {
    Serial.write(sim800.read());
  }
}
