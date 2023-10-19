#include <HTTPClient.h>
#include <PN532.h>
#include <PN532_I2C.h>
#include <WiFi.h>
#include <Wire.h>

const char *ssid = "your_wifi_network_name";
const char *password = "your_wifi_network_password";

void setup() {
  Serial.begin(115200);

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

bool verify_card_over_https(int first_four_card_bytes) {
  char url_buffer[50];
  // the below sprintf assumes first_four_card_bytes when converted to a char
  // is at most 12 characters long (4 byte max number is 255, 3 chars * 4 bytes)
  sprintf(url_buffer, "http://192.168.1.231:9000/verify?data=%d",
          first_four_card_bytes);
  HTTPClient http;

  http.begin(url_buffer);

  int httpResponseCode = http.GET();
  http.end();
  return httpResponseCode == 200;
}

void loop() {
  delay(1000);
  // below is just like `uint8_t fake_card_data[32]`
  uint8_t fake_card_data[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                              12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                              23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
  int card_data_as_int = 0;
  // in RFID-door-lock.ino:109 we only print out the first four bytes, maybe
  // thats all we need to verify?
  // The below loop stores the first four bytes in an int variable
  for (int i = 0; i < 4; i++) {
    card_data_as_int *= 10;
    card_data_as_int += (int)fake_card_data[i];
  }
  bool result = verify_card_over_https(card_data_as_int);
  if (result) {
    Serial.println("Card is valid, door opens");
  } else {
    Serial.println("Card invalid, locked out");
  }
}
