
#include "FirebaseArduino.h"
#include <CircularBuffer.h>
#include <MAX30100.h>
#include <MAX30100_BeatDetector.h>
#include <MAX30100_Filters.h>
#include <MAX30100_PulseOximeter.h>
#include <MAX30100_Registers.h>
#include <MAX30100_SpO2Calculator.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "ESP8266WebServer.h"
#include <Wire.h>
#define REPORTING_PERIOD_MS 10000  

#define FIREBASE_HOST "iot-project-max30100-e3550-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "KFIMQVvLhD8a9Bfxmqu6I9s2CQqPETDtcNZgCi2L"

#define WIFI_SSID       "Pamuk efe"
#define WIFI_PASSWORD   "Annesinin1oglusu"

PulseOximeter pox;

uint32_t tsLastReport = 0;

void onBeatDetected()
{
    Serial.println("Beat!");
}

void setup()
{
    Serial.begin(115200);
    pinMode(2, OUTPUT);
    Serial.print("Initializing Pulse Oximeter..");
    pinMode(16, OUTPUT);
    if (!pox.begin())
    {
      Serial.println("FAILED");
      for(;;);
    }
    else
    {
      Serial.println("SUCCESS");
      digitalWrite(2, HIGH);   //Turn off in-built LED
    }
    // Connect to wifi.
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

    Firebase.set("sensor/HeartBeat", 0);
    Firebase.set("sensor/SpO2", 0);

   if (Firebase.failed()) {
        Serial.print("setting /message failed:");
        Serial.println(Firebase.error());
    }

    pox.begin();
    pox.setOnBeatDetectedCallback(onBeatDetected);
    pox.setIRLedCurrent(MAX30100_LED_CURR_24MA);

}

void loop()
{
    pox.update();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

        float bpm = pox.getHeartRate();
        float spO2 = pox.getSpO2();
        
        if (bpm < 30 || spO2 == 0) {                            
          Serial.println(F("low value from sensor!"));
          return;
        }
        
        digitalWrite(2, LOW); 
        
        Serial.print("Heart rate:");
        Serial.print(bpm);
        Serial.print("bpm / SpO2:");
        Serial.print(spO2);
        Serial.println("%");
        
        digitalWrite(2, HIGH);                                             

        tsLastReport = millis();

        if (bpm != 0) {
            Firebase.setFloat("sensor/HeartBeat", bpm);
            Firebase.setInt("sensor/SpO2", spO2);
        }
    }
}
