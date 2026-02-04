#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //LiquidCrystal_I2C lcd(0x27, 16, 2);

#define soilLowWet 50   // Lower Soil Moisture Level Threshold
#define soilHighWet 70  // Upper Soil Moisture Level Threshold
#define sensorPower 7
#define sensorPin A1
int ML = 0;             // ML: Moisture Level


void setup() 
{
  lcd.begin(16,2);//Defining 16 columns and 2 rows of lcd display //lcd.init();                           // Initialize the display 
  lcd.backlight();                      // Power ON the back light
  pinMode(13, OUTPUT);                  // Set digital pin 13 -> Red LED output
  pinMode(12, OUTPUT);                  // Set digital pin 12 -> Green LED output
  pinMode(sensorPower, OUTPUT); // Set analog pin A1 -> soil moisture output
  
  // Initialization
	digitalWrite(sensorPower, LOW); // Soil Moisture Sensor OFF
  digitalWrite(13, LOW);          // Red LED OFF
  digitalWrite(12, LOW);          // Green LED OFF
  
  // //DELAY 1 minute to stablilize sensor reading
  // delay(60000);
  
  Serial.begin(9600);
}

void loop() 
{
  //get the reading from the function below and print it
	int moisture = readSensor();
  ML = (100 - ((moisture / 1023.00) * 100));
	Serial.print("Analog Output: ");
	Serial.println(moisture);
  Serial.print("Moisture Level: ");
  Serial.println(ML);
  
  display();

  // Process Variable
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
  }
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
    lcd.print("Add Water");
  } else if (ML >= soilHighWet) {
    lcd.setCursor(0, 1);
    lcd.print("Stop Add Water");
  } else if (ML >= soilLowWet && ML <= soilHighWet) {
    lcd.setCursor(0, 1);
    lcd.print("Stop Add Water");
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