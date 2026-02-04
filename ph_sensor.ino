#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

float calibration_value = 21.34 + 5.7;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  lcd.begin(16, 2);
  lcd.backlight();
  pinMode(D13, OUTPUT);                  // Set digital pin 13 -> Red LED output
  pinMode(D12, OUTPUT);                  // Set digital pin 12 -> Green LED output
  delay(2000);
}

void loop() {
  Blynk.run();
  // pH Measurement
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(A0);
    delay(30);
  }
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++) {
    avgval += buffer_arr[i];
  }
  float volt = (float)avgval * 5.0 / 1024 / 6;
  float ph_act = -5.70 * volt + calibration_value;
  lcd.clear();//Clean the screen
  lcd.setCursor(0, 0);
  lcd.print("pH Val:");
  lcd.setCursor(8, 0);
  lcd.print(ph_act);
  
  // Check pH Level and Control LEDs
  if (ph_act < 5.5) {
    digitalWrite(D12, LOW);
    digitalWrite(D13, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("pH too low");
  } else if (ph_act > 6.5) {
    digitalWrite(D12, LOW);
    digitalWrite(D13, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("pH too high");
  } else {
    digitalWrite(D13, LOW);
    digitalWrite(D12, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("pH is OK");
  }
  
  delay(1000);
}