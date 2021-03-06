// Saat fonksiyonu saat:dakika:saniye degeri string olarak geri donuyor
String getTime() {
        // Get the current time and date from the chip.
        Time t = rtc.time();
        // Name the day of the week.
        const String day = dayAsString(t.day);
        // Format the time and date and insert into the temporary buffer.
        char buf[50];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d",
                 t.hr, t.min, t.sec);
        // Print the formatted string to serial so we can see the time.
        time_n = buf;
        return time_n;
}

String dayAsString(const Time::Day day) {
        switch (day) {
        case Time::kSunday: return F("Sunday");
        case Time::kMonday: return F("Monday");
        case Time::kTuesday: return F("Tuesday");
        case Time::kWednesday: return F("Wednesday");
        case Time::kThursday: return F("Thursday");
        case Time::kFriday: return F("Friday");
        case Time::kSaturday: return F("Saturday");
        }
        return F("(unknown day)");
}

byte bcdToDec(byte val)  {
        // Convert binary coded decimal to normal decimal numbers
        return ( (val / 16 * 10) + (val % 16) );
}

// Sensorlerin test edilmesi gerektiginde acin.
// Moduller calisiyor mu diye test ediliyor.
void check_Modules() {
        if (!pressure.begin()) {
                Serial.println(F("ERR:BMP"));
        }
        if (!getVoltage()) {
                Serial.println(F("ERR:VLT"));
        }
        if (!lid_servo.read()) {
                Serial.println(F("ERR:SRV"));
        }
        if (!analogRead(ldrPin)) {
                Serial.println(F("ERR:LDR"));
        }
}

// Voltage divider a gore analog voltage okuma
float getVoltage() {
        float R1 = 9710.0, R2 = 9160.0;
        float Vout = 0.0;
        float Vin = 0.0;
        unsigned int sensorValue = analogRead(voltPin);
        // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
        Vout = sensorValue * (5.0 / 1023.0);
        // voltage divider uzerinden giris gerilimi hesaplama
        Vin = Vout * ((R2 + R1) / R2);
        //Analog giristen veri okuyabiliyorsa hesaplanan degeri geri dondur
        return Vin;
}

// Servonun acik pozisyondaki kontrolu ve acik degil ise servoyu ac
void servoClose () {
        for (pos = 90; pos <= 200; pos += 1) { // goes from 0 degrees to 180 degrees
                // in steps of 1 degree
                lid_servo.write(pos);  // tell servo to go to position in variable 'pos'
                delay(10);           // waits 15ms for the servo to reach the position
        }
        delay(1000);
        lid_servo.detach();
}

// Servonun kapali pozisyondaki kontrolu ve kapali degil ise kapat
void servoOpen() {
        lid_servo.attach(servoPin);
        for (pos = 180; pos >= 80; pos -= 1) {   // Servo 'kapat'den 'ac' pozisyonuna 180 derece donecek.
                lid_servo.write(pos); // Belirlenen pozisyona gitmesi isteniyor.
                delay(5); // Pozisyona 10 ms de ulasiyor
        }
        lid_servo.detach();
}

// Buzzer i acip tonunu 261 e ayarliyor.
void buzzerOn () {
        tone(buzzerPin, 261);
}

// Buzzeri kapatiyor.
void buzzerOff () {
        noTone(buzzerPin);
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
        }
}

// Sicaklik degeri aliniyor. Hata alinirsa -1 degeri donuyor.
double getTemperature() {
        char status;
        double T;

        status = pressure.startTemperature(); // Sicaklik degeri alma islemi baslatiliyor
        if (status != 0) {
                delay(status);
                status = pressure.getTemperature(T); // Sicaklik degeri aliniyor
                if (status != 0) {
                        return T; // Sicaklik degeri fonksiyona donduruluyor.
                }
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
        }
}
//@@BMP side end

// Ortamin aydinlik mi karanlik mi oldugunu anlayan fonksiyon
/*int check_light () {
        ldr_Value = analogRead(ldrPin);
        if (ldr_Value > 500) { // 20 den kucukse ortam karanliktir 0 gonderir
                return 0;
        } else { //20 den buyukse ortam aydinlik 1 gönderir
                return 1;
        }
   }*/

int check_Altitude() {
        if (getAltitude() >= 550) {
                return 1;
        } else {
                return 0;
        }
}

// count degiskenini eepromdan cekme islemi
int getCount() {
        return EEPROM.read(0);
}

// count degiskenini eeproma yazma islemi
void upCount(int up_count) {
        EEPROM.update(0, up_count);
}

// Yedek acilma sistemi: gunesi gordugu anda parasut acilma suresini
// hesaba katip o hesaba gore 400 metreye ulasma suresini hesapliyor.
// dist_X Container'dan ayrildigi anda ki yukseklik
// Test parasutu 500 gr 9.17s
void descentB(float dist_X) {
        float grav = 9.81;
        long previousMillis = 0;
        float dist_Y = 400;
        float time_X = 1.50, time_Y; // Parasut acilana kadar gecen sure
        time_Y = ((dist_X - (grav * pow(time_X, 2) / 2) - dist_Y) / 10.87) * 1000;
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
