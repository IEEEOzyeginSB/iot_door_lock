
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <PubSubClient.h>
#include "MFRC522.h"

/* wiring the MFRC522 to ESP8266 (ESP-12)
RST     = GPIO5
SDA(SS) = GPIO4 
MOSI    = GPIO13
MISO    = GPIO12
SCK     = GPIO14
GND     = GND
3.3V    = 3.3V
*/

#define RST_PIN  5  // RST-PIN für RC522 - RFID - SPI - Modul GPIO5 
#define SS_PIN  4  // SDA-PIN für RC522 - RFID - SPI - Modul GPIO4 
#define mqtt_server "192.168.2.1"
#define read_topic "RFID"
WiFiClient espClient;
PubSubClient client(espClient);
const char *ssid =  "raspieee";     // change according to your Network - cannot be longer than 32 characters!
const char *pass =  "aaabbbcccddd"; // change according to your Network

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);    // Initialize serial communications
  delay(25);
  Serial.println(F("Booting...."));
  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  wifisetup();
  pinMode(D3, OUTPUT);  //red
  pinMode(D4, OUTPUT);  //green
  pinMode(D8, OUTPUT);  //Buzz
  client.setCallback(callback);
}

void wifisetup(){
    WiFi.begin(ssid, pass);
  
  int retries = 0;
  while ((WiFi.status() != WL_CONNECTED) && (retries < 5)) {
    retries++;
    delay(1000);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected"));
    client.setServer(mqtt_server, 1883);
    if (!client.connected()) {
        reconnect();
      }
  }
  Serial.println(F("Ready!"));
  Serial.println(F("======================================================")); 
}

void loop() { 
  client.loop();
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(50);
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }
  // Show some details of the PICC (that is: the tag/card)
  String content = dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.print(F("Card UID: "));
  Serial.println(content);
  content.toUpperCase();
  if (content == "6915414459"||content == "16213533213"||content == "240569743"||content == "415016513718666108"){    //Authorized cards
    Serial.println("Authorized access");
    digitalWrite(D3, LOW);
    digitalWrite(D4, HIGH);
    Serial.println();
    digitalWrite(D8, HIGH);
    delay(100);
    digitalWrite(D8, LOW);
    delay(100);
    digitalWrite(D8, HIGH);
    delay(100);
    digitalWrite(D8, LOW);
    delay(1000);
    digitalWrite(D4, LOW);
    if (!client.connected()) {  //try to reconnect
      Serial.println("trying to reconnect");  
      wifisetup();
    }
    client.publish(read_topic,(char*) content.c_str());
    Serial.println(F("Scan for Card and print UID:"));
  }
  else{
      Serial.println("Access denied");
      digitalWrite(D4, LOW);
      digitalWrite(D3, HIGH);
      Serial.println();
      digitalWrite(D8, HIGH);
      delay(500);
      digitalWrite(D8, LOW);
      delay(1000);
      digitalWrite(D3, LOW);
      if (!client.connected()) {  //try to reconnect
      Serial.println("trying to reconnect");  
      wifisetup();
      }
      Serial.println(F("Scan for Card and print UID:"));
  }
}

String dump_byte_array(byte *buffer, byte bufferSize) {
  String result ="";
  for (byte i = 0; i < bufferSize; i++) {
    result +=(buffer[i]);
  }
  return result;
}

void reconnect() {
  // Loop until we're reconnected
  int retries = 0;
  while (!client.connected()&&(retries < 300)) {
        retries++;
        Serial.print("Attempting MQTT connection...");
    
          // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;  
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);
      clientName += "-";
      clientName += String(micros() & 0xff, 16);
      Serial.print("Connecting to ");
      Serial.print(mqtt_server);
      Serial.print(" as ");
      Serial.println(clientName);
   // Attempt to connect
  if (client.connect((char*) clientName.c_str())) {
      Serial.println("connected");
      client.subscribe("AUTH");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

String macToStr(const uint8_t* mac){
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}
