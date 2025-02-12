#define IN1 5  
#define IN2 6  
#define ENA 9  
#define POT A0 // Potentiometer on A0

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  
  digitalWrite(IN1, HIGH); // Set rotation direction
  digitalWrite(IN2, LOW);  
  Serial.begin(9600);
}

void loop() {
  int potValue = analogRead(POT); // Read potentiometer value (0-1023)
  int speed = map(potValue, 0, 1023, 0, 255); // Convert to PWM range (0-255)

  analogWrite(ENA, speed); // Set fan speed
  Serial.print("Potentiometer: ");
  Serial.print(potValue);
  Serial.print(" -> Speed: ");
  Serial.println(speed);

  delay(100); // Short pause
}
