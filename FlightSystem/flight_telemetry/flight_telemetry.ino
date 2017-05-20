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
//1. Camera Function da degiskenlerin ismi degistirilecek...
//2. Camera count EEPROMA kaydedilecek.
//3. RTC baslatilma fonksiyonu yazilacak

// Butun SubFunction lar
String mainTelemetry1(String);
String mainTelemetry(String,int);
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
byte readMag(int);
void writeMag(int, int);

// RTC kurulum degiskenleri
uint16_t startAddr = 0x0000;            // NV-RAM'de saklamak için başlangıç adresi
uint16_t lastAddr;                      // NV-RAM'de saklamak için yeni adres
uint16_t TimeIsSet = 0xaa55;            // Saatin tekrar ayarlanmamasına yardımcı olur.

// Fotograf cekim sayisi.
uint8_t data_comCNT;

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
        mainTelemetry("DEPLOY", data_comCNT);
        delay(980);
}

void loop() {
        Serial.println("Loop");
        if (!cam.takePicture())
                Serial.println("Failed to snap!");
        else {
                Serial.println("Picture taken!");
                // Create an image with the name IMAGExx.JPG
                char filename[13];
                strcpy(filename, "IMAGE00.JPG");
                for (int i = 0; i < 100; i++) {
                        filename[5] = '0' + i/10;
                        filename[6] = '0' + i%10;
                        // create if does not exist, do not open existing, write, sync after write
                        if (!SD.exists(filename)) {
                                break;
                        }
                }

                File imgFile = SD.open(filename, FILE_WRITE);
                uint16_t jpglen = cam.frameLength();
                Serial.print(jpglen, DEC);
                Serial.println(" byte image.");

                int32_t time = millis();
                pinMode(8, OUTPUT);
                // Read all the data up to # bytes!
                byte wCount = 0; // For counting # of writes
                while (jpglen > 0) {
                        // read 32 bytes at a time;
                        uint8_t *buffer;
                        uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
                        buffer = cam.readPicture(bytesToRead);
                        imgFile.write(buffer, bytesToRead);
                        if(++wCount >= 64) { // Every 2K, give a little feedback so it doesn't appear locked up
                                Serial.print('.');
                                wCount = 0;
                        }
                        // Saniye basi veri gondermek icin
                        long previousMillis2 = 0;
                        unsigned long currentMillis2 = millis();
                        if (currentMillis2 - previousMillis2 > 980) {
                                previousMillis2 = currentMillis2;
                                mainTelemetry("SPIN",data_comCNT);
                        }
                        //Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");
                        jpglen -= bytesToRead;
                }
                imgFile.close();

                time = millis() - time;
                Serial.println("PICTURE SAVED!");
                data_comCNT += 1;
                Serial.print(time); Serial.println(" ms elapsed");

        }

// yerden 10 metre yukseklige ulastiginda buzzer calsin.
/* if ( getAltitude() < 10) {
   while (1) {
   buzzerOn();
   mainTelemetry("LANDED",data_comCNT);
   }
   }*/
}
