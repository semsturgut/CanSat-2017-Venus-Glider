
void setup() {
  
  Serial.begin(9600);

}

void loop() {
  
  Serial.println(getVoltage());
  delay(1000);  
}

float getVoltage(){
 float R2= 1000.0, R1=10000.0 ; 
 float Vout=0.0 ; 
 float Vin=0.0;  
 int sensorValue = analogRead(A0);
 // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  Vout = sensorValue * (5.0 / 1023.0); 
 // voltage divider uzerinden giris gerilimi hesaplama
  Vin = Vout * ((R2 + R1) / R2) ;
 return Vin;
}

