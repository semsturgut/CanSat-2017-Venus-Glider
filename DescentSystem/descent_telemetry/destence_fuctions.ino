String getTime() {
  DateTime now = RTC.now();
  String time_n = String(now.hour(), DEC) + ':' + String(now.minute(), DEC) + ':' + String(now.second(), DEC);
  return time_n;
}

void check_Modules() {
  if (!pressure.begin()) {
    Serial.println("ERR:BMP180");
  }
  if (!RTC.isrunning()) {
    Serial.println("ERR:RTC");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  if (!SD.begin(4)) {
    Serial.println("ERR:SD");
  }
}

float getVoltage() {
  float R2 = 1000.0, R1 = 10000.0;
  float Vout = 0.0;
  float Vin = 0.0;
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  Vout = sensorValue * (5.0 / 1023.0);
  // voltage divider uzerinden giris gerilimi hesaplama
  Vin = Vout * ((R2 + R1) / R2);
  return Vin;
}
//servonun acik pozisyondaki kontrolu//
void servoOpen ()
{
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
void servoClose()
{
  if (myservo.read() > 160) // 160 dereceden buyuk oldugu konumdan cagiracak.
    for (pos = 180; pos >= 1; pos -= 1) // Servo 'kapat'den 'ac' pozisyonuna 180 derece donecek.
    {
      myservo.write(pos); // Belirlenen pozisyona gitmesi isteniyor.
      delay(10); // Pozisyona 10 ms de ulasiyor
    }
}
void buzzerOn () {

  tone(buzzerPin, 261);

}
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
byte read(int reg)
{
  Wire.beginTransmission(0x68); // 0x68 sensor adresine veri transferi baslar
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 1, false); // talep edilen data verisi
  byte val = Wire.read();
  Wire.endTransmission(true);
  return val;
}

void write(int reg, int data)
{
  Wire.beginTransmission(0x68); // 0x68 sensor adresine veri transferi baslar
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission(true);
}
double getPressure()
{
  char status;
  double T, P, p0, a;

  // Sıcaklık olcumu baslar
  // eger istek basariliysa, olculen deger geri doner
  // eger istek basarisizsa, 0 doner.

  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Use '&P' to provide the address of P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          return (P);
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}
//@@BMP side end
