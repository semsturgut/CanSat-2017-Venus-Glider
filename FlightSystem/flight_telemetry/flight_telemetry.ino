// GENEL DIKKAT EDILMESI GEREKENLER!!!
// 1. PITOT TUBE KUTUPHANESI ICERISINDE BULUNAN KM/H DEGERI ILE M/S DEGERI YER DEGISTIRILMELIDIR
// 2. SEA LEVEL PRESSURE (BASELINE) YARISMADAN HEMEN ONCE EEPROM A KAYDEDILECEK !!!!!
/* @@@@@@ SAAT MODULU AYNI BOARD DA CALISIR AMA FLIGHT UZERINDE CALISMAZ ISE
   Wire.beginTransmission kodunu getTime FONKSIYONUNUN BASINA YAZIN @@@@@@*/
// Buzzer fonksiyonunu acmayi unutmayin!!

#include <Arduino.h>
#include <Wire.h>
#include <DS1307new.h>
#include <SD.h>
#include <SPI.h>
#include <SFE_BMP180.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Adafruit_VC0706.h>
#include "Pressure.h"

// RTC register tanimlamasi
#define DS1307_ADDRESS 0x68
#define PRESSURE_PIN A1

Pressure MPXV7002DP(PRESSURE_PIN);

//SoftwareSerial telemetry(1, 0);
SoftwareSerial cameraconnection = SoftwareSerial(2, 3);
Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

//@@@ Duzenlenecekler
// TODO: Camera Function da degiskenlerin ismi anlamli bir sekilde degistirilecek. @@ ECE
// TODO: Camera count EEPROMA kaydedilecek. @@ ECE
// TODO: RTC DS1302 ye gore baslatilma fonksiyonu yazilacak
// TODO: RTC degeri sadece saniye olarak alinacak! ve surekli arttirilacak. @@ ECE
/* TODO: Software State kismi yazilacak.
   descent_telemetry ye benzer olabilir ya da yeni bir fonkisyon olusturulacak.*/

// Butun SubFunction lar
String getTime();
byte bcdToDec(byte val);
void check_Modules();
double getVoltage();
void buzzerOn();
void buzzerOff();
byte read(int);
void write(int, int);
double getAltitude();
double getTemperature();
double getPressure();
int getHeading();
int getSpeed ();
int getCount();
void upCount(int);
byte readMag(int);
void writeMag(int, int);
int getCam_count();
int upCam_count(int);

// RTC kurulum degiskenleri
uint16_t startAddr = 0x0000;            // NV-RAM'de saklamak için başlangıç adresi
uint16_t lastAddr;                      // NV-RAM'de saklamak için yeni adres
uint16_t TimeIsSet = 0xaa55;            // Saatin tekrar ayarlanmamasına yardımcı olur.

// Fotograf cekim sayisi.
int pic_count = 0;

// Alt fonksiyonlarda tanimlanan kontrol ve islem degiskenleri
int pos;
const int buzzerPin = 9;
SFE_BMP180 pressure;
double baseline; // BMP sensor degerlerinin olcumu icin
const int voltPin = A0;

// Veri gonderimi ve veri durumu gozetlenmesi icin tanimlanan degiskenler
String time_now;
double alt1tude;
double pressure1;
double temperature;
double voltage;
int heading;
int sp33d;
int count = 0;

long previousMillis2 = 0;

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
}

void loop() {
        //Serial.println(F("Loop"));
        if (cam.takePicture()) {
                // Create an image with the name IMAGExx.JPG
                char filename[13]; //dosya char olarak tanimlanip dosya uzunlugu yazilir (13)
                strcpy(filename, "IMAGE00.JPG");
                for (int i = 0; i < 100; i++) {
                        filename[5] = '0' + i / 10;
                        filename[6] = '0' + i % 10;
                        // create if does not exist, do not open existing, write, sync after write
                        if (!SD.exists(filename)) {
                                break;
                        }
                }

                File imgFile = SD.open(filename, FILE_WRITE);
                uint16_t jpglen = cam.frameLength();
                int32_t time = millis();
                pinMode(8, OUTPUT);
                // Read all the data up to # bytes!
                while (jpglen > 0) {
                        // read 32 bytes at a time;
                        uint8_t *buffer;
                        uint8_t bytesToRead = min(64, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
                        buffer = cam.readPicture(bytesToRead);
                        imgFile.write(buffer, bytesToRead);
                        // Saniye basi veri gondermek icin
                        unsigned long currentMillis2 = millis();
                        //Serial.println(currentMillis2);
                        if (currentMillis2 - previousMillis2 > 880) {
                                previousMillis2 = currentMillis2;
                                count++;
                                time_now = getTime();
                                alt1tude = getAltitude();
                                pressure1 = getPressure ();
                                temperature = getTemperature();
                                voltage = getVoltage();
                                heading = getHeading();
                                sp33d = getSpeed();

                                Serial.print(F("4773,"));
                                Serial.print(F("GLIDER,"));
                                Serial.print(time_now);
                                Serial.print(F(","));
                                Serial.print(count);
                                Serial.print(F(","));
                                Serial.print(alt1tude);
                                Serial.print(F(","));
                                Serial.print(pressure1);
                                Serial.print(F(","));
                                Serial.print(sp33d);
                                Serial.print(F(","));
                                Serial.print(temperature);
                                Serial.print(F(","));
                                Serial.print(voltage);
                                Serial.print(F(","));
                                Serial.print(heading);
                                Serial.print(F(","));
                                Serial.print(F("FLIGHT"));
                                Serial.print(F(","));
                                Serial.print(pic_count);
                                Serial.println();
                                upCount(count);           // count degerini EEPROM a yaziyor
                        }
                        jpglen -= bytesToRead;
                }
                imgFile.close();
                pic_count += 1;
                //Needed to buffer new image
                cam.resumeVideo();
        }

        // yerden 10 metre yukseklige ulastiginda buzzer calsin.
        /* if ( getAltitude() < 10) {
           while (1) {
           buzzerOn();
           mainTelemetry("LANDED",pic_count);
           }
           }*/
}
