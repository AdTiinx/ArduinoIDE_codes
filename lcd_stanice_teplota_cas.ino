#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <time.h>

// WiFi připojení
const char* ssid = "YOUR WIFI";
const char* password = "password";

// OpenWeatherMap API
const char* host = "api.openweathermap.org";
const char* city = "your location";
const char* country = "cz";
const char* apiKey = "your apiKey";

// LCD displej
LiquidCrystal_I2C lcd(0x27, 16, 4);

// Časová zóna pro ČR
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

// Proměnné
String currentWeather = "";
float currentTemp = 0.0;
unsigned long lastWeatherUpdate = 0;
const unsigned long weatherInterval = 60000;
String lastTimeStr = "";

String removeDiacritics(String text) {
  text.replace("á", "a"); text.replace("č", "c"); text.replace("ď", "d");
  text.replace("é", "e"); text.replace("ě", "e"); text.replace("í", "i");
  text.replace("ň", "n"); text.replace("ó", "o"); text.replace("ř", "r");
  text.replace("š", "s"); text.replace("ť", "t"); text.replace("ú", "u");
  text.replace("ů", "u"); text.replace("ý", "y"); text.replace("ž", "z");
  text.replace("Á", "A"); text.replace("Č", "C"); text.replace("Ď", "D");
  text.replace("É", "E"); text.replace("Ě", "E"); text.replace("Í", "I");
  text.replace("Ň", "N"); text.replace("Ó", "O"); text.replace("Ř", "R");
  text.replace("Š", "S"); text.replace("Ť", "T"); text.replace("Ú", "U");
  text.replace("Ů", "U"); text.replace("Ý", "Y"); text.replace("Ž", "Z");
  return text;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Pripojuji WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi pripojeno");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi pripojeno!");
  delay(1000);
  lcd.clear();
}

void updateWeather() {
  WiFiClient client;
  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("Chyba spojeni");
    lcd.setCursor(0, 0);
    lcd.print("Chyba spojeni   ");
    return;
  }

  String url = "/data/2.5/weather?q=" + String(city) + "," + String(country) +
               "&appid=" + String(apiKey) + "&units=metric&lang=cz";

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  while (!client.available()) delay(100);

  String line, json = "";
  bool jsonStarted = false;
  while (client.available()) {
    line = client.readStringUntil('\n');
    if (line.startsWith("{")) jsonStarted = true;
    if (jsonStarted) json += line;
  }

  const size_t capacity = 1024;
  DynamicJsonDocument doc(capacity);
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print("JSON chyba: ");
    Serial.println(error.f_str());
    lcd.setCursor(0, 0);
    lcd.print("Chyba JSON      ");
    return;
  }

  currentTemp = doc["main"]["temp"];
  currentWeather = doc["weather"][0]["description"].as<String>();
  currentWeather = removeDiacritics(currentWeather);
}

void loop() {
  if (millis() - lastWeatherUpdate > weatherInterval || currentWeather == "") {
    updateWeather();
    lastWeatherUpdate = millis();
  }

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[6];
    strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
    String nowTime = String(timeStr);

    if (nowTime != lastTimeStr) {
      // 1. řádek – čas
      lcd.setCursor(0, 0);
      lcd.print("Cas: ");
      lcd.print(nowTime);
      lcd.print("       ");

      // 2. řádek – teplota
      lcd.setCursor(0, 1);
      lcd.print("Teplota: ");
      lcd.print(currentTemp, 1);
      lcd.print(" C      ");

      // 3. řádek – popis počasí
      lcd.setCursor(0, 2);
      lcd.print("                ");
      lcd.setCursor(0, 2);
      lcd.print(currentWeather);

      // 4. řádek – datum s dnem
      char dateStr[17];
      strftime(dateStr, sizeof(dateStr), "%a %d.%m.%Y", &timeinfo);
      lcd.setCursor(0, 3);
      lcd.print("                ");
      lcd.setCursor(0, 3);
      lcd.print(dateStr); // např. "Sun 25.05.2025"

      lastTimeStr = nowTime;
    }
  }

  delay(1000);
}
