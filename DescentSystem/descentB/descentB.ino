float grav = 9.81;
long previousMillis = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  descentB();
}

void descentB() {
  float dist_X;// Container'dan ayrildigi anda ki yukseklik
  float dist_Y = 390; //
  float time_X = 3.00, time_Y; //Parasut acilana kadar gecen sure
  //dist_X = pressure.altitude(P,baseline);
  dist_X = 600.00;
  time_Y = ((dist_X - (grav * pow(time_X, 2) / 2) - dist_Y) / 10.87)*1000;

  //ardunio calisma suresi milisaniye cinsinden sayiyor.
  unsigned long currentMillis = millis();
  //eger onceki zamanla simdiki zaman arasindaki fark 400m ye gelene kadar gecen sureden buyuk ise servo acilacak.
  if (currentMillis - previousMillis > time_Y) { 
    previousMillis = currentMillis;
    //setServo();
    Serial.println("servoOpen();");
  }

}

