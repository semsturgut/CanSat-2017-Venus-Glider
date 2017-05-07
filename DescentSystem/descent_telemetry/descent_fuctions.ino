//Saat fonksiyonu
String getTime() {
        RTC.getTime(); // Saat ve Tarih verilerini al
        String time_n = String(RTC.hour, DEC) + ':' + String(RTC.minute, DEC) + ':' + String(RTC.second, DEC);
        //Serial.println(time_n);
        return time_n;
}

//Moduller calisiyor mu diye test ediliyor.
void check_Modules() {
        if (!pressure.begin()) {
                Serial.println("ERR:BMP180");
        }
        else if (!RTC.year) { //RTC check islemine farkli bir algoritma yapilacaktir.
                Serial.println("ERR:RTC");

        }
        else if (!SD.begin(4)) {
                Serial.println("ERR:SD");
        } else {
                Serial.println("Sensors are OK");
        }
}

double getVoltage() {
        double R2 = 1000.0, R1 = 10000.0;
        double Vout = 0.0;
        double Vin = 0.0;
        int sensorValue = analogRead(A1);
        // Convert the analo reading (which goes from 0 - 1023) to a voltage (0 - 5V):
        Vout = sensorValue * (5.0 / 1023.0);
        // voltage divider uzerinden giris gerilimi hesaplama
        Vin = Vout * ((R2 + R1) / R2);
        return Vin;
}
//servonun acik pozisyondaki kontrolu//
void servoOpen () {
        if (myservo.read() < 160) //160 derecen kucuk oldugu konumundan cagiracak.
        {
                for (pos = 0; pos < 180; pos += 1) // Servo 'ac'dan 'kapat' pozisyonuna 180 derece donecek.
                { // 1 derecelik adimlarla
                        myservo.write(pos); // Belirlenen pozisyona gitmesi isteniyor.
                        delay(10); // Pozisyona 10 ms de ulasiyor.
                }
        }
}
//servonun kapali pozisyondaki kontrolu//
void servoClose() {
        if (myservo.read() > 160) // 160 dereceden buyuk oldugu konumdan cagiracak.
                for (pos = 180; pos >= 1; pos -= 1) // Servo 'kapat'den 'ac' pozisyonuna 180 derece donecek.
                {
                        myservo.write(pos); // Belirlenen pozisyona gitmesi isteniyor.
                        delay(10); // Pozisyona 10 ms de ulasiyor
                }
}

//Buzzer i acip tonunu 261 e ayarliyor.
void buzzerOn () {
        tone(buzzerPin, 261);
}

//Buzzeri kapatiyor.
void buzzerOff () {
        noTone (buzzerPin);
}

void saveSD(String data_t) {
        // open the file. note that only one file can be open at a time,
        // so you have to close this one before opening another.
        LOG_TELEMETRY = SD.open("flight.csv", FILE_WRITE);
        // if the file opened okay, write to it:
        if (LOG_TELEMETRY) {
                LOG_TELEMETRY.println(data_t);
                // close the file:
                LOG_TELEMETRY.close();
                Serial.println(data_t);
        } else {
                // if the file didn't open, print an error:
                Serial.println("error");
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
        return alt; // Yukseklik degeri metre cinsinden fonksiyona donduruluyor.
}

double getTemperature() {
        char status;
        double T;

        status = pressure.startTemperature(); // Sicaklik degeri alma islemi baslatiliyor
        if (status != 0) {
                delay(status);
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
        ldr = analogRead(sensorPin);
        if (ldr < 20) { // 20 den kucukse ortam karanliktir 0 gonderir
                return 0;
        } else { //20 den buyukse ortam aydinlik 1 gönderir
                return 1;
        }
}

int getCount() {
        return EEPROM.read(0);
}

void upCount(int up_count) {
        EEPROM.update(0,up_count);
}
