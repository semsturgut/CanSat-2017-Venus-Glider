#include <Wire.h>
#include <DS1307new.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
#include <SFE_BMP180.h>

String getTime();
void check_Modules();
double getVoltage();
void servoOpen ();
void servoClose();
void buzzerOn ();
void buzzerOff ();
void saveSD(String);
byte read(int);
void write(int, int);
double getAltitude();
double getTemperature();
double getPressure();

uint16_t startAddr = 0x0000;            // NV-RAM'de saklamak için başlangıç adresi
uint16_t lastAddr;                      // NV-RAM'de saklamak için yeni adres
uint16_t TimeIsSet = 0xaa55;            // Saatin tekrar ayarlanmamasına yardımcı olur.

int count;
String con_data;

Servo myservo;

int pos = 0; // Bu komutla servonun başlangıç aç pozisyonunu belirliyor.
int buzzerPin = 8;
File LOG_TELEMETRY;
SFE_BMP180 pressure;
double baseline; // BMP sensor degerlerinin olcumu icin

int sensorPin = A0; // select the input pin for LDR
int ldr;
int check_light ();

void setup() {
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
  RTC.setRAM(0, (uint8_t *)&startAddr, sizeof(uint16_t));// Store startAddr in NV-RAM address 0x08
  RTC.getRAM(54, (uint8_t *)&TimeIsSet, sizeof(uint16_t));
  myservo.attach(9); // Servonun sinyal alacagi pin numarasini belirliyor.
  pressure.begin();
  baseline = getPressure();
  check_Modules();
  write(0x6B, 0); //Guc yonetimi registeri default:0
  write(0x6A, 0); // I2C master kapali, acik olmasini istiyorsaniz 0x20 olmali
  write(0x37, 0x02); //Bypass modu acik
}

void loop() {
  if (check_light()) {
    count++;
    con_data = String("4773") + ',' + String("CONTAINER") + ',' + String(getTime()) + ',' + String(count) + ',' + String(getAltitude()) + ',' + String(getTemperature()) + ',' + String(getVoltage()) + ',' + String("1");
    Serial.println (con_data);

    if (getAltitude() < 410) {
      //servoOpen();
    }
    if (getAltitude() < 10) {
      //buzzerOn ();
    }
    delay(1000);
  }
}
