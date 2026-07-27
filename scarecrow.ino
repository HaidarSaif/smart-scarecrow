#include <WiFi.h>
#include <Wire.h>
#include <time.h>
#include <Ticker.h>
#include <Arduino.h>
#include <OneWire.h>
#include <ESP32Servo.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>

#define FREQUENCY1 20000
#define FREQUENCY2 40000

LiquidCrystal_I2C lcd(0x27, 20, 4);

Servo myservo1;
Servo myservo2;

Ticker timeTicker1;
Ticker timeTicker2;

const char* ssid = "scarecrow";
const char* password = "12345678";
const char* host = "192.168.105.133"; // Ganti dengan IP address Raspberry Pi
const uint16_t port = 8083;

unsigned long sendDataPrevMillis = 0;
bool manualPH, manualWave, manualMoist;
float soilPH, moist, temp, voltage1, voltage2, current1, current2;
int water = 1, count = 0;
int night, age, pest, M1, M2, M3, M4, dolomit, ultrasonic, moisture;
int currentFrequency = FREQUENCY1;
String newData = "";
const int moistPin = 27;
const int waterPin = 26;
const int oneWireBusPin = 25;
const int voltPin1 = 35;
const int currentPin1 = 34;
const int pumpPin = 33;
const int mixerPin = 32;
const int valvePin = 2;
const int wavePin = 4;
const int servoPin1 = 13;
const int servoPin2 = 23;
const int tonePin = 14;
const int RE = 18;
const int DE = 17;
const int RX1 = 19;
const int TX1 = 16;
const int voltPin2 = 39;
const int currentPin2 = 36;

const byte ph[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};
byte values[11];

const int numReadings = 10; // Jumlah nilai yang akan disimpan
float readings[numReadings]; // Array untuk menyimpan nilai
int readIndex = 0;           // Indeks saat ini dalam array
float total = 0;             // Jumlah semua nilai
float average = 0;

float readings2[numReadings]; // Array untuk menyimpan nilai
int readIndex2 = 0;           // Indeks saat ini dalam array
float total2 = 0;             // Jumlah semua nilai
float average2 = 0;

byte symbolTemp[8] = {B00100, B01010, B01010, B01110, B11111, B11111, B01110, B00000};
byte symbolMoist[8] = {B00100, B01010, B01010, B10001, B10001, B10001, B01110, B00000};
byte symbolPh[8] = {B01001, B01001, B01111, B01001, B11101, B10100, B11100, B10000};

OneWire oneWire(oneWireBusPin);
DallasTemperature sensors(&oneWire);

SoftwareSerial mod(RX1, TX1);

time_t startTime;

int mapValue(int input) {
  int minInput = 100;
  int maxInput = 3008;

  int minOutput = 0;
  int maxOutput = 100;

  int output = map(input, minInput, maxInput, maxOutput, minOutput);
  return output;
}

void setup()
{
  Serial.begin(115200);
  
  myservo1.attach(servoPin1);
  myservo2.attach(servoPin2);
  
  myservo1.write(90);
  delay(300);
  myservo2.write(0);
 delay(5000);
  myservo1.write(0);
  delay(300);
  myservo2.write(90);
  
  lcd.begin(20, 4);
  lcd.init();
  lcd.backlight();

  mod.begin(4800);

  lcd.setCursor(0, 0);
  lcd.print("  SMART  SCARECROW  ");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    int ii;
    delay(1000);
    if (ii == 4) {
      for (int i = 0; i < 5; i++) {
        lcd.setCursor(15 + i, 1);
        lcd.print(" ");
      }
      ii = 0;
    }
    Serial.println("Connecting to WiFi...");
    lcd.setCursor(0, 1);
    lcd.print("  Menghubungkan");
    lcd.setCursor(15 + ii, 1);
    lcd.print(".");
    ii++;
  }
  Serial.println("Connected to WiFi");
  lcd.setCursor(0, 1);
  lcd.print("  Terhubung         ");

  sensors.begin();

  Wire.begin();

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  pinMode(waterPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(mixerPin, OUTPUT);
  pinMode(valvePin, OUTPUT);
  pinMode(wavePin, OUTPUT);
  digitalWrite(pumpPin, HIGH);
  digitalWrite(mixerPin, HIGH);
  digitalWrite(valvePin, HIGH);
  digitalWrite(wavePin, HIGH);

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
    readings2[thisReading] = 0;
  }
  ledcSetup(0, FREQUENCY1, 8);
  ledcAttachPin(tonePin, 0);
  ledcWriteTone(0, currentFrequency);
}

