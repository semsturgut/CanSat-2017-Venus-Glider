//GENEL DIKKAT EDILMESI GEREKENLER!!!
//1. PITOT TUBE KUTUPHANESI ICERISINDE BULUNAN KM/H DEGERI ILE M/S DEGERI YER DEGISTIRILMELIDIR
//2. SEA LEVEL PRESSURE (BASELINE) YARISMADAN HEMEN ONCE EEPROM A KAYDEDILECEK !!!!!

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
//2. Camera module kodlari eklenecek
//3. RTC baslatilma fonksiyonu yazilacak

// Butun SubFunction lar
String mainTelemetry(String);
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
int count = 0;

void setup() {
        Serial.begin(19200);
        //telemetry.begin(9600);
        Wire.begin(); // join i2c bus (address optional for master)
        RTC.setRAM(0, (uint8_t *)&startAddr, sizeof(uint16_t)); // Store startAddr in NV-RAM address 0x08
        RTC.getRAM(54, (uint8_t *)&TimeIsSet, sizeof(uint16_t));
        pressure.begin();
        baseline = getPressure();
        check_Modules();
        count = getCount();
        write(0x6B, 0); //Guc yonetimi registeri default:0
        write(0x20, 0); // I2C master kapali, acik olmasini istiyorsaniz 0x20 olmali kapali 0x6A
        write(0x37, 0x02); //Bypass modu acik
        writeMag(0x0A, 0x12); // surekli olcebilmek icin manyetik sensor registeri
        mainTelemetry("DEPLOY");
        mainTelemetry("BOOT");
}

void loop() {
        mainTelemetry("SPIN")
        // yerden 10 metre yukseklige ulastiginda buzzer calsin.
        /* if ( getAltitude() < 10) {
           while (1) {
           buzzerOn();
           }
           }*/
}
