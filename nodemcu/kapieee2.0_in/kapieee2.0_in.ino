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

#define RST_PIN  5  // RST-PIN for RC522 - RFID - SPI - Modul GPIO5 
#define SS_PIN  4  // SDA-PIN for RC522 - RFID - SPI - Modul GPIO4 

const char* SSID = "atolyekapi"; 
const char* PASS = "atolyekapi"; 

const char* mqtt_server = "172.24.1.1";
 
long lastMsg = 0;
char msg[50];
int value = 0;

WiFiClient espClient;
PubSubClient client(espClient);
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

void setup() {
  Serial.begin(9600);    // Initialize serial communications
  pinMode(15, OUTPUT); 
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  digitalWrite(15, LOW);
  delay(25);
  Serial.println(F("Booting...."));
  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  delay(50);
  
  Serial.println("");                      //wifi_connection_begin
  Serial.print("Connecting To ");
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(50);                               //wifi_connection_end

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() { 
  if (!client.connected()) {
    reconnect();
  }
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
  char charBuf[50];
  content.toCharArray(charBuf, 50);
  client.publish("inside", charBuf);
  delay(200);
  Serial.println("READY");
}

String dump_byte_array(byte *buffer, byte bufferSize) {
  String result ="";
  for (byte i = 0; i < bufferSize; i++) {
    result +=(buffer[i]);
  }
  delay(50);
  return result;
}

void open_the_door(){
  digitalWrite(16, HIGH);
  digitalWrite(15, HIGH);
  delay(750);
  digitalWrite(16, LOW);
  digitalWrite(15, LOW);
  delay(50);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  
  if ((char)payload[0] == '1') {
    open_the_door();
  } 
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("I/O");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
