// Saat fonksiyonu saat:dakika:saniye degeri string olarak geri donuyor
String getTime() {
        RTC.getTime(); // Saat ve Tarih verilerini al
        String time_n = String(RTC.hour, DEC) + ':' + String(RTC.minute, DEC) + ':' + String(RTC.second, DEC);
        //Serial.println(time_n);
        return time_n;
}

// Moduller calisiyor mu diye test ediliyor.
void check_Modules() {
        if (!pressure.begin()) {
                Serial.println("ERR:BMP");
        }
        if (!RTC.year) { //RTC check islemine farkli bir algoritma yapilacaktir.
                Serial.println("ERR:RTC");
        }
        if (!SD.begin(4)) {
                Serial.println("ERR:SD");
        }
        if (!getVoltage()) {
                Serial.println("ERR:VLT");
        }
        if (!lid_servo.read()) {
                Serial.println("ERR:SRV");
        }
        if (analogRead(ldrPin)) {
                Serial.println("ERR:LDR");
        }
}

// Voltage divider a gore analog voltage okuma
double getVoltage() {
        double R1 = 9710.0, R2 = 9360.0;
        double Vout = 0.0;
        double Vin = 0.0;
        int sensorValue = analogRead(A1);
        // Convert the analo reading (which goes from 0 - 1023) to a voltage (0 - 5V):
        Vout = sensorValue * (5.0 / 1023.0);
        // voltage divider uzerinden giris gerilimi hesaplama
        Vin = Vout * ((R2 + R1) / R2);
        //Analog giristen veri okuyabiliyorsa hesaplanan degeri geri dondur
        if(sensorValue) {
                return Vin;
        } else {
                return -1;
        }
}

// Servonun acik pozisyondaki kontrolu ve acik degil ise servoyu ac
void servoOpen () {
        if (lid_servo.read() < 160) { //160 derecen kucuk oldugu konumundan cagiracak.
                // 1 derecelik adimlarla
                for (pos = 1; pos < 180; pos += 1) { // Servo 'ac'dan 'kapat' pozisyonuna 180 derece donecek.
                        lid_servo.write(pos); // Belirlenen pozisyona gitmesi isteniyor.
                        delay(10); // Pozisyona 10 ms de ulasiyor.
                }
        }
}

// Servonun kapali pozisyondaki kontrolu ve kapali degil ise kapat
void servoClose() {
        if (lid_servo.read() > 160) { // 160 dereceden buyuk oldugu konumdan cagiracak.
                for (pos = 180; pos >= 1; pos -= 1) { // Servo 'kapat'den 'ac' pozisyonuna 180 derece donecek.
                        lid_servo.write(pos); // Belirlenen pozisyona gitmesi isteniyor.
                        delay(10); // Pozisyona 10 ms de ulasiyor
                }
        }
}

// Buzzer i acip tonunu 261 e ayarliyor.
void buzzerOn () {
        tone(buzzerPin, 261);
}

// Buzzeri kapatiyor.
void buzzerOff () {
        noTone(buzzerPin);
}

// SD karta veri kayit ediyor.
void saveSD(String data_t) {
        // open the file. note that only one file can be open at a time,
        // so you have to close this one before opening another.
        LOG_TELEMETRY = SD.open("flight.csv", FILE_WRITE);
        // if the file opened okay, write to it:
        if (LOG_TELEMETRY) {
                LOG_TELEMETRY.println(data_t);
                // close the file:
                LOG_TELEMETRY.close();
                //Serial.println(data_t);
        } else {
                // if the file didn't open, print an error:
                Serial.println("ERR:SDWR");
        }
}

//@@BMP side start

byte read(int reg) {
        Wire.beginTransmission(0x68); // 0x68 sensor adresine veri transferi baslar
        Wire.write(reg);
        Wire.endTransmission(false);
        Wire.requestFrom(0x68, 1, false); // talep edilen data verisi
        byte val = Wire.read();
        Wire.endTransmission(true);
        return val;
}

void write(int reg, int data) {
        Wire.beginTransmission(0x68); // 0x68 sensor adresine veri transferi baslar
        Wire.write(reg);
        Wire.write(data);
        Wire.endTransmission(true);
}

