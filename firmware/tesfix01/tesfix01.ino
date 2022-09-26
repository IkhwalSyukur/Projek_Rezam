
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define pin 32
//pins:
const int HX711_dout = 16; //mcu > HX711 dout pin
const int HX711_sck = 4; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

#define WIFI_SSID "Variasi Aluminium"
#define WIFI_PASSWORD "hapisahsyukur1"
#define API_KEY "AIzaSyDFtTXeTBSzVSgD0DMub2rRER9pHrLeEPE"
#define DATABASE_URL "https://bbideal-14f54-default-rtdb.firebaseio.com/"
#define USER_EMAIL "ikhwals16@gmail.com"
#define USER_PASSWORD "ayamku21"

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int berat = 0;
int tinggi = 0;
float ideal=0;
String gender = "";
String kondisi = "";
int Status=0;
int updateData=0;
int bbIdeal=0;
int isIdeal=0;


void setup() {
  Serial.begin(57600); delay(10);
  Serial.println();
  Serial.println("Starting...");

 //setingan awal wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
  Firebase.begin(&config, &auth);


  float calibrationValue; // calibration value
//  calibrationValue = 24.73; //barbel uncomment this if you want to set this value in the sketch
//    calibrationValue = 21.94; //lebih 2kilo
//    calibrationValue = 21.75; //70kilo
//    calibrationValue = 23.29;//66 kilo
    calibrationValue = 22.65;
#if defined(ESP8266) || defined(ESP32)
//  EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch this value from eeprom
#endif
//  EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch this value from eeprom

  LoadCell.begin();
  //LoadCell.setReverseOutput();
  unsigned long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration factor (float)
    Serial.println("Startup is complete");
  }
  while (!LoadCell.update());
  Serial.print("Calibration value: ");
  Serial.println(LoadCell.getCalFactor());
  Serial.print("HX711 measured conversion time ms: ");
  Serial.println(LoadCell.getConversionTime());
  Serial.print("HX711 measured sampling rate HZ: ");
  Serial.println(LoadCell.getSPS());
  Serial.print("HX711 measured settlingtime ms: ");
  Serial.println(LoadCell.getSettlingTime());
  Serial.println("Note that the settling time may increase significantly if you use delay() in your sketch!");
  if (LoadCell.getSPS() < 7) {
    Serial.println("!!Sampling rate is lower than specification, check MCU>HX711 wiring and pin designations");
  }
  else if (LoadCell.getSPS() > 100) {
    Serial.println("!!Sampling rate is higher than specification, check MCU>HX711 wiring and pin designations");
  }
  lcd.begin();
  // Turn on the blacklight and print a message.
  lcd.backlight();
  
  lcd.setCursor(0,0);
  lcd.print("System Ready");
  delay(3000);
  lcd.clear();
}

void loop() {
  kirim();
}
