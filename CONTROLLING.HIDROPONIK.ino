#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Pin definitions
#define LED_PIN 2
#define BUZZER_PIN 3
#define RELAY_PIN 5 
#define DHTPIN 7
#define DHTTYPE DHT11
#define ECHO_PIN 8
#define TRIG_PIN 9
#define PH_PIN A0

// Thresholds
#define BATAS_PH_BAWAH 6.0
#define BATAS_PH_ATAS 7.0
#define BATAS_JARAK_BAWAH 2.0
#define BATAS_JARAK_ATAS 7.0

// Initialize LCD, DHT sensor
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Initialize DHT sensor
  dht.begin();
  
  // Initialize pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PH_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initial states
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Display scrolling message
  lcd.setCursor(0, 0);
  lcd.print("Sistem Monitoring");
  lcd.setCursor(0, 1);
  lcd.print("Tanaman Hidroponik");
  for (int positionCounter = 0; positionCounter < 26; positionCounter++) {
    lcd.scrollDisplayLeft();
    delay(200);
  }
  lcd.clear();
}

void loop() {
  // Read temperature and humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Read pH value
  float phValue = analogRead(PH_PIN);
  float voltage = phValue * (5.0 / 1023.0);
  float ph = 1.4 * voltage + 0.02;

  // Read water level
  long duration, distance;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1;
  
  // Display on LCD
  static int currentDisplay = 0;
  const int numDisplays = 2;

  lcd.clear();
  if (currentDisplay == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Suhu: ");
    lcd.print(t);
    lcd.print("C");
      
    lcd.setCursor(0, 1);
    lcd.print("Humid: ");
    lcd.print(h);
    lcd.print("%");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("pH: ");
    lcd.print(ph);
      
    lcd.setCursor(0, 1);
    lcd.print("Jarak Air: ");
    lcd.print(distance);
    lcd.print("cm");
  }
  currentDisplay = (currentDisplay + 1) % numDisplays;

  // Check conditions and control devices

  if (ph < BATAS_PH_BAWAH || ph > BATAS_PH_ATAS) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Control water pump based on water level
  if (distance > BATAS_JARAK_ATAS) {  // If water level is too low (distance too far)
    digitalWrite(RELAY_PIN, LOW);
  } else {
    digitalWrite(RELAY_PIN, HIGH);
  }
  
  if (distance < BATAS_JARAK_BAWAH){
    digitalWrite(BUZZER_PIN,HIGH);
  } else{
    digitalWrite(BUZZER_PIN,LOW);
  }
  

  // Print data to serial monitor
  Serial.print("Suhu: ");
  Serial.print(t);
  Serial.println(" C");

  Serial.print("Humid: ");
  Serial.print(h);
  Serial.println(" %");
 
  Serial.print("pH: ");
  Serial.print(ph);
  Serial.println(" %");
      
  Serial.print("Jarak Air: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.println();

  // Wait 2 seconds before next loop
  delay(2000);
}