#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>  // For I2C communication
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include "EmonLib.h"  // Include Emon Library
#include <PZEM004Tv30.h>

#include <LiquidCrystal_I2C.h>
#include <Bounce2.h>

#define up_button 9
#define down_button 10
#define ok_button 11

Bounce upButton = Bounce();
Bounce downButton = Bounce();
Bounce okButton = Bounce();

LiquidCrystal_I2C lcd(0x27, 20, 4); // Initialize the LCD object with the I2C address and dimensions
#include <SoftwareSerial.h>

#define GSM_TX_PIN 3  // Connect GSM module TX to Arduino RX pin
#define GSM_RX_PIN 2// Connect GSM module RX to Arduino TX pin
SoftwareSerial sim800Serial(GSM_RX_PIN, GSM_TX_PIN); // RX, TX

int errorFlag = 0;



unsigned long lastCommandTime = 0;
unsigned long commandDelay = 1500;
int orderNum = 0;

bool veryBadPrinted = false;
bool badPrinted = false;
bool goodPrinted = false;
bool veryGoodPrinted = false;
bool excellentPrinted = false;
String PHONE_NUMBER = "+263776432893";
// Constants for RMS calculation
#define NUM_SAMPLES 100     // Number of samples for RMS calculation
#define RMS_THRESHOLD 0.8   // Threshold for significant vibration change in g(s)

// Arrays to store accelerometer readings
float accelX[NUM_SAMPLES];
float accelY[NUM_SAMPLES];
float accelZ[NUM_SAMPLES];

// Baseline RMS values
float baselineRMS_X = 0.0;
float baselineRMS_Y = 0.0;
float baselineRMS_Z = 0.0;
  float rmsX = 0;
  float rmsY = 0;
  float rmsZ = 0;


// Create instances for each PZEM module
PZEM004Tv30 pzem1(&Serial1);  // Using Hardware Serial 1 for PZEM1
PZEM004Tv30 pzem2(&Serial2);  // Using Hardware Serial 2 for PZEM2
PZEM004Tv30 pzem3(&Serial3);  // Using Hardware Serial 3 for PZEM3

// Data wire connected to digital pin 2
#define ONE_WIRE_BUS 22

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
EnergyMonitor emon1;  // Create an instance
EnergyMonitor emon2;  // Create an instance


// ADXL345 setup
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);  // Replace with your actual sensor address

// Motor Protection
// Define pins for relay control
const int relayPin = 28;  // Change to the appropriate pin

float I50set = 2.0;  // Set your overcurrent threshold (in amps)
float I51set = 2.0;
float Vrated = 230;
float Ir = 1.6;    // define the rated current
float Vset = 250.0;  // Set your overvoltage threshold (in volts)
float FrequencySet = 50;
float TemperatureSet = 50.0;  // Set maximum allowable temperature (in Celsius)
float AccelSet = 20;            // Set vibration threshold (adjust based on your sensor)

double t = 10 * log(1.0 / (1.0 - pow(Ir / I51set, 2)));  //set delay time in secconds for overload i.e it should trip after time e
unsigned int overloadThreshold = static_cast<unsigned int>(t * 1000);

float voltageR = 0;  // Voltage in Red Phase
float voltageY = 0;
float voltageB = 0;
float currentR = 0;
float currentY = 0;
float currentB = 0;
float frequencyR = 0;
float frequencyY = 0;
float frequencyB = 0;
float temperatureC = 0;
float accelXx = 0;
float accelYy = 0;
float accelZz = 0;
int count = 0;
int count2 = 0;
char commandx;
 float overallCondition = 0;

unsigned long overloadStartTimeR = 0;
unsigned long overloadStartTimeY = 0;
unsigned long overloadStartTimeB = 0;

unsigned long overloadTimeR = 0;
unsigned long overloadTimeY = 0;
unsigned long overloadTimeB = 0;

unsigned long previousMillis = 0;

float accelerationReading = 0;
float PhaseFailureIndicatorVolatge = 0;
float BreakerStatusIndicatorVolatge = 0;
unsigned long prevMillis = 0; // Variable to store the last time SMS was sent
const int SMSInterval = 10000; // Interval between SMS sending

