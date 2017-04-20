
#include <Servo.h>

Servo myservo;

int pos = 0; // Bu komutla servonun başlangıç aç pozisyonunu belirliyor.

void setup()
{
  myservo.attach(9); // Servonun sinyal alacağı pin numarasını belirliyor.
}


void loop()
{


}
//servonun açık pozisyondaki kontrolü//
void servoOpen ()
{
  if (myservo.read() < 160) //160 derecen küçük olduğu konumundan çağıracak.
  {
    for (pos = 0; pos < 180; pos += 1) // Servo 'aç'dan 'kapat' pozisyonuna 180 derece dönecek.
    { // 1 derecelik adımlarla
      myservo.write(pos); // Belirlenen pozisyona gitmesi isteniyor.
      delay(10); // Pozisyona 10 ms de ulaşıyor.
    }
  }
}
//servonun kapalı pozisyondaki kontrolü//
void servoClose()
{
  if (myservo.read() > 160)// 160 dereceden büyük olduğu konumdan çağıracak.
    for (pos = 180; pos >= 1; pos -= 1) // Servo 'kapat'den 'aç' pozisyonuna 180 derece dönecek.
    {
      myservo.write(pos); // Belirlenen pozisyona gitmesi isteniyor.
      delay(10); // Pozisyona 10 ms de ulaşıyor
    }
}


