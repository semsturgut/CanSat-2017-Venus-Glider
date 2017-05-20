//GENEL DIKKAT EDILMESI GEREKENLER!!!
//1. PITOT TUBE KUTUPHANESI ICERISINDE BULUNAN KM/H DEGERI ILE M/S DEGERI YER DEGISTIRILMELIDIR
//<<<<<<< HEAD
//2. SEA LEVEL PRESSURE (BASELINE) YARISMADAN HEMEN ONCE EEPROM A KAYDEDILECEK !!!!!
//=======
//2.
//>>>>>>> origin/master

#include <Wire.h>
#include <DS1307new.h>
#include <SD.h>
#include <SPI.h>
#include <SFE_BMP180.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "Pressure.h"

// RTC register tanimlamasi
#define DS1307_ADDRESS 0x68
#define PRESSURE_PIN A1

Pressure MPXV7002DP(PRESSURE_PIN);

SoftwareSerial telemetry(1, 0);
//@@@ Duzenlenecekler
//1. softwarestate kismi eklenecek.
//Not: Software state derken yazilimin bulundugu durumdan bahsediyor
// Glider ayrildi mi ,Dususe gecti mi vsvs ..
//3. pressure verisi telemetri formatiyla string e eklenecek --- ECE ARA
//4. Camera module kodlari eklenecek
//<<<<<<< HEAD
//5. check_Modules e pitot tube init fonksiyonunda delay duzenlenip eklenecek
//6. getheading in icine state kismi eklenecek
//7. RTC baslatilma fonksiyonu yazilacak
//8. planorun ciktigi yerde
//=======
//8. RTC baslatilma fonksiyonu yazilacak
//>>>>>>> origin/master

// Butun SubFunction lar
String getTime();
byte bcdToDec(byte val);
void check_Modules();
double getVoltage();
void buzzerOn();
void buzzerOff();
void saveSD(String);
byte read(int);
void write(int, int);
double getAltitude();
double getTemperature();
double getPressure();
int getHeading();
int getSpeed ();
int getCount();
void upCount(int);
//int softState(String, double, ,double,double, double, int);
byte readMag(int);
void writeMag(int, int);

// RTC kurulum degiskenleri
uint16_t startAddr = 0x0000;            // NV-RAM'de saklamak için başlangıç adresi
uint16_t lastAddr;                      // NV-RAM'de saklamak için yeni adres
uint16_t TimeIsSet = 0xaa55;            // Saatin tekrar ayarlanmamasına yardımcı olur.

// Alt fonksiyonlarda tanimlanan kontrol ve islem degiskenleri
int pos;
int buzzerPin = 9;
File LOG_TELEMETRY;
SFE_BMP180 pressure;
double baseline; // BMP sensor degerlerinin olcumu icin
int voltPin = A0;

// Veri gonderimi ve veri durumu gozetlenmesi icin tanimlanan degiskenler
String time_now;
String con_data, con_data1, con_data2;
double alt1tude;
double pressure1;
double temperature;
double voltage;
int heading;
int sp33d;
int softwarestate;
int count = 1;

void setup() {
  Serial.begin(19200);
  //telemetry.begin(9600);
  //SD card begin  
  Wire.begin(); // join i2c bus (address optional for master)
  RTC.setRAM(0, (uint8_t *)&startAddr, sizeof(uint16_t)); // Store startAddr in NV-RAM address 0x08
  RTC.getRAM(54, (uint8_t *)&TimeIsSet, sizeof(uint16_t));
  pressure.begin(); // bmp sensorunu baslatir
  baseline = getPressure();
  check_Modules();
  Serial.println("Pitot tube init...");
  int val = MPXV7002DP.Init();
  Serial.println("init done!");
  count = getCount();
  write(0x6B, 0);    //Guc yonetimi registeri default:0
  write(0x20, 0);    // I2C master kapali, acik olmasini istiyorsaniz 0x20 olmali kapali 0x6A
  write(0x37, 0x02);    //Bypass modu acik
  writeMag(0x0A, 0x12); // surekli olcebilmek icin manyetik sensor registeri
}

void loop() {

  // ldr count arttirilmissa isigi gormus demektir ve sistemler calismaya baslayacaktir
  count++;
  // Butun sensor verilerini alip telemetri formatiyla birlikte string e ceviriyor
  time_now = getTime();
  alt1tude = getAltitude();
  pressure1 = getPressure ();
  temperature = getTemperature();
  voltage = getVoltage();
  heading = getHeading ();
  sp33d = getSpeed();
  //softwarestate = softState(time_now, alt1tude, pressure1, temperature, voltage, heading);
  con_data = String("4773") + ',' + String("GLIDER") + ',' + String(time_now) + ',';
  con_data1 = String(count) + ',' + String(alt1tude) + ',' + String(pressure1) + ',' + String(sp33d) + ',' + String(temperature) + ',';
  con_data2 = String(voltage) + ',' + String(heading) + ',' + String("DEPLOY") + ',' + String ("24");

  // Veriyi telemetri ile ground station a gonderdikten sonra SD karta kaydediyor
  //telemetry.print(con_data);
  Serial.print(con_data);
  delay(10);
  //telemetry.print(con_data1);
  Serial.print(con_data1);
  delay(10);
  //telemetry.println(con_data2);
  Serial.println(con_data2);
  upCount(count); // count degerini EEPROM a yaziyor
  saveSD(con_data + con_data1 + con_data2); // butun telemetri versini SD karta kaydediyo.
  // yerden 10 metre yukseklige ulastiginda buzzer calsin.
  /* if ( getAltitude() < 10) {
     while (1) {
     buzzerOn();
     }

     }*/
  delay(980);
}
