#include <SoftwareSerial.h>
#include <LCD-I2C.h>
#include <Wire.h>
LCD_I2C lcd(0x27,16,2);

#define RX 3
#define TX 2

SoftwareSerial esp01(RX, TX);
String WIFI_SSID = "Redmi 12 5G"; //Have to Change
String WIFI_PASS = "123456789"; //Have to Change 
String API = "0VSFO4ZN66KCG2ME"; // Have to Change
String HOST = "api.thingspeak.com";
String PORT = "80";

int countTrueCommand;
int countTimeCommand;
boolean found = false;
#define SOIL_PIN A0
int dryThreshold = 300;


void setup() {
  Serial.begin(115200);
  esp01.begin(115200);
  Wire.begin();
  lcd.begin(&Wire);
  lcd.display();
  lcd.backlight();

  sendCommand("AT", 5, "OK");
  sendCommand("AT+CWMODE=1", 5, "OK");
  sendCommand("AT+CWJAP=\"" + WIFI_SSID + "\",\"" + WIFI_PASS + "\"", 20, "OK");
}

void loop() {
 int soilValue = analogRead(SOIL_PIN);
  Serial.print("Soil Moisture: ");
  Serial.println(soilValue);
   if (soilValue < dryThreshold) {   
    Serial.println("Soil is DRY - Watering...");
    lcd.setCursor(4,0);
    lcd.clear();
    lcd.print("Soil is DRY");
    lcd.setCursor(4,1);
    lcd.print("Watering....");
  } else {  
    Serial.println("Soil is MOIST - Idle");
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Soil is MOIST");
    lcd.setCursor(5,1);
    lcd.print("Idle");
  }

  String getData = "GET /update?api_key=" + API + "&field1=" + String(soilValue);

  sendCommand("AT+CIPMUX=1", 5, "OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT, 15, "OK");
  sendCommand("AT+CIPSEND=0," + String(getData.length() + 4), 4, ">");

  esp01.println(getData);
  delay(1500);

  countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0", 5, "OK");
  delay(2000);
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". AT command => ");
  Serial.print(command);
  Serial.print(" ");
 
  countTimeCommand = 0;

  while (countTimeCommand < (maxTime * 1)) {
    esp01.println(command);
    if (esp01.find(readReplay)) {
      found = true;
      break;
    }
    countTimeCommand++;
  }

  if (found) {
    Serial.println("OK");
    countTrueCommand++;
  } else {
    Serial.println("Fail");
    countTrueCommand = 0;
  }

  found = false;
}