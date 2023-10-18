/**
 * TODO we could hook up the serial to the server (Write serial output to a server connection)
 * TODO Maybe split display code into a seperate header file
 * 
 */
// include/configure.h
#include <Arduino.h>
#include "configure.h"

// Arduino
#include <WiFi.h>
#include <Wire.h>

// rfid-door-lock/lib
#include <PN532_I2C.h>
#include <PN532.h>
#include <display.hpp>

#include <http.hpp>

PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);


/**
 * Initialize wifi on the esp.
*/
void init_wifi() {
  Serial.print("\n\nConnecting to ");
  Serial.print(WIFI_SSID);
  Serial.print(" ...\n");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int retries = 15;
  while((WiFi.status() != WL_CONNECTED) && retries) {
    retries--;
    delay(5000);
    Serial.print("Retrying ...\n");
  }

  if(retries == 0) {
    Serial.println("FAILED to connect to wifi.\n");
  }else {
    Serial.print("Wifi connected!\nIP Address: ");
    Serial.println(WiFi.localIP());
  }
}




/**
 * Make a request to the server.
*/
int query_server(int num) {
  WiFiClient client;
  IPAddress server = SERVER_IP;
  if(!client.connect(server, 80)) {
    Serial.println("Could not connect to server!");
    delay(1000);
    return -1;
  }


  client.write(REQUEST_STRING);

  Serial.println("----- REQUEST -----");
  Serial.println("----- SERVER RESPONSE ----");

  // HTTP/1.1 200 OK
  // [version] [status code] [status reason]
  char buf[RESPONSE_BUFFER_SIZE];
  int status_code = 0;

  http::get_version(buf,sizeof(buf), 1000, client);
  Serial.print("HTTP Version: "); Serial.print(buf);
  http::get_status_code(status_code, buf, sizeof(buf), 1000, client);
  Serial.print("\nStatus code: "); Serial.println(buf);
  http::get_status_reason(buf, sizeof(buf), 1000, client);
  Serial.print("Status Reason: "); Serial.println(buf);
  
  Serial.println("----- END SERVER RESPONSE -----");

  return status_code;
}




void init_pn532() {
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN532 board \n");
    Serial.print("Restarting in 10 seconds \n");
    delay(10000);
    Serial.print("Standby");

    ESP.restart(); // ABORT
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  init_display();
  display_init_status("Initializing lock...");
  delay(500);
  init_pn532();
  display_init_status("Finding Auth Server ...");
  delay(500);
  // init_wifi();
  display_init_status("Finished init!");
  delay(5000);

  // query_server(0);
}


bool read_card(uint8_t * uid, uint8_t& uid_length, uint8_t * data, uint8_t n_pages) {

  uint8_t success;

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uid_length);

  if(!success) {
    display_error();
    return false;
  }

  if(uid_length != 7) {
    // Reject invalid card types.

    display_error();
    return false;
  }

  // Display some basic information about the card
  Serial.println("Found an ISO14443A card");
  Serial.print("  UID Length: ");Serial.print(uid_length, DEC);Serial.println(" bytes");
  Serial.print("  UID Value: ");
  nfc.PrintHex(uid, uid_length);
  Serial.println();


  // We probably have a Mifare Ultralight card ...
  Serial.println("Card Details: Mifare Ultralight Tag (7 byte UID)");

  Serial.println("Reading pages");

  for(int i = 0; i < n_pages; i ++) {
    success = nfc.mifareultralight_ReadPage (i, data + i * 4);
    if(!success) {
      Serial.print("Error reading page ");
      Serial.println(i, DEC);
      display_error();
      return false;
    }
  }


  return true;
}

/// @brief Auth code will go here.
bool authenticate(uint8_t * uid, uint8_t *  data) {

  return true;
}

void loop() {
  display_ready();
  Serial.println("Scanning for card...");
  
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uid_length;
  uint8_t n_pages = 8;
  uint8_t data[32]; // n_pages * 4 bytes per page

  if(!read_card(uid, uid_length, data, n_pages)) {
    /// error.
    delay(3000);
    return;
  }
  display_authenticating();
  if(authenticate(uid, data)) {
    // Authenticated.
    display_auth_success();
  }else {
    // Failed
    display_auth_fail();
  }
  delay(2000);
}

