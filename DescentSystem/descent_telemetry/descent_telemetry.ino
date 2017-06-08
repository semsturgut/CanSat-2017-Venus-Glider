// GENEL DIKKAT EDILMESI GEREKENLER
// EKLENECEK!!
// Yarisma alaninda zemin pressure degeri kaydedilecek
// Yarismadan once butun guc sistemi + saat pili devreden cikarilip bir sure beklenilecek
// Yarismadan once setup daki delaylar silinecek!

#include <Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <SFE_BMP180.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <stdio.h>
#include <DS1302.h>

// RTC register tanimlamasi
#define DS1307_ADDRESS 0x68

// @@@ Duzenlenecekler
// TODO: Test et Barometre sensoru pressure degeri degismez bir degisken seklinde kaydedilecek
// TODO: RTC DS1302 ye gore baslatilma fonksiyonu yazilacak
/* TODO: Sistemin toplamda kac ms da veri gonderildigine bakilacak.
   Millis kullanilabilir.*/
/* TODO: Descent B dusus hizi makinaci arkadaslardan
   alinan verilere gore duzenlenecek*/

// Butun SubFunctionlar
void check_Modules();
String getTime();
float getVoltage();
void servoOpen();
void servoClose();
void buzzerOn();
void buzzerOff();
byte read(int);
void write(int, int);
double getAltitude();
double getTemperature();
double getPressure();
int check_light();
int getCount();
void upCount(int);
void wait2secs();
void descentB(float);
byte bcdToDec(byte);
String dayAsString(const Time::Day day);

// Alt fonksiyonlarda tanimlanan kontrol ve islem degiskenleri
unsigned int pos;
const int buzzerPin = 9;
const int servoPin = 3;
SFE_BMP180 pressure;
double baseline; // BMP sensor degerlerinin olcumu icin
Servo lid_servo;
const int voltPin = A0;
const int ldrPin = A1;
unsigned int ldr_Value;
const int kCePin   = 8;  // Chip Enable
const int kIoPin   = 7;  // Input/Output
const int kSclkPin = 6;  // Serial Clock
DS1302 rtc(kCePin, kIoPin, kSclkPin);

// Veri gonderimi ve veri durumu gozetlenmesi icin tanimlanan degiskenler
String time_n;
String STATE = "DESCENT";
double fall_alt;
unsigned int count = 0;
unsigned int ldr_count = 0;
unsigned int separatedCount = 0;
unsigned int silenceCount = 0;
unsigned int descentCount = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
  lid_servo.attach(servoPin); // Servonun sinyal alacagi pin numarasini belirliyor.
  delay(5000); // Yarismadan once silinecek!
  servoClose(); // servoyu kapali konuma getirir.
  lid_servo.detach();
  pressure.begin(); // bmp sensorunu baslatir
  baseline = getPressure();
  check_Modules(); // Sensorlerin test edilmesi gerektiginde acin.
  count = getCount();
  write(0x6B, 0); //Guc yonetimi registeri default:0
  write(0x6A, 0); // I2C master kapali, acik olmasini istiyorsaniz 0x20 olmali
  write(0x37, 0x02); //Bypass modu acik

  // Initialize a new chip by turning off write protection and clearing the
  // clock halt flag. These methods needn't always be called. See the DS1302
  // datasheet for details.
  rtc.writeProtect(false);
  rtc.halt(false);
  // Make a new time object to set the date and time.
  // Sunday, September 22, 2013 at 01:38:50.
  Time t(2017, 5, 26, 00, 00, 00, Time::kSunday);
  // Set the time and date on the chip.
  rtc.time(t);

  // State kismi icin veri BOOT'da oldugumuzu gosteren veri gonderimi yapiliyor.
  count++;
  Serial.print(F("4773,"));
  Serial.print(F("CONTAINER,"));
  Serial.print(getTime());
  Serial.print(F(","));
  Serial.print(count);
  Serial.print(F(","));
  Serial.print(getAltitude());
  Serial.print(F(","));
  Serial.print(getTemperature());
  Serial.print(F(","));
  Serial.print(getVoltage());
  Serial.print(F(","));
  Serial.print(F("BOOT"));
  Serial.println();
  upCount(count);
  delay(1000);
}

void loop() {
  // isigi bir kere gorurse ldr_count u arttirmaya basliyor
  if (!ldr_count) {
    count++;
    Serial.print(F("4773,"));
    Serial.print(F("CONTAINER,"));
    Serial.print(getTime());
    Serial.print(F(","));
    Serial.print(count);
    Serial.print(F(","));
    Serial.print(getAltitude());
    Serial.print(F(","));
    Serial.print(getTemperature());
    Serial.print(F(","));
    Serial.print(getVoltage());
    Serial.print(F(","));
    Serial.print(F("IDLE"));
    Serial.println();
    upCount(count);
    delay(1000);
  }

  if (!ldr_count && check_light()) {
    ldr_count = 1;
    fall_alt = getAltitude();
  }
  // ldr count arttirilmissa isigi gormus demektir ve sistemler calismaya baslayacaktir
  if (ldr_count) {

    // 100 metrenin uzerinde ve 410 metrenin altinda oldugu zaman servoyu ac
    // ya da sadece 100 metrenin uzerinde oldugu zaman millis iceren ikinci ayrilma fonksiyonunu calistir
    if (/*getAltitude() > 100 &&*/ getAltitude() < 160 && !descentCount) {
      descentCount++;
      servoOpen();
      STATE = "SEPARATED";
    } /*else if (getAltitude() < 140 && lid_servo.read() >= 120) { // TODO: 390 metre ve servo acik olmadigi zaman descenB yi calistirmak gerekiyor
                     descentB(fall_alt);
                     STATE = "PLAN-B";
                     }*/

    count++;
    // Veriyi telemetri ile ground station a gonderdiyor
    Serial.print(F("4773,"));
    Serial.print(F("CONTAINER,"));
    Serial.print(getTime());
    Serial.print(F(","));
    Serial.print(count);
    Serial.print(F(","));
    Serial.print(getAltitude());
    Serial.print(F(","));
    Serial.print(getTemperature());
    Serial.print(F(","));
    Serial.print(getVoltage());
    Serial.print(F(","));
    Serial.print(STATE); // Yazilimin durumuna gore degisiyor.
    Serial.println();
    upCount(count);

    // Servo aciksa kapak acilmis demektir
    // Kapak acildigindan 2 saniye sonra veriyi kesiyor
    /* if (lid_servo.read() < 120) {
             if (silenceCount) {
                     wait2secs();
             }
             silenceCount++;
             STATE = "2SECS-2SILENCE";
      }*/
    delay(980);
    
  }
}
