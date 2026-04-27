#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESP8266httpUpdate.h>
#include <time.h> // Time sync ke liye zaroori hai

// --- WiFi Credentials ---
const char* ssid = "realme C65 5G";
const char* password = "chandwani8";

// --- AWS IoT Core Settings ---
const char* aws_endpoint = "a1aruuug3gi6gh-ats.iot.us-east-1.amazonaws.com"; 
const char* subscribe_topic = "esp8266/ota/trigger";

// --- Certificates ---
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

static const char AWS_CERT_CRT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUe8bWInxMxGn6MjBbkEeQq2/tOR4wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MDQyMTAyNDcw
MloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKCzGu5iP3NSnwmKKfnU
U+qkWahy8nsem5QCCQgL4CF2NKCqDX1btzOR8BjRLFPkOMBlL0Sx5bBxaCqYNQcw
mjVsDFd4SAkb9IBBRCuWt0eMNSQLYNKWbMJR0f49ZK9LJnBL6g8Q+MK2akYV1Bfp
KHrGezzPQrrXa1cpFQ1LMZwtPkRQcMKrQ76w6F2j4RwQ3v2+hJRUGRMEjJt/WDds
vTbmjfV+U9KyTkxTaq2fUiv1VzdtaAUHOgFF9L6aVxAT/9nAu/4ohJadX9cntdoO
G9MlsxB0gEzbKFkVs0uuQtVbkAketDT87yJQSEk3oJ98mMBFGS0cAXshccg9xQKb
AbECAwEAAaNgMF4wHwYDVR0jBBgwFoAU5vv3o6RY1V5B0oqW61+O8BgnvAswHQYD
VR0OBBYEFIREi85g6JmCKuhfA6SC4psG479dMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAYtCx70OWbR1l/cVZDEO1QjnOv
1PBlsBguePGRDHOn6J70DcU1NeJgm7fEyf+1cPfSWdAy27XuT48wncSNGebWHH2+
P5twNQo6LvdZa3XjX1Bht4+Hvp/dFY0rGM5gbac4mIqlv0fwN1NfXt4X6gqmJZrR
tDakVqCbqR1MLytTnLAxrxUYrB7niqmr0bb0qkQMNfdk/aRzq+1BWOZSV5qihyiG
ZVfscteDG0HY2ByuE1vNSWUe3bJ3lcyCIOBIl6PXRv2L8x334i8yPZupsMSFnhiL
Dgr5Kk8Z+4NqV46EAKqh+SfaxWQyl23WBJgBdCEW61Bdf8UfxzNfboZB4Cdp
-----END CERTIFICATE-----
)EOF";

