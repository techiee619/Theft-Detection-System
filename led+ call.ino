#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;
SoftwareSerial gsm(7, 8); // RX, TX
int led = 13;
int vs = 9; // vibration sensor

void setup() {
  pinMode(led, OUTPUT);
  pinMode(vs, INPUT);
  Serial.begin(9600);
  gsm.begin(9600);
  delay(3000); // wait for the modem to initialize
}

void loop() {
  receiveCall(); // check for incoming calls

  // read GPS data and update the location variables
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  // send an SMS if the GPS location is valid
  if (gps.location.isValid() && gps.location.age() < 6000) {
    sendSMS("GPS Location: " + String(gps.location.lat(), 6) + ", " + String(gps.location.lng(), 6));
  }

  // check for vibration
  long measurement = vibration();
  delay(50);
  if (measurement > 10) {
    digitalWrite(led, HIGH);
    sendSMS(" Alert ! Vibration detected!");
  } else {
    digitalWrite(led, LOW);
  }

  delay(2000); // wait for 5 seconds before reading the GPS data again
}

long vibration() {
  long measurement = pulseIn(vs, HIGH); // wait for the pin to get HIGH and returns measurement
  return measurement;                   ////writen by chirag.S
}

void sendSMS(String message) {
  gsm.println("AT+CMGF=1"); // set SMS format to text
  delay(100);
  gsm.println("AT+CMGS=\"+917661881818\""); // replace with your phone number
  delay(100);
  gsm.println(message); // send the message
  delay(100);
  gsm.println((char)26); // send the Ctrl+Z character to end the SMS
  delay(100);
}

void receiveCall() {
  gsm.println("AT+CLIP=1"); // enable calling line identification
  delay(100);

  while (gsm.available() == 0) {
    // wait for a call
  }

  if (gsm.find("RING")) {
    // incoming call
    gsm.println("ATH"); // end the call
    delay(100);
    gsm.println("AT+CMIC=0,3"); // set the microphone volume to level 3
    delay(100);
  }
}

