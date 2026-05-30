#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Ultrasonic Sensor Pins
#define TRIG_PIN 9
#define ECHO_PIN 10

// Vibration Sensor Pin
#define VIBRATION_PIN 2

// Buzzer Pin
#define BUZZER_PIN 8

TinyGPSPlus gps;

// GPS Module
SoftwareSerial gpsSerial(4, 3);

// GSM Module
SoftwareSerial gsm(11, 12);

long duration;
int distance;

void setup()
{
  Serial.begin(9600);

  gpsSerial.begin(9600);
  gsm.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(VIBRATION_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  Serial.println("Vehicle Accident Prevention System Started");
}

void loop()
{
  obstacleDetection();
  accidentDetection();
  gpsTracking();
}

// Obstacle Detection
void obstacleDetection()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);

  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);

  if(distance < 20)
  {
    Serial.println("Obstacle Detected");

    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// Accident Detection
void accidentDetection()
{
  int vibration = digitalRead(VIBRATION_PIN);

  if(vibration == HIGH)
  {
    Serial.println("Accident Detected");

    digitalWrite(BUZZER_PIN, HIGH);

    sendSMS();

    delay(5000);

    digitalWrite(BUZZER_PIN, LOW);
  }
}

// GPS Tracking
void gpsTracking()
{
  while(gpsSerial.available())
  {
    gps.encode(gpsSerial.read());

    if(gps.location.isUpdated())
    {
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);

      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
    }
  }
}

// Send SMS Alert
void sendSMS()
{
  String latitude = String(gps.location.lat(), 6);
  String longitude = String(gps.location.lng(), 6);

  gsm.println("AT+CMGF=1");
  delay(1000);

  gsm.println("AT+CMGS=\"+91XXXXXXXXXX\"");
  delay(1000);

  gsm.println("ACCIDENT DETECTED!");

  gsm.print("Location: ");
  gsm.print("https://maps.google.com/?q=");
  gsm.print(latitude);
  gsm.print(",");
  gsm.println(longitude);

  gsm.write(26);

  delay(5000);
}