int selectedMenuItem = 0; // Index of the selected menu item

// Define menu items
const int NUM_MENU_ITEMS = 8;
const String menuItems[NUM_MENU_ITEMS] = {
  "I50thr",
  "I51thr",
  "Vr",
  "Ir",
  "V",
  "Freq",
  "Temp",
  "Accel"
};

// Define corresponding parameter values
float parameters[NUM_MENU_ITEMS] = {
  2.0,  // I50 Threshold
  2.0,  // I51 Threshold
  230,  // V Rated
  1.6,  // Ir
  250.0,  // V Threshold
  50,   // Frequency
  10.0, // Temperature
  1.00 // acceleration
};


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);  // Initialise serial 1
  Serial2.begin(9600);  // Initialise serial 2
  Serial3.begin(9600);  // Initialise serial 3
  sim800Serial.begin(9600);  // Initialize serial communication with the GSM module

  sensors.begin();  // Initialise ZMPTS, i.e to be mdified to initialise 2 sensors
  accel.begin();    // Initialize ADXL345

  //voltage indicators for breaker and phase sequence failure
  emon1.voltage(A0, 234.26, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon2.voltage(A1, 234.26, 1.7);  // Voltage: input pin, calibration, phase_shift

  // Set ADXL345 range (options: RANGE_2_G, RANGE_4_G, RANGE_8_G, RANGE_16_G)
  accel.setRange(ADXL345_RANGE_16_G);
    calculateBaselineRMS();
    lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight

  pinMode(up_button, INPUT_PULLUP);
  pinMode(down_button, INPUT_PULLUP);
  pinMode(ok_button, INPUT_PULLUP);

  upButton.attach(up_button);
  downButton.attach(down_button);
  okButton.attach(ok_button);

  lcd.setCursor(0, 0);
  lcd.print("Welcome");
  delay(3000);
  lcd.clear();
   lcd.setCursor(0, 0);
  lcd.print("Press OK to access"); 
    lcd.setCursor(0, 1);
  lcd.print("SETTINGS");
 


  sendSMSx(PHONE_NUMBER, "SYSTEM ON");

  pinMode(relayPin, OUTPUT);    // Contactor / breaker control relay
  digitalWrite(relayPin, LOW);  //initialise relay pin status
}

