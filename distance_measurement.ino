#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <NewPing.h>

// Definice pinu pro ultrazvukový senzor
#define TRIGGER_PIN  D1  // Pin pro Trigger (změněno na D1)
#define ECHO_PIN     D2  // Pin pro Echo (změněno na D2)
#define MAX_DISTANCE 400 // Maximální měřitelná vzdálenost v cm

// Inicializace objektu pro ultrazvukový senzor
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Inicializace OLED displeje
#define SCREEN_WIDTH 128 // Šířka displeje
#define SCREEN_HEIGHT 64 // Výška displeje
#define OLED_RESET    -1 // Pokud používáte reset pin, nastavte jej, jinak -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Nastavení sériové komunikace
  Serial.begin(115200);
  
  // Inicializace displeje
  Wire.begin(D6, D5); // Zde definujeme piny pro SDA a SCL (D6 a D5)
  
  // I2C adresa displeje - 0x3C je běžná pro mnoho OLED displejů
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED displej nenalezen!"));
    while (true); // Zastaví program, pokud displej není nalezen
  }
  
  display.display();
  delay(2000);  // Pauza na zobrazení startovací obrazovky
  
  // Vymazání displeje
  display.clearDisplay();
}

void loop() {
  // Měření vzdálenosti
  unsigned int distance = sonar.ping_cm();
  
  // Zobrazení vzdálenosti na sériovém monitoru
  Serial.print("Vzdálenost: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // Zobrazení pouze čísla a cm na OLED displeji
  display.clearDisplay();  // Vyčistí displej
  display.setTextSize(3);  // Nastaví velikost textu na 3 (větší text)
  display.setTextColor(SSD1306_WHITE); // Nastaví barvu textu
  display.setCursor(10,10);  // Nastaví kurzor na pozici pro text
  display.print(distance);  // Zobrazí vzdálenost
  display.print(" cm");     // Zobrazí "cm"
  
  // Aktualizace displeje
  display.display();
  
  delay(500); // Pauza mezi měřeními
}
