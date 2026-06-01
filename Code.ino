#define BLYNK_TEMPLATE_ID   "TMPL3VSlOIHEk"
#define BLYNK_TEMPLATE_NAME "Iotsmart"
#define BLYNK_AUTH_TOKEN    "0n1KlKEt8iKZkZ3R7Cx-PpkvaRhaWJuJ"


#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal.h>
#include "DHT.h"

// WiFi credentials
char ssid[] = "vivo 1723";       
char pass[] = "2244668899";   

// DHT22
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Ultrasonic Sensors
#define TRIG1 5
#define ECHO1 18
#define TRIG2 19
#define ECHO2 25

// MQ-2 Gas Sensor
#define MQ2_PIN 34   

// Buzzer + LED
#define BUZZER 23
#define LED 2

// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(13, 12, 14, 27, 26, 33);

// Thresholds
float TEMP_THRESHOLD = 35.0;   
int GAS_THRESHOLD = 200;       

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  dht.begin();
  lcd.begin(16, 2);
  lcd.print(" Smart Parking ");
  delay(2000);

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);

  lcd.clear();
}

long getDistance(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  return pulseIn(echo, HIGH) * 0.034 / 2;
}

void loop() {
  Blynk.run();

  // Temperature & Humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Blynk.virtualWrite(V1, t);  
  Blynk.virtualWrite(V2, h);  

  // Gas Sensor
  int gasValue = analogRead(MQ2_PIN);
  Blynk.virtualWrite(V3, gasValue);

  // Parking Slots
  long dist1 = getDistance(TRIG1, ECHO1);
  String slot1 = (dist1 < 10) ? "Occu" : "Avail";
  Blynk.virtualWrite(V4, slot1);

  long dist2 = getDistance(TRIG2, ECHO2);
  String slot2 = (dist2 < 10) ? "Occu" : "Avail";
  Blynk.virtualWrite(V5, slot2);

  // Alerts
  bool alertActive = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("S1:");
  lcd.print(slot1);
  lcd.print(" S2:");
  lcd.print(slot2);

  if (t > TEMP_THRESHOLD) {
    Blynk.logEvent("fire_alert", "High Temperature!");
    lcd.setCursor(0, 1);
    lcd.print("FIRE ALERT:");
    lcd.print(t, 1);
    lcd.print("C");
    alertActive = true;
  } 
  else if (gasValue > GAS_THRESHOLD) {
    Blynk.logEvent("gas_alert", "Gas Leakage Detected!");
    lcd.setCursor(0, 1);
    lcd.print("GAS ALERT:");
    lcd.print(gasValue);
    alertActive = true;
  } 
  else {
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(t, 1);
    lcd.print("C G:");
    lcd.print(gasValue);
  }

  if (alertActive) {
    digitalWrite(BUZZER, HIGH);
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
    digitalWrite(LED, LOW);
  }

  delay(2000);
}