void loop() {
  if (errorFlag == 0) displayParameters();
  okButton.update();
 // Serial.println("OK Button" + String(okButton.fell()));
 if (okButton.fell())
  {
  displayMenu();
  do{
    menuControl();
   }
  while(selectedMenuItem < NUM_MENU_ITEMS);
  
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("All parameters saved");
    delay(2000);
    selectedMenuItem = 0;
      I50set = parameters[0];
      I51set = parameters[1];
      Vrated = parameters[2];
      Ir = parameters[3];
      Vset = parameters[4];
      FrequencySet = parameters[5];
      TemperatureSet = parameters[6];
      AccelSet = parameters[7];
  }
  if (Serial.available()) {
  commandx = Serial.read();
  switch (commandx) {
    case '1':
      // Turn the relay ON
      count = 1;
      digitalWrite(relayPin, HIGH);
      Serial.println("Relay turned ON.");
      break;
    case '0':
      count = 0;
      // Turn the relay OFF
      digitalWrite(relayPin, LOW);
      Serial.println("Relay turned OFF.");
      break;
    default:

      if (count == 0) Serial.println("Invalid command. Use '1' to turn ON or '0' to turn OFF.");

      break;
  }
  }

  // Read data from PZEM1

    voltageR = pzem1.voltage();
    currentR = pzem1.current();
    frequencyR = pzem1.frequency();




    voltageY = pzem2.voltage();
    currentY = pzem2.current();
    frequencyY = pzem2.frequency();

// Read data from PZEM3

    voltageB = pzem3.voltage();
    currentB = pzem3.current();
    frequencyB = pzem3.frequency();


  // Read temperature
  sensors.requestTemperatures();
  temperatureC = sensors.getTempCByIndex(0);
  

  // Read accelerometer data
  // Read accelerometer data and store in arrays
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sensors_event_t event;
    accel.getEvent(&event);
    accelX[i] = event.acceleration.x;
    accelY[i] = event.acceleration.y;
    accelZ[i] = event.acceleration.z;
    delay(10); // Adjust delay based on sampling frequency
  }
    sensors_event_t events;
    accel.getEvent(&events);
  accelXx = events.acceleration.x;
  accelYy = events.acceleration.y;
  accelZz = events.acceleration.z;

  // Calculate RMS values for each axis
  rmsX = calculateRMS(accelX, NUM_SAMPLES);
  rmsY = calculateRMS(accelY, NUM_SAMPLES);
  rmsZ = calculateRMS(accelZ, NUM_SAMPLES);

  // Print RMS values for debugging
  Serial.print("RMS X: ");
  Serial.print(rmsX);
  Serial.print(" RMS Y: ");
  Serial.print(rmsY);
  Serial.print(" RMS Z: ");
  Serial.println(rmsZ);

  //Read voltage from volatge indicator sensors
  emon1.calcVI(20, 2000);  // Calculate all. No.of half wavelengths (crossings), time-out
  emon2.calcVI(20, 2000);  // Calculate all. No.of half wavelengths (crossings), time-out

  PhaseFailureIndicatorVolatge = emon1.Vrms;   // Extract Vrms into Variable
  BreakerStatusIndicatorVolatge = emon2.Vrms;  // Extract Vrms into Variable



  // NEED TO MODIFY CODE TO READ BOTH A0 AND A1

  // Display Data
  if (count > 0) {
    Serial.println("PZEMR - Vr: " + String(voltageR) + " V, Ir: " + String(currentR) + " A,FrequencyR:" + String(frequencyR) + "Hz");
    Serial.println("PZEMY - Vy: " + String(voltageY) + " V, Iy: " + String(currentY) + " A,FrequencyY:" + String(frequencyY) + "Hz");
    Serial.println("PZEMB - Vb: " + String(voltageB) + " V, Ib: " + String(currentB) + " A,FrequencyB:" + String(frequencyB) + "Hz");

    Serial.print("Temperature: ");
    Serial.print(temperatureC, 2);  // Display Celsius temperature with 2 decimal places
    Serial.print("°C / ");
   

    Serial.print("Accelerometer (X, Y, Z): ");
    Serial.print(accelXx, 2);
    Serial.print(", ");
    Serial.print(accelYy, 2);
    Serial.print(", ");
    Serial.println(accelZz, 2);


    Serial.print("PhaseFailureIndicatorVoltage: ");
    Serial.println(PhaseFailureIndicatorVolatge);  // Print Voltage

    Serial.print("BreakerStatusIndicatorVolatge: ");
    Serial.println(BreakerStatusIndicatorVolatge);  // Print Voltage
    //Contactor /Breaker control relay
    // Check for commands from the serial monitor
    motorProtection();
      float temperatureReading = temperatureC;
      float currentReading = currentB; // I dont know what to put here, i assume Irms
      float voltageReading = voltageB; // I dont know what to put here, i assume Vrms
      accelerationReading = calculateAccelerationChange(accelXx, accelYy, accelZz,baselineRMS_X, baselineRMS_Y, baselineRMS_Z);

    //float overallCondition = calculateOverallCondition(accelerationReading, temperatureReading, currentReading, voltageReading);

    //Serial.println("Condition Score : " + String(overallCondition,2));
    Serial.println("Acceleration: " + String(accelerationReading,2));
  // printConditionAndSendSMS(overallCondition);
   //sendSMSEveryXMilliseconds(SMSInterval);

  // Call the motor protection function
  if (count > 0) {

    if (count2 == 0) {
      Serial.println("reading Values");
      delay(1);
    }
    count2++;


  }




  //delay(5000); // Wait for a second
}
}




void turnOffMotor() {
  digitalWrite(relayPin, LOW);  // Turn off the relay
}
void turnOnMotor() {
  digitalWrite(relayPin, HIGH);  // Turn off the relay
}

float calculateAccelerationModulus(float accX, float accY, float accZ) {
  return sqrt(pow(accX, 2) + pow(accY, 2) + pow(accZ, 2));
}