static const char AWS_CERT_PRIVATE[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAoLMa7mI/c1KfCYop+dRT6qRZqHLyex6blAIJCAvgIXY0oKoN
fVu3M5HwGNEsU+Q4wGUvRLHlsHFoKpg1BzCaNWwMV3hICRv0gEFEK5a3R4w1JAtg
0pZswlHR/j1kr0smcEvqDxD4wrZqRhXUF+koesZ7PM9CutdrVykVDUsxnC0+RFBw
wqtDvrDoXaPhHBDe/b6ElFQZEwSMm39YN2y9NuaN9X5T0rJOTFNqrZ9SK/VXN21o
BQc6AUX0vppXEBP/2cC7/iiElp1f1ye12g4b0yWzEHSATNsoWRWzS65C1VuQCR60
NPzvIlBISTegn3yYwEUZLRwBeyFxyD3FApsBsQIDAQABAoIBAHh5TbthVIg0bJ5O
QmHVWIQUalLqufarX596VyL9JC5x28qHMk3EsHes9P7l8gGCmzlBokoxxCyfGKS2
LysQuMtMEYSR3A9YMdjlFSJIf2bYKg63qQK3WxnjFTcw94ajJFIT14ksBBKVht68
BqFWXs6Mdl+EUlaYoRRa7DlnIe4enK4bP1sepYhYNlBlnva2RV2U/JJZHMPJz99X
ERzW80odsitrGSXOBHMohSPcjBz0heBNAGZlerR3NP54X6ZKJSS2seJ9uzJMvgbA
MA9XLDx9JoEC1IU4jRS7e4wU6so0gZ+1ZMYgvW8mhzG9h+TOd+0yOazukT+JCAc+
+unS+0ECgYEA0y98g/YTlIzWVHnSKp+cbQCfGzzNQehJDKc6IQlww92bw9Msho48
OgD3SfetlNIFDUCmgbvcsXbKxVP7r1Nubd9mNHGlZ+8MCkL+5uTumxiXi6EKkkbK
ZAmpb8AmLUS2FealjTA/OdPtmz8z26C8Gce5oHr7rUuKQkPzIDCxwRUCgYEAws0B
36K7Ugl+tsakF+t+XZXcL3e38sxo582NQ2oM0EOXNwwtzhitTF+iR57Sah9D/uiG
6VpJz6n2lP2heom4igsMe/RjxyPztLlpvKPYDy8tyWgSDX6b5REfsId3CqCtazCh
RTiGaswafQ9TWxtFpofHhcnyfID8n/+jezHWDS0CgYBEYq4pdTfl9K068a/SyZi3
22mqQkgdPYDaLhSuugXgBxT3lpxmfgyWo9VwCFoXROMJ5ZOFkzXl6ZfDymxH2/Z4
cc0A0oRDYa8+a29riVqSsqXwOxfXQkErn81UOJ6kLgngka2uSJ9QtIXOreq3Bhej
KX+8rdZtyXZd+u7xz5Lf+QKBgQC4n5YT/2Zr9SXMUu244e+tANhjgKKtkkAm2fD1
oLAuPrE1ceyDtW2hwc3K9WLVQ3atRHKA/ZqRH7PdN5jwy1BEPP6DCHf8iD103nmx
WNPwaDWL+n+JnKJh8t3G8EzJnMlfmwJt2YnD9FdyibDl2y4jC/L1ARY6HAHSHnYx
z2TgrQKBgQDQnMlBYi28JQ8ULGjQDrKTyKx3QAXD53V0LGbFexO+PNwV/9JBUeYj
dX4aNbE4zTRySCrcjbUjctzLNFuRHot1OeNTXjZ9dUHIg2pJSAaRRb/DS+Q78JBj
B1R9G19OadoXta37iFw+SHbPSMQSGf6uqJK9geKvwr8crVPeEcJGiw==
-----END RSA PRIVATE KEY-----
)EOF";

WiFiClientSecure net;
PubSubClient client(net);

// --- Time Sync Function (Most Important for rc=-2 fix) ---
void syncTime() {
    configTime(5.5 * 3600, 0, "pool.ntp.org", "time.nist.gov"); // IST (UTC +5:30)
    Serial.print("Waiting for NTP time sync: ");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println("\nTime synced!");
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print("Current time: ");
    Serial.print(asctime(&timeinfo));
}
 // Har update ke saath ise manually badalna hoga



void startOTA() {
    Serial.println("Starting OTA Update from S3...");
    WiFiClient otaClient;
    ESPhttpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    t_httpUpdate_return ret = ESPhttpUpdate.update(otaClient, "http://iotcicdframework.s3-website-us-east-1.amazonaws.com/firmware.bin");

    if (ret == HTTP_UPDATE_FAILED) {
        Serial.printf("Update Failed! (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Update Trigger Received on: ");
    Serial.println(topic);
    startOTA();
}

void connectToAWS() {
    net.setTrustAnchors(new BearSSL::X509List(AWS_CERT_CA));
    net.setClientRSACert(new BearSSL::X509List(AWS_CERT_CRT), new BearSSL::PrivateKey(AWS_CERT_PRIVATE));
    
    client.setServer(aws_endpoint, 8883);
    client.setCallback(callback);

    while (!client.connected()) {
        Serial.print("Attempting AWS IoT connection...");
        if (client.connect("ESP8266_Controller")) { 
            Serial.println("SUCCESS: Connected to AWS!");
            client.subscribe(subscribe_topic);
        } else {
            Serial.print("FAILED, rc=");
            Serial.print(client.state());
            Serial.println(" - Retrying in 5s...");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
    
    syncTime(); // Connection se pehle time sync karna MUST hai
    connectToAWS();
}



void loop() {
    if (!client.connected()) {
        connectToAWS();
    }
    client.loop();
    
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 100) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        lastBlink = millis();
    }
    
}