#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Wire.h>
#include "RTClib.h"
#include <Servo.h>

Servo myservo;

int pos = 0; // Bu komutla servonun başlangıç aç pozisyonunu belirliyor.
int buzzerPin = 12;
void buzzerOn();
void buzzerOff();
Adafruit_BMP085 bmp;
RTC_DS1307 RTC;

void setup() {
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
  RTC.begin();
  myservo.attach(9); // Servonun sinyal alacagi pin numarasini belirliyor.
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}

void loop() {
  Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");
  Serial.println(getTime());
  Serial.println(getVoltage());
  delay(1000);
  buzzerOn();
  delay (1000);
  buzzerOff ();
  delay (1000);
}
String getTime() {
  DateTime now = RTC.now();
  String time_n = String(now.hour(), DEC) + ':' + String(now.minute(), DEC) + ':' + String(now.second(), DEC);
  return time_n;
}

float getVoltage() {
  float R2 = 1000.0, R1 = 10000.0 ;
  float Vout = 0.0 ;
  float Vin = 0.0;
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  Vout = sensorValue * (5.0 / 1023.0);
  // voltage divider uzerinden giris gerilimi hesaplama
  Vin = Vout * ((R2 + R1) / R2) ;
  return Vin;
}
//servonun acik pozisyondaki kontrolu//
void servoOpen ()
{
  if (myservo.read() < 160) //160 derecen kucuk oldugu konumundan cagiracak.
  {
    for (pos = 0; pos < 180; pos += 1) // Servo 'ac'dan 'kapat' pozisyonuna 180 derece donecek.
    { // 1 derecelik adimlarla
      myservo.write(pos); // Belirlenen pozisyona gitmesi isteniyor.
      delay(10); // Pozisyona 10 ms de ulasiyor.
    }
  }
}
//servonun kapali pozisyondaki kontrolu//
void servoClose()
{
  if (myservo.read() > 160)// 160 dereceden buyuk oldugu konumdan cagiracak.
    for (pos = 180; pos >= 1; pos -= 1) // Servo 'kapat'den 'ac' pozisyonuna 180 derece donecek.
    {
      myservo.write(pos); // Belirlenen pozisyona gitmesi isteniyor.
      delay(10); // Pozisyona 10 ms de ulasiyor
    }
}
void buzzerOn () {

  tone(buzzerPin, 261);

}
void buzzerOff () {

  noTone (buzzerPin);
}



