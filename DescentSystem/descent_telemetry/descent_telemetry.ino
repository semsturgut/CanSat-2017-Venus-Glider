#include <Wire.h>
#include <DS1307new.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
#include <SFE_BMP180.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

SoftwareSerial telemetry(1,0);
//@@@ Duzenlenecekler
//1. RTC check_Modules kontrolu eklenecek
//2. softState kismina RTC de zaman farki var mi yok mu kontrolu eklenecek
//

// Butun SubFunction lar
String getTime();
void check_Modules();
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
int softState(String,double,double,double);
void wait2secs();
void descentB(float);

// RTC kurulum degiskenleri
uint16_t startAddr = 0x0000;            // NV-RAM'de saklamak için başlangıç adresi
uint16_t lastAddr;                      // NV-RAM'de saklamak için yeni adres
uint16_t TimeIsSet = 0xaa55;            // Saatin tekrar ayarlanmamasına yardımcı olur.

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
        telemetry.begin(9600);
        Wire.begin(); // join i2c bus (address optional for master)
        RTC.setRAM(0, (uint8_t *)&startAddr, sizeof(uint16_t)); // Store startAddr in NV-RAM address 0x08
        RTC.getRAM(54, (uint8_t *)&TimeIsSet, sizeof(uint16_t));
        lid_servo.attach(servoPin); // Servonun sinyal alacagi pin numarasini belirliyor.
        servoClose(); // servoyu kapali konuma getirir.
        pressure.begin(); // bmp sensorunu baslatir
        baseline = getPressure();
        check_Modules();
        count = getCount();
        write(0x6B, 0); //Guc yonetimi registeri default:0
        write(0x6A, 0); // I2C master kapali, acik olmasini istiyorsaniz 0x20 olmali
        write(0x37, 0x02); //Bypass modu acik
}

void loop() {
        // isigi bir kere gorurse ldr_count u arttirmaya basliyor
        if (!ldr_count && check_light()) {
                ldr_count = 1;
                fall_alt = getAltitude();
        }
        // ldr count arttirilmissa isigi gormus demektir ve sistemler calismaya baslayacaktir
        if (ldr_count) {
                count++;
                // Butun sensor verilerini alip telemetri formatiyla birlikte string e ceviriyor
                time_now = getTime();
                alt1tude = getAltitude();
                temperature = getTemperature();
                voltage = getVoltage();
                softwarestate = softState(time_now, alt1tude, temperature, voltage);
                con_data = String("4773") + ',' + String("CONTAINER") + ',' + String(time_now) + ',';
                con_data1 = String(count) + ',' + String(alt1tude) + ',' + String(temperature) + ',';
                con_data2 = String(voltage) + ',' + String(softwarestate);

                // Veriyi telemetri ile ground station a gonderdikten sonra SD karta kaydediyor
                telemetry.print(con_data);
                //Serial.print(con_data);
                delay(10);
                telemetry.print(con_data1);
                //Serial.print(con_data1);
                delay(10);
                telemetry.println(con_data2);
                //Serial.println(con_data2);
                upCount(count); // count degerini EEPROM a yaziyor
                saveSD(con_data+con_data1+con_data2); // butun telemetri versini SD karta kaydediyo.
                // 100 metrenin uzerinde ve 410 metrenin altinda oldugu zaman servoyu ac
                // ya da sadece 100 metrenin uzerinde oldugu zaman millis iceren ikinci ayrilma fonksiyonunu calistir
                if (getAltitude() > 100 && getAltitude() < 410) {
                        servoOpen();
                } else if (getAltitude() > 100) {
                        descentB(fall_alt);
                }
                // Servo aciksa kapak acilmis demektir
                // Kapak acildigindan 2 saniye sonra veriyi kesiyor
                if (lid_servo.read() > 160) {
                        wait2secs();
                }
                delay(980);
        }
}
