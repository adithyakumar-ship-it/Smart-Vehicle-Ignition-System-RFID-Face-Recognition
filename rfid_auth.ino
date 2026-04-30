#include <SPI.h>
#include <MFRC522.h>

// RFID setup
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

byte authorizedUID[4] = {0xDE, 0xAD, 0xBE, 0xEF};  // Replace with your RFID Tag UID

// Relay and ESP32-CAM input
const int relayPin = 8; // Relay connected here
const int faceRecPin = 7; // ESP32-CAM signal input here

// Variables
bool rfidAuthorized = false;
bool faceRecognized = false;

void setup() {
  Serial.begin(9600);
  
  // Init Relay Pin
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Ensure relay is OFF initially

  // Init Face Recognition Pin
  pinMode(faceRecPin, INPUT);

  // Init RFID
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("System Ready. Scan RFID...");
}

void loop() {

  // 1️⃣ RFID Check
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    
    Serial.print("RFID UID: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    // Check UID
    rfidAuthorized = true;
    for (byte i = 0; i < 4; i++) {
      if (rfid.uid.uidByte[i] != authorizedUID[i]) {
        rfidAuthorized = false;
        break;
      }
    }

    if (rfidAuthorized) {
      Serial.println("RFID Authorized!");
    } else {
      Serial.println("RFID Unauthorized!");
    }

    // Halt RFID read
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  // 2️⃣ Check Face Recognition signal
  faceRecognized = digitalRead(faceRecPin);

  if (rfidAuthorized && faceRecognized) {
    // Both RFID + Face recognized → Turn Relay ON
    digitalWrite(relayPin, HIGH);
    Serial.println("Access Granted → Ignition ON");
  } else {
    // One or both not recognized → Turn Relay OFF
    digitalWrite(relayPin, LOW);
    if (!rfidAuthorized) Serial.println("Waiting for Authorized RFID...");
    if (!faceRecognized) Serial.println("Waiting for Face Recognition...");
  }

  delay(500); // Small delay to prevent spamming
}
