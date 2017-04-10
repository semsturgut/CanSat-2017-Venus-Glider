
int buzzerPin = 12;
void buzzerOn();
void buzzerOff();

void setup() {
  Serial.begin(9600);
  
  }

 void loop() {
    buzzerOn();
    delay (1000);
    buzzerOff ();
    delay (1000);
   }

 void buzzerOn () {
  
  tone(buzzerPin, 261); 
  
 }
void buzzerOff () {

  noTone (buzzerPin); 
}

