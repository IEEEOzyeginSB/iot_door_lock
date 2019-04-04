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

const char* SSID = "yilmazdoga"; 
const char* PASS = "tukantukan997"; 

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

void setup() {
  Serial.begin(9600);    // Initialize serial communications
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
}

void loop() { 
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
  delay(2000);
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
