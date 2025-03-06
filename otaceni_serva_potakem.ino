#include <Servo.h>

Servo servo;  
#define POT_PIN A0  // Pin potenciometru
#define SERVO_PIN 9 // Pin serva

void setup() {
    servo.attach(SERVO_PIN); // Připojení serva
    Serial.begin(115200);
}

void loop() {
    int potValue = analogRead(POT_PIN);       // Čtení hodnoty z potenciometru (0-1023)
    int angle = map(potValue, 0, 1023, 0, 180); // Převod na úhel (0-180°)

    servo.write(angle); // Nastavení serva
    Serial.print("Uhel: ");
    Serial.println(angle);

    delay(15); // Krátké zpoždění pro stabilitu
}