// Basinc sensorunden alinan degerler ile yukseklik olcumu
double getAltitude() {
        double alt, press;
        press = getPressure(); // Basinc degeri fonksiyondan cagiriliyor.
        alt = pressure.altitude(press, baseline); // Metre cinsinden deger doner
        //Yukseklik degeri alabiliyorsa geri dondur.
        if (alt) {
                return alt; // Yukseklik degeri metre cinsinden fonksiyona donduruluyor.
        } else {
                return -1;
        }
}

// Sicaklik degeri aliniyor. Hata alinirsa -1 degeri donuyor.
double getTemperature() {
        char status;
        double T;

        status = pressure.startTemperature(); // Sicaklik degeri alma islemi baslatiliyor
        if (status != 0) {
                delay(status);
                Serial.println(status);
                status = pressure.getTemperature(T); // Sicaklik degeri aliniyor
                if (status != 0) {
                        return T; // Sicaklik degeri fonksiyona donduruluyor.
                } else {
                        Serial.println("ERR:GETTEMP");
                        return -1;
                }
        } else {
                Serial.println("ERR:STARTTEMP");
                return -1;
        }


}

// Basinc degerlerini alma islemi
double getPressure() {
        char status;
        double T, P;
        status = pressure.startPressure(3); // Basinc degeri alma islemi baslatiliyor
        if (status != 0) {
                delay(status);
                status = pressure.getPressure(P, T); // Basinc degeri aliniyor
                if (status != 0) {
                        return (P); // Basinc degeri fonksiyona donduruluyor.
                }
                else Serial.println("ERR:RETPRESS");
        }
        else Serial.println("ERR:STARTPRESS");
}
//@@BMP side end

//ortamin aydinlik mi karanlik mi oldugunu anlayan fonksiyon
int check_light () {
        ldr = analogRead(ldrPin);
        if (ldr < 20) { // 20 den kucukse ortam karanliktir 0 gonderir
                return 0;
        } else { //20 den buyukse ortam aydinlik 1 gönderir
                return 1;
        }
}

// count degiskenini eepromdan cekme islemi
int getCount() {
        return EEPROM.read(0);
}

// count degiskenini eeproma yazma islemi
void upCount(int up_count) {
        EEPROM.update(0,up_count);
}

// software state durumunu gelen degerlere gore yazilim verilerinin dogrulugu kontrolu
int softState(String s_time, double s_alt, double s_temp, double s_volt) {
        if (s_alt != -1 || s_temp != -1 || s_volt != -1) {
                return 0;
        } else {
                return 1;
        }
}

// Yedek acilma sistemi: gunesi gordugu anda parasut acilma suresini
// hesaba katip o hesaba gore 400 metreye ulasma suresini hesapliyor.
// dist_X Container'dan ayrildigi anda ki yukseklik
void descentB(float dist_X) {
        float grav = 9.81;
        long previousMillis = 0;
        float dist_Y = 390; //
        float time_X = 3.00, time_Y; //Parasut acilana kadar gecen sure

        time_Y = ((dist_X - (grav * pow(time_X, 2) / 2) - dist_Y) / 10.87)*1000;

        // ardunio calisma suresi milisaniye cinsinden sayiyor.
        unsigned long currentMillis = millis();
        // eger onceki zamanla simdiki zaman arasindaki fark 400m ye
        // gelene kadar gecen sureden buyuk ise servo acilacak.
        if (currentMillis - previousMillis > time_Y) {
                previousMillis = currentMillis;
                servoOpen();
        }

}

// fonksiyon basladiktan 2 saniye sonra veriyi gondermeyi durdurmak icin
// sonsuz donguye giriyor ve yukseklik 20m nin altina inince buzzer aciliyor.
// 2 saniyeden servo acilis suresini cikardik
void wait2secs() {
        long previousMillis = 0;
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis > 1820) {
                previousMillis = currentMillis;
                while (1) {
                        if (getAltitude() < 20) {
                                buzzerOn ();
                        }
                }
        }
}
