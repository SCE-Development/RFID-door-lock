#include <HTTPClient.h>
#include <MFRC522.h>
#include <SPI.h>
#include <WiFi.h>

#define SS_PIN 5
#define RST_PIN 0
#define BUTTON_PIN 15
#define ADD_LED_PIN 4
#define DOOR_PIN 16

MFRC522 rfid(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key;

void LOG_INFO(const String &msg) {
  Serial.println("[" + String(millis()) + "] " + msg);
}

const String ssid = "SJSU_guest";
const String password = "";
volatile bool flag = false;
unsigned long flagStartTime = 0;

bool door_active = false;
unsigned long door_unlocked_at = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ADD_LED_PIN, OUTPUT);
  pinMode(DOOR_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  LOG_INFO("Connecting to " + ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  LOG_INFO("Connected to WiFi network with IP Address: " +
           WiFi.localIP().toString());
  SPI.begin();      // Init SPI bus
  rfid.PCD_Init();  // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void CheckIfAddButtonPressed() {
  int high_or_low = LOW;
  if (flag) {
    high_or_low = HIGH;
  }
  if (digitalRead(BUTTON_PIN) == LOW && !flag) {
    flag = true;
    flagStartTime = millis();
    LOG_INFO("Entering ADD_CARD state");
    digitalWrite(ADD_LED_PIN, HIGH);
  }

  // Reset flag after 30 seconds
  if (flag && millis() - flagStartTime >= 30000) {
    flag = false;
    LOG_INFO("Exiting ADD_CARD state due to timeout");
    digitalWrite(ADD_LED_PIN, LOW);
  }
}

bool VerifyCardOverHttps(byte *buffer, byte bufferSize) {
  String card_bytes = "";
  for (byte i = 0; i < bufferSize; i++) {
    if (buffer[i] < 0x10) {
      card_bytes += "0";
    }
    card_bytes += buffer[i];
  }
  LOG_INFO(card_bytes);

  String url = "https://sce.sjsu.edu/api/OfficeAccessCard/verify?cardBytes=";
  url += card_bytes;
  if (flag) {
    url += "&add=1";
  }
  HTTPClient http;
  http.begin(url);
  http.addHeader("X-API-Key", "NOTHING_REALLY");

  int httpResponseCode = http.GET();
  http.end();
  LOG_INFO("Server responded with code " + (String)httpResponseCode);
  bool response_is_ok = httpResponseCode == 200;
  if (response_is_ok && flag) {
    LOG_INFO("Exiting ADD_CARD state due to 200 response");
    digitalWrite(ADD_LED_PIN, LOW);
    flag = false;
  }
  return response_is_ok;
}

void UnlockDoor() {
  // This for loop will be removed, it was meant to
  // retry unlocking the door to when it was being
  // supplied with a voltage too low (5V). The door can
  // be unlocked with 12V and no longer needs this loop.
  //
  // We will remove the loop after verifying that the door
  // unlocks reliably at 12V without the quick retries.
  for (int i = 0; i < 20; i++) {
    digitalWrite(DOOR_PIN, HIGH);
    delay(300);
    digitalWrite(DOOR_PIN, LOW);
  }
  digitalWrite(DOOR_PIN, HIGH);
  delay(9000);
  digitalWrite(DOOR_PIN, LOW);
}

void loop() {
  CheckIfAddButtonPressed();
  if (!rfid.PICC_IsNewCardPresent()) return;

  if (!rfid.PICC_ReadCardSerial()) return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  LOG_INFO(rfid.PICC_GetTypeName(piccType));

  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    LOG_INFO("Your tag is not of type MIFARE Classic.");
    return;
  }

  LOG_INFO("A new card has been detected.");
  bool valid_card = VerifyCardOverHttps(rfid.uid.uidByte, rfid.uid.size);
  if (valid_card) {
    UnlockDoor();
  }
  rfid.PICC_HaltA();

  rfid.PCD_StopCrypto1();
}
