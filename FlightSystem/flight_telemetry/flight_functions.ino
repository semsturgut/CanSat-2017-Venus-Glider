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

void check_Modules() {
        if (!pressure.begin()) {
                Serial.println(F("ERR:BMP"));
        }
        /*if (!SD.begin(10)) {
                Serial.println(F("ERR:SD"));
           }*/
        /*if (!cam.begin()) {
                Serial.println(F("ERR:CAM"));
           } else {
                cam.setImageSize(VC0706_640x480);
           }*/
        MPXV7002DP.Init(); // Pitot tube baslatiliyor .. Fakat Uzun suruyor
}

// Voltage divider a gore analog voltage okuma
float getVoltage() {
        float R1 = 9710.0, R2 = 9360.0;
        float Vout = 0.0;
        float Vin = 0.0;
        unsigned int sensorValue = analogRead(voltPin);
        // Convert the analo reading (which goes from 0 - 1023) to a voltage (0 - 5V):
        Vout = sensorValue * (5.0 / 1023.0);
        // voltage divider uzerinden giris gerilimi hesaplama
        Vin = Vout * ((R2 + R1) / R2);
        //Analog giristen veri okuyabiliyorsa hesaplanan degeri geri dondur
        if (sensorValue) {
                return Vin;
        }
}

// Buzzer i acip tonunu 261 e ayarliyor.
void buzzerOn() {
        tone(buzzerPin, 261);
}

// Buzzeri kapatiyor.
void buzzerOff() {
        noTone(buzzerPin);
}

//@@BMP side start

byte readMag(int reg) {
        Wire.beginTransmission(0x0C);
        Wire.write(reg);
        Wire.endTransmission(false);
        Wire.requestFrom(0x0C, 1, false); // talep edilen data verisi
        byte val = Wire.read();
        Wire.endTransmission(true);
        return val;
}

void writeMag(int reg, int data) {
        Wire.beginTransmission(0x0C);
        Wire.write(reg);
        Wire.write(data);
        Wire.endTransmission(true);
}

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
//magnetometer degerlerini alma islemi
float getHeading() {
        int xh = readMag(0x04); // x yonunu oku, high byte
        int xl = readMag(0x03); // x yonunu oku, low byte
        int yh = readMag(0x06);
        int yl = readMag(0x05);
        int zh = readMag(0x08);
        int zl = readMag(0x07);
        readMag(0x09); //Mag modul baska bir olcum yapmak icin
        int x = (xh << 8) | (xl & 0xff);
        int y = (yh << 8) | (yl & 0xff);
        int z = (zh << 8) | (zl & 0xff);
        return atan(y/x)*180/pi;
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

// count degiskenini eepromdan cekme islemi
int getCount() {
        return EEPROM.read(0);
}

// count degiskenini eeproma yazma islemi
void upCount(int up_count) {
        EEPROM.update(0, up_count);
}

// pitot tube uzerinden hizin gelen analog degerinin m/s olarak degistirilmesi
int getSpeed () {
        avarageSpeed = 0;
        for (size_t i = 0; i < 10; i++) {
                avarageSpeed += MPXV7002DP.GetAirSpeed();
        }
        return MPXV7002DP.GetAirSpeed();
}

/*
   // saving camera count to EEPROM
   int getCam_count(){
        return EEPROM.read (1);
   }
   // getting camera count from EEPROM
   int upCam_count(int pic_count){
        EEPROM.update (1, pic_count);
   }*/
