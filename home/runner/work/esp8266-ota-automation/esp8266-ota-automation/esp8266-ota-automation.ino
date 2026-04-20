#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

const char* ssid = "realme C65 5G";
const char* password = "chandwani8";

// S3 HTTP Endpoint (Verify: Browser mein download honi chahiye ye link)
String updateURL = "http://iotcicdframework.s3-website-us-east-1.amazonaws.com/sketch_apr20a.ino.bin"; 

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT); 

    Serial.println("\n--- OTA Version 1.0 Starting ---");
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    // LED Indication
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); 
    delay(500);

    // Wait 20 seconds then attempt update once
    static bool updateAttempted = false;
    if (millis() > 20000 && !updateAttempted) { 
        updateAttempted = true;
        Serial.println("Starting Update Process...");
        
        WiFiClient client;
        
        // S3 ke redirects handle karne ke liye (Important for AWS)
        ESPhttpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        
        // OTA update call
        t_httpUpdate_return ret = ESPhttpUpdate.update(client,"http://iotcicdframework.s3-website-us-east-1.amazonaws.com/sketch_apr20a.ino.bin");

        switch(ret) {
            case HTTP_UPDATE_FAILED:
                Serial.printf("Update Failed! Error (%d): %s\n", 
                    ESPhttpUpdate.getLastError(), 
                    ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("Server returned no updates.");
                break;

            case HTTP_UPDATE_OK:
                Serial.println("Update Success! System will reboot...");
                // Device automatically reboots on success
                break;
        }
    }
}