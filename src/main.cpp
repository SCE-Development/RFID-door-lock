/**
 * TODO we could hook up the serial to the server (Write serial output to a server connection)
 * TODO Maybe split display code into a seperate header file
 * TODO maybe wait for user to remove card before performing authentication.
 */

#include <Arduino.h>


#include "configure.h"

// Arduino
#include <WiFi.h>
#include <Wire.h>

// rfid-door-lock/lib
#include <PN532_I2C.h>
#include <PN532.h>
#include <display.hpp>

#include <base64.hpp>
#include <http.hpp>

PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

/**
 * Initialize wifi on the esp.
*/
bool init_wifi() {
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
    return false;
  }else {
    Serial.print("Wifi connected!\nIP Address: ");
    Serial.println(WiFi.localIP());
    return true;
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



/**
 * Initialize PN532
 * returns false on failure.
*/
bool init_pn532() {
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN532 board \n");
    return false;
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();

  return true;
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
  init_wifi();
  display_init_status("Finished init!");
  delay(5000);

  // query_server(0);
}

struct card_info {
  uint8_t uid[7];
  uint8_t uid_length;
  uint8_t data[48];
};


enum card_error {
  ok = 0,
  pn532_error=-1,
  invalid_card_type=-2,
};

card_error read_card(card_info& card) {

  uint8_t success;

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, card.uid, &card.uid_length);

  if(!success) {
    Serial.println("Error when passive reading");
    display_error();
    return card_error::pn532_error;
  }

  if(card.uid_length != 7) {
    // Reject invalid card types.
    Serial.println("Invalid card type");
    display_error();
    return card_error::invalid_card_type;
  }

  // Display some basic information about the card
  Serial.println("Found an ISO14443A card");
  Serial.print("  UID Length: ");Serial.print(card.uid_length, DEC);Serial.println(" bytes");
  Serial.print("  UID Value: ");
  nfc.PrintHex(card.uid, card.uid_length);
  Serial.println();


  // We probably have a Mifare Ultralight card ...
  Serial.println("Card Details: Mifare Ultralight Tag (7 byte UID)");

  Serial.println("Reading pages");
  for(int i = 0; i < (sizeof(card.data) / 4); i ++) {
    success = nfc.mifareultralight_ReadPage (i, card.data + i * 4);
    if(!success) {
      Serial.print("Error reading page ");
      Serial.println(i, DEC);

      display_error();
      return card_error::pn532_error;
    }
  }

  return card_error::ok;
}

card_error write_card(card_info& card) {

  uint8_t success;

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, card.uid, &card.uid_length);

  if(!success) {
    Serial.println("Error when passive reading");
    display_error();
    return card_error::pn532_error;
  }

  if(card.uid_length != 7) {
    // Reject invalid card types.
    Serial.println("Invalid card type");
    display_error();
    return card_error::invalid_card_type;
  }

  // Display some basic information about the card
  Serial.println("Found an ISO14443A card");
  Serial.print("  UID Length: ");Serial.print(card.uid_length, DEC);Serial.println(" bytes");
  Serial.print("  UID Value: ");
  nfc.PrintHex(card.uid, card.uid_length);
  Serial.println();


  // We probably have a Mifare Ultralight card ...
  Serial.println("Card Details: Mifare Ultralight Tag (7 byte UID)");

  Serial.println("Writing pages");
  for(int i = 0; i < (sizeof(card.data) / 4); i ++) {
    // success = nfc.mifareultralight_ReadPage (i, card.data + i * 4);
    success = nfc.mifareultralight_WritePage(i+4, card.data + i * 4);
    if(!success) {
      Serial.print("Error writing page ");
      Serial.println(i, DEC);

      display_error();
      return card_error::pn532_error;
    }
  }

  return card_error::ok;
}

enum auth_result {
  auth_success = 1,
  auth_reject = 0,
  auth_error = -1,
};

/**
 * Perform authentication based on card info.
 * 
 * Returns success, reject, or error.
 */
auth_result authenticate(card_info& card) {

  WiFiClient client;
  IPAddress server = SERVER_IP;
  if(!client.connect(server, 1234)) {
    Serial.println("Could not connect to server!");
    delay(1000);
    return auth_result::auth_error;
  }


  char base64id[1 + 4 * 48 / 3];
  base64url::from_bytes(card.data, 48, base64id);

  Serial.write("Id: ");
  Serial.println(base64id);
  // GET /:id/authenticate?scope=[scope name]&expires_in=[days] HTTP/1.1
  client.write("GET /");
  client.write(base64id);
  client.write("/authenticate?scope=door-lock HTTP/1.1");
  client.write("\r\nHost:172.20.10.2\r\n\r\n");
  
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

  // if not 200, then assume not authenticated.
  if(status_code == 200)
    return auth_result::auth_success;
  return auth_result::auth_reject;
}

void loop() {
  display_ready();
  Serial.println("Scanning for card...");
  
  card_info card;

  if(read_card(card) != card_error::ok) {
    /// error.
    Serial.println("Failed to read card.");
    delay(5000);
    return;
  }

  display_authenticating();

  switch(authenticate(card)) {
  case auth_success:

    // Fire off the MOSFET and let em in.
    display_auth_success();
    break;
  case auth_reject:
    display_auth_fail();
    break;
  case auth_error:
    display_error();
    break;
  }

  
  delay(5000);
}

