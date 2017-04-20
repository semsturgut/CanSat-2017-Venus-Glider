#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;

void setup () {
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}
void loop () {
  Serial.println(getTime());
}

String getTime() {
  DateTime now = RTC.now();
  String time_n = String(now.hour(), DEC) + ':' + String(now.minute(), DEC) + ':' + String(now.second(), DEC);
  return time_n;
}

