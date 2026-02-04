// Soil Monitoring & Analysis Chilli System วิเคราะห์ความชื้นในดินการปลูกต้นพริก
// Created by SmartFarmDIY
// (1) Specified parameter values as based on chilli padi plant.
// (2) Water and fertilizer are added manually onto the spoon connected to the servo motor.

#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //LiquidCrystal_I2C lcd(0x27, 16, 2);

#define soilLowWet 50   // Lower Soil Moisture Level Threshold
#define soilHighWet 70  // Upper Soil Moisture Level Threshold
#define sensorPower 7
#define sensorPin A1
int ML = 0;             // ML: Moisture Level

float calibration_value = 21.34 + 5.7;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;

Servo myservo;


void setup() 
{
  lcd.begin(16,2);                      //Defining 16 columns and 2 rows of lcd display
  lcd.backlight();                      // Power ON the back light
  pinMode(13, OUTPUT);                  // Set digital pin 13 -> Red LED output
  pinMode(12, OUTPUT);                  // Set digital pin 12 -> Green LED output
  pinMode(sensorPower, OUTPUT);         // Set analog pin A1 -> soil moisture output
  myservo.attach(9);                    // Attach the servo to servo object

  // Initialization
	digitalWrite(sensorPower, LOW);       // Soil Moisture Sensor OFF
  digitalWrite(13, LOW);                // Red LED OFF
  digitalWrite(12, LOW);                // Green LED OFF
  myservo.write(90);                    // Initialize servo at 0 speed or 'rest position'

  //Delay 5 seconds to stablilize sensor reading
  delay(5000);
  
  Serial.begin(9600);
}


void loop() 
{
  // Soil Moisture Sensing
  int moisture = readSensor();
  ML = (100 - ((moisture / 1023.00) * 100));
	Serial.print("Analog Output: ");
	Serial.println(moisture);
  Serial.print("Moisture Level: ");
  Serial.println(ML);
  display();

  if (ML < soilLowWet) 
  {
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);  //Red LED
    display();               // Add Water
  } else if (ML >= soilHighWet) 
  {
    digitalWrite(12, LOW);
    digitalWrite(12, HIGH);  // Green LED
    display();               // Stop Add Water
  } else if (ML > soilLowWet && ML < soilHighWet)
  {
    digitalWrite(13, LOW);
    digitalWrite(12, HIGH);  // Green LED
    display();               // Stop Add Water
    
    delay(1000);
    pH_Fertilizer(); // Continue to next pH sensing & fertilizer addition when ML is sufficient
  }
}


void pH_Fertilizer()
{
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
  lcd.clear(); //Clean the screen
  lcd.setCursor(0, 0);
  lcd.print("pH Val:");
  lcd.setCursor(8, 0);
  lcd.print(ph_act);
  
  // Check pH Level and Control LEDs
  if (ph_act < 5.5) {
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("pH too low");
    delay(10000);
    
    // Add a bit of water
    myservo.write(70); // rotate the motor counterclockwise
    delay(500); // keep rotating for 0.5 seconds (500 milliseconds)
    myservo.write(90); // stop the motor
    delay(1000); // stay stopped
    myservo.write(110); // rotate the motor clockwise
    delay(500); // keep rotating for 0.5 seconds (500 milliseconds)
    myservo.write(90); // stop the motor
    delay(1000); //staystopped
  } else if (ph_act > 6.5) {
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("pH too high");
    delay(10000);
    
    // Add fertilizer
    myservo.write(70); // rotate the motor counterclockwise
    delay(500); // keep rotating for 0.5 seconds (500 milliseconds)
    myservo.write(90); // stop the motor
    delay(1000); // stay stopped
    myservo.write(110); // rotate the motor clockwise
    delay(500); // keep rotating for 0.5 seconds (500 milliseconds)
    myservo.write(90); // stop the motor
    delay(1000); //staystopped
  } else {
    digitalWrite(13, LOW);
    digitalWrite(12, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("pH OK");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Soil Ready!");
  }
  
  delay(1000);
}


void display() 
{
  lcd.clear();//Clean the screen
  lcd.setCursor(0, 0);
  lcd.print("ML=");
  lcd.print(ML);
  
  if (ML < soilLowWet) 
  {
    lcd.setCursor(0, 1);
    lcd.print("Need Water");
  } else if (ML >= soilHighWet) {
    lcd.setCursor(0, 1);
    lcd.print("Stop Need Water");
  } else if (ML >= soilLowWet && ML <= soilHighWet) {
    lcd.setCursor(0, 1);
    lcd.print("Stop Need Water");
  }
  
  delay(1000);
  Serial.println();
}


//  This function returns the analog soil moisture measurement
int readSensor() 
{
	digitalWrite(sensorPower, HIGH);	// Turn the sensor ON
	delay(10);							          // Allow power to settle
	int val = analogRead(sensorPin);	// Read the analog value form sensor
	digitalWrite(sensorPower, LOW);		// Turn the sensor OFF
	return val;							          // Return analog moisture value
}