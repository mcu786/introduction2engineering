/*
  LoRa Node With GPS/BD and TMP
  MCU：ESP32-S
  Lora: ai-tinker RA-02 sx1278 470 
  Tempture Sensor:18b20
  Beidou:ai-thinker BG-01 based by gk9501 soc
  created 2020.8.25
  by Bloomlj
*/

// for BEIDOU
#include <HardwareSerial.h>
HardwareSerial SerialBD(1);
#include <NMEAGPS.h>
NMEAGPS gps;
gps_fix fix;

//for 18b20
#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 25;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
float tmp;
// for Lora
#include <LoRaNow.h>

//vspi for lora radio module
//#define MISO 19
//#define MOSI 23
//#define SCK 18
//#define SS 5

//hspi for lora radio module
#define MISO 12
#define MOSI 13
#define SCK 14
#define SS 15

#define DIO0 4

#define MSG_DELAY 600000 //should sleep,use delay for tem use

void setup() {
  Serial.begin(115200);
  Serial.println("LoRa Node With Beidou and Temperature");
  SerialBD.begin(9600,SERIAL_8N1,16,17);//16 rx,17 tx

  // Start the DS18B20 sensor
  sensors.begin();
  
   LoRaNow.setFrequencyCN(); // Select the frequency 486.5 MHz - Used in China
  // LoRaNow.setFrequencyEU(); // Select the frequency 868.3 MHz - Used in Europe
  // LoRaNow.setFrequencyUS(); // Select the frequency 904.1 MHz - Used in USA, Canada and South America
  // LoRaNow.setFrequencyAU(); // Select the frequency 917.0 MHz - Used in Australia, Brazil and Chile

  // LoRaNow.setFrequency(frequency);
  // LoRaNow.setSpreadingFactor(sf);
  // LoRaNow.setPins(ss, dio0);

   LoRaNow.setPinsSPI(SCK, MISO, MOSI, SS, DIO0); // Only works with ESP32

  if (!LoRaNow.begin()) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }

  LoRaNow.onMessage(onMessage);
  LoRaNow.onSleep(onSleep);
  LoRaNow.showStatus(Serial);
}

void loop() {
  sensors.requestTemperatures(); 
  tmp = sensors.getTempCByIndex(0);
  readBD();
  LoRaNow.loop();
}

void onMessage(uint8_t *buffer, size_t size)
{
  Serial.print("Receive Message: ");
  Serial.write(buffer, size);
  Serial.println();
}

void onSleep()
{
  Serial.println("Sleep");
  //readBD(); 
  
  Serial.println("Lora Send Message");
  delay(4000); // "kind of a sleep"

  //LoRaNow.print(message);
  //LoRaNow.print("message");

    LoRaNow.print(fix.status);  
    LoRaNow.print(",");
    LoRaNow.print(fix.longitude());
    LoRaNow.print(",");
    LoRaNow.print(fix.latitude());    
    LoRaNow.print(",");
    LoRaNow.print(fix.altitude());
    LoRaNow.print(",");
    LoRaNow.print(fix.speed_kph());
    LoRaNow.print(",");
    LoRaNow.print(fix.satellites);
    LoRaNow.print(","); 
    LoRaNow.print(fix.dateTime.year);
    LoRaNow.print("-"); 
    LoRaNow.print(fix.dateTime.month);
    LoRaNow.print("-"); 
    LoRaNow.print(fix.dateTime.date);
    LoRaNow.print("~"); 
    LoRaNow.print(fix.dateTime.hours);
    LoRaNow.print(":"); 
    LoRaNow.print(fix.dateTime.minutes);
    LoRaNow.print(":"); 
    LoRaNow.print(fix.dateTime.seconds);
    LoRaNow.print(",");
    LoRaNow.print(tmp);
    LoRaNow.send();
}



void readBD(){
  if (gps.available( SerialBD )) {
    fix = gps.read();
    Serial.println("--GPS info--");
//    //0 STATUS_NONE, 1 STATUS_EST, 2 STATUS_TIME_ONLY, 3 STATUS_STD or 4 STATUS_DGPS  
    Serial.print("status:");
    Serial.println(fix.status);  
    Serial.print("longitude:");
    Serial.println(fix.longitude());
    Serial.print("latitude:");
    Serial.println(fix.latitude());    
    Serial.print("altitude:");
    Serial.println(fix.altitude());
    Serial.print("speed:");
    Serial.println(fix.speed_kph());
    Serial.print("satellites count:");
    Serial.println(fix.satellites);
    Serial.print("UTC "); 
    Serial.print(fix.dateTime.year);
    Serial.print("-"); 
    Serial.print(fix.dateTime.month);
    Serial.print("-"); 
    Serial.print(fix.dateTime.date);
    Serial.print(" "); 
    Serial.print(fix.dateTime.hours);
    Serial.print(":"); 
    Serial.print(fix.dateTime.minutes);
    Serial.print(":"); 
    Serial.println(fix.dateTime.seconds);
    
  }
 }