void loop()
{
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("Connection to server failed");
    delay(1000);
    return;
  }

  myservo2.write(0);
  delay(300);
  myservo1.write(90);

  water = digitalRead(waterPin);

  float moistSensor = analogRead(moistPin);
  moist = mapValue(moistSensor);
  if (moist < 0)moist = 0;
  if (moist > 100)moist = 100;

  sensors.requestTemperatures();

  temp = sensors.getTempCByIndex(0);
  if (temp < 0)temp = 0;

  byte val;
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if (mod.write(ph, sizeof(ph)) == 8)
  {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for (byte i = 0; i < 11; i++)
    {
      values[i] = mod.read();
      Serial.print(values[i], HEX);
    }
    Serial.println();
  }
  soilPH = float(values[4]) / 10;
  if (soilPH > 9)soilPH = 9;

  int voltValue1 = analogRead(voltPin1); // Membaca nilai analog dari pin sensor
  voltage1 = (voltValue1 / 4095.0) * 3.3 / (7500.0 / (30000.0 + 7500.0)) + 0.6;
  if (voltage1 <= 0.61)voltage1 = 0;

  int voltValue2 = analogRead(voltPin2); // Membaca nilai analog dari pin sensor
  voltage2 = (voltValue2 / 4095.0) * 3.3 / (7500.0 / (30000.0 + 7500.0)) + 0.6;
  if (voltage2 <= 0.61)voltage2 = 0;

  total = total - readings[readIndex];
  int adc1 = analogRead(currentPin1);
  float volt1 = (adc1 * 3.336) / 4095;
  current1 = (volt1 - 1.48) / 0.185;
  readings[readIndex] = current1;
  total = total + current1;
  readIndex = readIndex + 1;
  if (readIndex >= numReadings)readIndex = 0;
  average = total / numReadings;
  if (average < 0.5)average = 0;
  current1 = average / 1000;

  total2 = total2 - readings2[readIndex2];
  int adc2 = analogRead(currentPin2);
  float volt2 = (adc2 * 3.336) / 4095;
  current2 = (volt1 - 1.48) / 0.185;
  readings2[readIndex2] = current2;
  total2 = total2 + current2;
  readIndex2 = readIndex2 + 1;
  if (readIndex2 >= numReadings)readIndex2 = 0;
  average2 = total2 / numReadings;
  if (average2 < 0.5)average2 = 0;
  current2 = average2 / 1000;

  Serial.print("Moist: ");
  Serial.print(moist);
  Serial.print(" %| Temp: ");
  Serial.print(temp);
  Serial.print(" C| pH: ");
  Serial.print(soilPH);
  Serial.print(" | Volt 1: ");
  Serial.print(voltage1);
  Serial.print(" V| Volt 2: ");
  Serial.print(voltage2);
  Serial.print(" V| Current 1: ");
  Serial.print(current1);
  Serial.print(" mA| Current 2: ");
  Serial.print(current2);
  Serial.print(" mA| Water: ");
  Serial.println(water);

  lcd.createChar(1, symbolTemp);
  lcd.createChar(2, symbolMoist);
  lcd.createChar(3, symbolPh);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  SMART  SCARECROW  ");
  lcd.setCursor(0, 1);
  lcd.write(1);
  lcd.print(" ");
  lcd.print(temp, 1);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 2);
  lcd.write(2);
  lcd.print(" ");
  lcd.print(moist, 1);
  lcd.print(" %");
  lcd.setCursor(0, 3);
  lcd.write(3);
  lcd.print(" ");
  lcd.print(soilPH, 1);
  lcd.setCursor(10, 1);
  lcd.print("Umur  : ");
  lcd.print(age);
  lcd.setCursor(10, 2);
  lcd.print("Hama  : ");
  lcd.print(pest);
  lcd.setCursor(10, 3);
  lcd.print("Tangki: ");
  lcd.print(water);

  String message = String(moist) + "&" + String(temp) + "&" + String(soilPH) + "&" + String(voltage1) + "&" + String(voltage2) + "&" + String(current1) + "&" + String(current2) + "&" + String(water);
  client.print(message);
  Serial.println("Message sent: " + message);

  while (client.available() == 0) {
    delay(100);
  }

  String response = client.readString();
  Serial.println("Response: " + response);
  if (response != newData)
    splitString(response);
  newData = response;

  // Tampilkan hasilnya
  Serial.print("M1: ");
  Serial.println(M1);
  Serial.print("M2: ");
  Serial.println(M2);
  Serial.print("M3: ");
  Serial.println(M3);
  Serial.print("M4: ");
  Serial.println(M4);
  Serial.print("age: ");
  Serial.println(age);
  Serial.print("pest: ");
  Serial.println(pest);

  if (voltage2 <= 0.5)
    night = 1;
  else
    night = 0;
  if (night)
    currentFrequency = FREQUENCY2;
  else
    currentFrequency = FREQUENCY1;

  if (M2 == 1) {
      controlPH();
  }
  else if (M3 == 1) {
    ultrasonic = 1;
  }
  else if (M4 == 1) {
    moisture = 1;
  }
  if (dolomit)controlPH();
  if (moisture)controlMoisture();
  ultrasonic = 0;
  moisture = 0;
  M1 = 0;
  M2 = 0;
  M3 = 0;
  M4 = 0;
  client.stop();
  delay(1000);
}

