#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

static const int RXPin = 10, TXPin = 11;
String s = "www.google.com/maps/dir/";

unsigned long interval = 10000;
static const uint32_t GPSBaud = 9600;
unsigned long previousMillis = 0;
int data_counter;

const size_t BUFSIZE = 300;
char f_buffer[BUFSIZE];
float *f_buf = (float*)f_buffer;

TinyGPSPlus gps;// The TinyGPSPlus object
SoftwareSerial ss(RXPin, TXPin);// The serial connection to the GPS device


void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);
  
  Serial.println("Starting...");
  ss.println("\r");
  ss.println("AT\r");
  delay(10);

  ss.println("\r");
  ss.println("AT+GPS=1\r");

  delay(100);
  ss.println("AT+CREG=2\r");
  delay(6000);

  //ss.print("AT+CREG?\r");
  ss.println("AT+CGATT=1\r");
  delay(6000);

  ss.println("AT+CGDCONT=1,\"IP\",\"WWW\"\r");
  delay(6000);

  // ss.println("AT+LOCATION=1\r");
  ss.println("AT+CGACT=1,1\r");
  delay(6000);

  //Initialize ends
  //Initialize GPS
  ss.println("\r");
  ss.println("AT+GPS=1\r");
  delay(1000);

  //ss.println("AT+GPSMD=1\r");   // Change to only GPS mode from GPS+BDS, set to 2 to revert to default.
  ss.println("AT+GPSRD=10\r");
  delay(100);

  // set SMS mode to text mode
  ss.println("AT+CMGF=1\r");
  delay(1000);

  //ss.println("AT+LOCATION=2\r");

  Serial.println("Setup Executed");
}

void loop() {
//    if (Serial.available()) {      // If anything comes in Serial (USB),
//      ss.write(Serial.read());   // read it and send it out ss (pins 0 & 1) 
//    }                            //writen by chirag.S
//  
//    if (ss.available()) {     // If anything comes in ss (pins 0 & 1)
//      Serial.write(ss.read());   // read it and send it out Serial (USB)
//    }

  smartDelay(2000);

  if (millis() > 5000 && gps.charsProcessed() < 5)
    Serial.println(F("No GPS data received: check wiring"));

  unsigned long currentMillis = millis();

  if ((unsigned long)(currentMillis - previousMillis) >= interval) {

    send_gps_data();
    previousMillis = currentMillis;
  }
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

void send_gps_data()
{
  if (gps.location.lat() == 0 || gps.location.lng() == 0)
  {
    Serial.println("Return Executed");
    return;
  }

  data_counter++;

  Serial.print("Latitude (deg): ");
  f_buf[data_counter] = gps.location.lat();
  Serial.println(f_buf[data_counter]);

  Serial.print("Longitude (deg): ");
  f_buf[data_counter + 1] = gps.location.lng();
  Serial.println(f_buf[data_counter + 1]);

  Serial.println(data_counter);
  Serial.println();

  s += String(gps.location.lat(), 6);
  s += ",";
  s += String(gps.location.lng(), 6);
  s += "/";

  Serial.println(s);

  if (data_counter >= 5)
  {
    data_counter = 0;

    Serial.println("Sending Message");

    ss.println("AT+CMGF=1\r");
    delay(1000);

    ss.println("AT+CNMI=2,2,0,0,0\r");
    delay(1000);

    ss.print("AT+CMGS=\"+917661881818\"\r");//Replace this with your mobile number
    delay(1000);
    ss.print(s);
    ss.write(0x1A);
    delay(1000);
    s = "www.google.com/maps/dir/";
  }
}