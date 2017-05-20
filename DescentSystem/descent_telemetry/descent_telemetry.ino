
//DESCENT B DUSUS HIZI VE YUKSEKLIGI KONTROL EDILECEK !!
// LDR SENSORU ICIN BIRDEN FAZLA DEGER ALINIP ONA GORE ISIK DENETIMI YAPILICAK

#include <Wire.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
#include <SFE_BMP180.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
// RTC register tanimlamasi
#define DS1307_ADDRESS 0x68

SoftwareSerial telemetry(1, 0);
//@@@ Duzenlenecekler
//3. RTC baslatilma fonksiyonu yazilacak

// Butun SubFunction lar
String getTime();
//void check_Modules();
double getVoltage();
void servoOpen();
void servoClose();
void buzzerOn();
void buzzerOff();
void saveSD(String);
byte read(int);
void write(int, int);
double getAltitude();
double getTemperature();
double getPressure();
int check_light();
int getCount();
void upCount(int);
//int softState(String,double,double,double);
void wait2secs();
void descentB(float);
byte bcdToDec(byte);

// RTC kurulum degiskenleri

// Alt fonksiyonlarda tanimlanan kontrol ve islem degiskenleri
int pos;
int buzzerPin = 9;
int servoPin = 3;
File LOG_TELEMETRY;
SFE_BMP180 pressure;
double baseline; // BMP sensor degerlerinin olcumu icin
Servo lid_servo;
int voltPin = A0;
int ldrPin = A1;
int ldr;

// Veri gonderimi ve veri durumu gozetlenmesi icin tanimlanan degiskenler
String time_now;
String con_data, con_data1, con_data2;
double alt1tude;
double temperature;
double voltage;
int softwarestate;
double fall_alt;
int count = 1;
int ldr_count = 0;

void setup() {
        Serial.begin(19200);
        //telemetry.begin(9600);
        Wire.begin(); // join i2c bus (address optional for master)
        lid_servo.attach(servoPin); // Servonun sinyal alacagi pin numarasini belirliyor.
        servoClose(); // servoyu kapali konuma getirir.
        pressure.begin(); // bmp sensorunu baslatir
        baseline = getPressure();
        //check_Modules();
        count = getCount();
        write(0x6B, 0); //Guc yonetimi registeri default:0
        write(0x6A, 0); // I2C master kapali, acik olmasini istiyorsaniz 0x20 olmali
        write(0x37, 0x02); //Bypass modu acik
        mainTelemetry("START");
        mainTelemetry("BOOT");
}

void loop() {
        // isigi bir kere gorurse ldr_count u arttirmaya basliyor
        if (!ldr_count && check_light()) {
                ldr_count = 1;
                fall_alt = getAltitude();
        }
        // ldr count arttirilmissa isigi gormus demektir ve sistemler calismaya baslayacaktir
        if (ldr_count) {
                mainTelemetry ("DESCENT");
        } else {
                mainTelemetry("LAUNCH");
        }
}
