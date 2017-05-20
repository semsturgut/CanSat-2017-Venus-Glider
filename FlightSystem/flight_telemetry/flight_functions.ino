// Saat fonksiyonu saat:dakika:saniye degeri string olarak geri donuyor
String getTime() {
  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);
  byte zero = 0x00;
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
  int second = bcdToDec(Wire.read());
  int minute = bcdToDec(Wire.read());
  int hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  String time_n = String(hour, DEC) + ':' + String(minute, DEC) + ':' + String(second, DEC);
  //telemetry.println(time_n);
  //Serial.println(time_n);
  return time_n;
}

byte bcdToDec(byte val)  {
  // Convert binary coded decimal to normal decimal numbers
  return ( (val / 16 * 10) + (val % 16) );
}
// Moduller calisiyor mu diye test ediliyor.
void check_Modules() {
  if (!pressure.begin()) {
    //telemetry.println("ERR:BMP");
    Serial.println("ERR:BMP");
  }
  if (!Wire.requestFrom(DS1307_ADDRESS, 7)) { //RTC check islemine farkli bir algoritma yapilacaktir.
    //telemetry.println("ERR:RTC");
    Serial.println("ERR:RTC");
  }
  if (!SD.begin(10)) {
    //telemetry.println("ERR:SD");
    Serial.println("ERR:SD");
  }

}

// Voltage divider a gore analog voltage okuma
double getVoltage() {
  double R1 = 9710.0, R2 = 9360.0;
  double Vout = 0.0;
  double Vin = 0.0;
  int sensorValue = analogRead(voltPin);
  // Convert the analo reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  Vout = sensorValue * (5.0 / 1023.0);
  // voltage divider uzerinden giris gerilimi hesaplama
  Vin = Vout * ((R2 + R1) / R2);
  //Analog giristen veri okuyabiliyorsa hesaplanan degeri geri dondur
  if (sensorValue) {
    return Vin;
  } else {
    return -1;
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
  } else {
    // if the file didn't open, print an error:
    //telemetry.println("ERR:SD");
    Serial.println("ERR:SD");
  }
}

//@@BMP side start

byte readMag(int reg)
{
  Wire.beginTransmission(0x0C);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(0x0C, 1, false); // talep edilen data verisi
  byte val = Wire.read();
  Wire.endTransmission(true);
  return val;
}

void writeMag(int reg, int data)
{
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
    status = pressure.getTemperature(T); // Sicaklik degeri aliniyor
    if (status != 0) {
      return T; // Sicaklik degeri fonksiyona donduruluyor.
    } else {
      //telemetry.println("ERR:GETTEMP");
      Serial.println("ERR:GETTEMP");
      return -1;
    }
  } else {
    //telemetry.println("ERR:STARTTEMP");
    Serial.println("ERR:STARTTEMP");
    return -1;
  }


}
//magnetometer degerlerini alma islemi
int getHeading() {
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

  return x;
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
    else {
      //telemetry.println("ERR:RETPRESS");
      Serial.println("ERR:RETPRESS");
    }
  }
  else {
    //telemetry.println("ERR:STARTPRESS");
    Serial.println("ERR:STARTPRESS");
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

/*
   // software state durumunu gelen degerlere gore yazilim verilerinin dogrulugu kontrolu
   int softState(String s_time, double s_alt, double s_press, double s_temp, double s_volt, int s_head) {
        if (s_alt != -1 || s_press !=-1 || s_temp != -1 || s_volt != -1) {
                return 0;
        } else {
                return 1;
        }
   }*/

// pitot tube uzerinden hizin gelen analog degerinin m/s olarak degistirilmesi
int getSpeed () {
  return MPXV7002DP.GetAirSpeed();
}
