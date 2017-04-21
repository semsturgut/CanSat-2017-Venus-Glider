
#include <Servo.h>

Servo myservo;

int pos = 0; // Bu komutla servonun başlangıç aç pozisyonunu belirliyor.

void setup()
{
  myservo.attach(9); // Servonun sinyal alacagi pin numarasini belirliyor.
}


void loop()
{


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
  if (myservo.read() > 160)// 160 dereceden buyuk oldugu konumdan cagiracak.
    for (pos = 180; pos >= 1; pos -= 1) // Servo 'kapat'den 'ac' pozisyonuna 180 derece donecek.
    {
      myservo.write(pos); // Belirlenen pozisyona gitmesi isteniyor.
      delay(10); // Pozisyona 10 ms de ulasiyor
    }
}


