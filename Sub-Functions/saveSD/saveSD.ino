#include <SPI.h>
#include <SD.h>

File LOG_TELEMETRY;

void setup() {
        // Open serial communications and wait for port to open:
        Serial.begin(9600);
        if (!SD.begin(4)) {
                Serial.println("initialization failed!");
                return;
        }

}

void loop() {
        saveSD("data");
}

void saveSD(String data_t){
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
