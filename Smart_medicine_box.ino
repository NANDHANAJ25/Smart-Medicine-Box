

#define BLYNK_TEMPLATE_ID "YOUR TEMPLATE ID"
#define BLYNK_TEMPLATE_NAME "YOUR TEMPLATE NAME"
#define BLYNK_AUTH_TOKEN "YOUR AUTH TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define PIR_PIN      13
#define SERVO_PIN    18
#define BUZZER_PIN   27
#define BUTTON_PIN   14

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servoMotor;

bool reminderActive = false;
bool boxOpened = false;

unsigned long previousMillis = 0;
const unsigned long interval = 20000;   
void setup() {

  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  servoMotor.attach(SERVO_PIN);
  servoMotor.write(0);

  lcd.setCursor(0, 0); 
 lcd.print("Medicine Box");
 lcd.setCursor(0, 1);
  lcd.print("System Ready");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting...");
  lcd.setCursor(0, 1);
  lcd.print("Next Dose");

  Blynk.virtualWrite(V0, "Waiting...");
  Blynk.virtualWrite(V1, "No Person");
  Blynk.virtualWrite(V2, 0);
  Blynk.virtualWrite(V3, 0);

  previousMillis = millis();
}

void loop() {

  Blynk.run();

  // Trigger reminder every 30 seconds
  if (!reminderActive && (millis() - previousMillis >= interval)) {

    reminderActive = true;
    boxOpened = false;

    tone(BUZZER_PIN, 1000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Medicine Time!");
    

    Blynk.virtualWrite(V0, "Medicine Time!");
    Blynk.virtualWrite(V3, 255);   // Reminder LED ON
    Blynk.virtualWrite(V2, 0);     // Ack LED OFF
  }

  // PIR detects person
  if (reminderActive &&
      !boxOpened &&
      digitalRead(PIR_PIN) == HIGH) {

    servoMotor.write(90);
    boxOpened = true;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Box Opened");
    lcd.setCursor(0, 1);
    lcd.print("Take Medicine");

    Blynk.virtualWrite(V1, "Person Detected");
  }

  // Push button pressed
  if (reminderActive &&
      digitalRead(BUTTON_PIN) == LOW) {

    noTone(BUZZER_PIN);

    servoMotor.write(0);

    reminderActive = false;
    boxOpened = false;
    previousMillis = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Medicine");
    lcd.setCursor(0, 1);
    lcd.print("Taken");

    Blynk.virtualWrite(V0, "Medicine Taken");
    Blynk.virtualWrite(V1, "No Person");
    Blynk.virtualWrite(V2, 255);   // Ack LED ON
    Blynk.virtualWrite(V3, 0);     // Reminder LED OFF

    delay(1000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Waiting...");
    lcd.setCursor(0, 1);
    lcd.print("Next Dose");

    Blynk.virtualWrite(V0, "Waiting...");
  }
}
