#include <Wire.h>
#include <RTClib.h>
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

int count;
String con_data;

Servo myservo;

int pos = 0; // Bu komutla servonun başlangıç aç pozisyonunu belirliyor.
int buzzerPin = 8;
RTC_DS1307 RTC;
File LOG_TELEMETRY;
SFE_BMP180 pressure;
double baseline; // BMP sensor degerlerinin olcumu icin

int sensorPin = A0; // select the input pin for LDR
int ldr;
int check_light ();

void setup() {
        Serial.begin(9600);
        Wire.begin(); // join i2c bus (address optional for master)
        RTC.begin();
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
