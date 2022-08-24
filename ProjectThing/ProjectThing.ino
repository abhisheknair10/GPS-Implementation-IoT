//ESP32 Chip model = ESP32-D0WDQ5 Rev 3
//This chip has 2 cores
//Chip ID: 14220856

#include <Adafruit_GPS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <uri/UriBraces.h>
#include <uri/UriRegex.h>
#include <HTTPClient.h>

// LED init
int redPin = 15;
int greenPin = 32;

// Run Number
String runNum = "1";

// network credentials
const char* ssid = "network";
const char* password =  "12345678";

// what"s the name of the hardware serial port?
#define GPSSerial Serial2

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

#ifdef NMEA_EXTENSIONS
// Create another GPS object to hold the state of the boat, with no
// communications, so you don"t need to call Boat.begin() in setup. 
// We will build some fake sentences from the Boat data to feed to 
// GPS for testing.
Adafruit_GPS Boat;
#endif

// Set GPSECHO to "false" to turn off echoing the GPS data to the Serial console
// Set to "true" if you want to debug and listen to the raw GPS sentences
#define GPSECHO true

uint32_t timer = millis();

void setup() {

    Serial.begin(115200);
    Serial.println("Adafruit GPS library basic test!");

    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);

    // connect to wifi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
    digitalWrite(greenPin, HIGH);

    HTTPClient http;

    String urlreq = "http://176.58.116.164:3000/newrun";

    http.begin(urlreq);
    int httpCode = http.GET();
    if (httpCode > 0) {
        runNum = http.getString();
    }else {
        Serial.println("Error on HTTP request");
    }

    // 9600 NMEA is the default baud rate for Adafruit MTK GPS"s- some use 4800
    GPS.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ); // 10 second update time

    // Request updates on antenna status, comment out to keep quiet
    GPS.sendCommand(PGCMD_ANTENNA);
    delay(1000);

    // Ask for firmware version
    GPSSerial.println(PMTK_Q_RELEASE);
}

void loop() {

    char c = GPS.read();

    if (GPSECHO)
        if (c)
            Serial.print(c);

    if (GPS.newNMEAreceived()) {
        if (!GPS.parse(GPS.lastNMEA()))
            return;
    }

    // approximately every 1 second or so, random intervals, print out the
    static unsigned nextInterval = 1000;
    if (millis() - timer > nextInterval) {
        timer = millis(); // reset the timer
        nextInterval = 1500 + random(1000);
        // Time in seconds keeps increasing after we get the NMEA sentence.
        // This estimate will lag real time due to transmission and parsing delays,
        // but the lag should be small and should also be consistent.
        float s = GPS.seconds + GPS.milliseconds / 1000. + GPS.secondsSinceTime();
        int m = GPS.minute;
        int h = GPS.hour;
        int d = GPS.day;
        // Adjust time and day forward to account for elapsed time.
        // This will break at month boundaries!!! Humans will have to cope with
        // April 31,32 etc.
        while (s > 60) {
            s -= 60;
            m++;
        }
        while (m > 60) {
            m -= 60;
            h++;
        }
        while (h > 24) {
            h -= 24;
            d++;
        }
        // ISO Standard Date Format, with leading zeros https://xkcd.com/1179
        if (GPS.fix) {
            Serial.print("Location: ");
            Serial.print(GPS.latitude, 6);
            Serial.print(GPS.lat);
            Serial.print(", ");
            Serial.print(GPS.longitude, 6);
            Serial.println(GPS.lon);
            Serial.print("Speed (knots): ");
            Serial.println(GPS.speed);
            Serial.print("Satellites: ");
            Serial.println((int)GPS.satellites);
            
            if ((WiFi.status() == WL_CONNECTED)) {
                digitalWrite(redPin, HIGH);
                HTTPClient http;

                String urlreq = "http://176.58.116.164:3000/append/";
                urlreq += String(runNum);
                urlreq += "/";
                urlreq += String(GPS.latitudeDegrees, 15);
                urlreq += "/";
                urlreq += String(GPS.longitudeDegrees, 15);
        
                http.begin(urlreq);
                int httpCode = http.GET();
            }
            else{
                digitalWrite(redPin, LOW);
            }
        }
    #ifdef NMEA_EXTENSIONS
            char latestBoat[200] = "";
            updateBoat();
            Boat.build(latestBoat, "GN", "RMC");
            GPS.resetSentTime();
            GPS.parse(latestBoat);
    #endif
    }
}

#ifdef NMEA_EXTENSIONS
void updateBoat() { 
    double t = millis() / 1000.;
    double theta = t / 100.;   // slow
    double gamma = theta * 10; // faster
    Boat.latitude = 4400 + sin(theta) * 60;
    Boat.lat = 'N';
    Boat.longitude = 7600 + cos(theta) * 60;
    Boat.lon = 'W';
    Boat.fixquality = 2;
    Boat.speed = 3 + sin(gamma);
    Boat.hour = abs(cos(theta)) * 24;
    Boat.minute = 30 + sin(theta / 2) * 30;
    Boat.seconds = 30 + sin(gamma) * 30;
    Boat.milliseconds = 500 + sin(gamma) * 500;
    Boat.year = 1 + abs(sin(theta)) * 25;
    Boat.month = 1 + abs(sin(gamma)) * 11;
    Boat.day = 1 + abs(sin(gamma)) * 26;
    Boat.satellites = abs(cos(gamma)) * 10;
}
#endif