void controlPH() {
  digitalWrite(valvePin, HIGH);
  if (water == 1) {
    digitalWrite(pumpPin, LOW);
  }
  else {
    digitalWrite(pumpPin, HIGH);
    myservo2.write(0);
    delay(300);
    myservo1.write(90);
    digitalWrite(mixerPin, LOW);
    timeTicker1.attach(30, deactiveMixer);
  }
}

void controlMoisture() {
  if (!dolomit) {
    digitalWrite(valvePin, LOW);
  }
  timeTicker1.attach(10, deactiveValve);
}

void deactiveMixer() {
  digitalWrite(valvePin, LOW);
  digitalWrite(mixerPin, HIGH);
  myservo1.write(0);
  delay(300);
  myservo2.write(90);
  timeTicker1.detach();
  timeTicker1.attach(10, deactiveValve);
  dolomit = 0;
  Serial.println("Dolomit selesai diproses");
}

void deactiveValve() {
  digitalWrite(valvePin, HIGH);
  timeTicker1.detach();
}

void splitString(String str) {
  str = str.substring(1, str.length() - 1);
  char buf[str.length() + 1];
  str.toCharArray(buf, str.length() + 1);
  char *token = strtok(buf, "&");

  if (token != NULL) {
    M1 = atoi(token);
    token = strtok(NULL, "&");
  }

  if (token != NULL) {
    M2 = atoi(token);
    token = strtok(NULL, "&");
  }

  if (token != NULL) {
    M3 = atoi(token);
    token = strtok(NULL, "&");
  }

  if (token != NULL) {
    M4 = atoi(token);
    token = strtok(NULL, "&");
  }

  if (token != NULL) {
    age = atoi(token);
    token = strtok(NULL, "&");
  }

  if (token != NULL) {
    pest = atoi(token);
  }
}
