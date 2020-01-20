/*******
 
 All the resources for this project:
 https://randomnerdtutorials.com/

*******/

#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti WiFiMulti;

int bluLed = 2;
int smokeA0 = A0;
// Your threshold value
int sensorThres = 1400;
int outLed = HIGH;

//const char *ssid = "SmokeMonitor";
//const char *pass = "SmokeMonitorPassword";
const char *ssid = "yourAP";
const char *pass = "yourPassword";

void setup() {
    pinMode(bluLed, OUTPUT);
    pinMode(smokeA0, INPUT);
    Serial.begin(115200);

    // We start by connecting to a WiFi network
    WiFiMulti.addAP( ssid, pass );

    Serial.println();
    Serial.println();
    Serial.print("Waiting for WiFi... ");

    int waitloops = 0;
    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
        waitloops++;
        if (waitloops > 10) 
        {
            Serial.println("Failed to connect to WiFiAP");
            break;
        }
        digitalWrite(bluLed, outLed); outLed = (outLed == HIGH)?LOW:HIGH;
        delay(50);
    }

    if(WiFiMulti.run() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }
    outLed = HIGH;
    digitalWrite(bluLed, outLed); 

}

int newState = LOW;
int prevState = LOW;

void loop() {

  // Pulse remote LED to show we are connected.
  if(WiFiMulti.run() == WL_CONNECTED) {
    int invState = (prevState == HIGH)?LOW:HIGH;
    digitalWrite(bluLed, invState); 
    setRemoteLed(invState);
    delay(50);
    digitalWrite(bluLed, prevState); 
    setRemoteLed(prevState);
  }

  int analogSensor = analogRead(smokeA0);
  
  Serial.print("Pin A0: ");
  Serial.println(analogSensor);

  int digitalLevel = digitalRead(15);
  // Checks if it has reached the threshold value
  //if (analogSensor > sensorThres)
  if (digitalLevel == LOW)
  {
    newState = HIGH;
  }
  else
  {
    newState = LOW;
  }

  digitalWrite(bluLed, newState); 

  //if (prevState != newState)
  {
    setRemoteLed(newState);
  }

  prevState = newState;

  delay(1000);
}

void setRemoteLed(int LedState)
{
//    const uint16_t port = 80;
//    const char * host = "192.168.1.1"; // ip or dns
//    const uint16_t port = 1337;
//    const char * host = "192.168.1.10"; // ip or dns
    const uint16_t port = 80;
    const char * host = "192.168.4.1"; // ip or dns

    Serial.print("Setting remote LED ");
    Serial.println(LedState?"HIGH":"LOW");

    //Serial.print("Connecting to ");
    //Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        Serial.println("Waiting 5 seconds before retrying...");
        delay(5000);
        return;
    }

    // This will send a request to the server
    //uncomment this line to send an arbitrary string to the server
    //client.print("Send this data to the server");
    //uncomment this line to send a basic document request to the server
  if (LedState == HIGH)
  {
    client.print("GET /L HTTP/1.1\n\n");
  }
  else
  {
    client.print("GET /H HTTP/1.1\n\n");
  }

  int maxloops = 0;

  //wait for the server's reply to become available
  while (!client.available() && maxloops < 10)
  {
    maxloops++;
    delay(1); //delay 1 msec
  }
  if (client.available() > 0)
  {
    //read back one line from the server
    String line = client.readStringUntil('\r');
    //Serial.println(line);
  }
  else
  {
    Serial.println("client.available() timed out ");
  }

    //Serial.println("Closing connection.");
    client.stop();

    //Serial.println("Waiting 5 seconds before restarting...");
    //delay(5000);
}
