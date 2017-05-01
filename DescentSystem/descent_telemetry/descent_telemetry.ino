#include <Wire.h>
#include <RTClib.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
#include <SFE_BMP180.h>

Servo myservo;

int pos = 0; // Bu komutla servonun başlangıç aç pozisyonunu belirliyor.
int buzzerPin = 8;
RTC_DS1307 RTC;
File LOG_TELEMETRY;
SFE_BMP180 pressure;
double baseline;

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
        writeMag(0x0A, 0x12); // surekli olcebilmek icin manyetik sensor registeri
}

void loop() {
        Serial.println(getTime());
        delay(1000);
}
