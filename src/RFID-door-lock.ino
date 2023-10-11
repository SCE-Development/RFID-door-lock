#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>

#include <WiFi.h>

#define REQUEST_STRING "GET / HTTP/1.1\r\nHost:google.com\r\n\r\n"

const char * ssid = "no"; // WIFI NAME
const char * password = "123456789"; // WIFI PASSWORD

PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);	
IPAddress server = IPAddress(142,251,46,238);

// WifiClient client;
// Im pretty sure this is c++. void in the param is unnecessary.
void init_wifi(void) {
  Serial.print("\n\nConnecting to ");
  Serial.print(ssid);
  Serial.print(" ...\n");
  WiFi.begin(ssid, password);
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



WiFiClient client;

/**
 * Fill buffer until a space is read from client or size is reached.
*/
void read_until_space(char * buf, size_t size) {
  int i = 0;
  while(buf[i-1] != ' ' && i < size){
    while(!client.available()) {}
    buf[i] = client.read();
    i++;
  }
  buf[i-1] = '\0'; // replace space
}

/**
 * Make a request to the server.
*/
int query_server(int num) {
  if(!client.connect(server, 80)){      // <<<<<< fixed
    Serial.println("Could not connect to server!");
    delay(1000);
    return -1;
  }


  client.write(REQUEST_STRING);

  Serial.println("----- REQUEST -----");
  Serial.println("----- SERVER RESPONSE ----");

  // HTTP/1.1 200 OK
  // [version] [status code] [status reason]
  char buf[20];
  read_until_space(buf, sizeof(buf));
  Serial.print("HTTP Version: "); Serial.println(buf);
  read_until_space(buf, sizeof(buf));
  Serial.print("Status code: "); Serial.println(buf);
  int status_code = atoi(buf);

  int i = 0;
  while((buf[i-2] != '\r' && i >= 2) && buf[i-1] == '\n' && i < 20){
    while(!client.available()) {}
    buf[i] = client.read();
    i++;
  }
  buf[i-2] = '\0';
  Serial.print("Status Reason: "); Serial.println(buf);
  
  Serial.println("----- END SERVER -----");

  return status_code;
}

void setup(void) {
  Serial.begin(115200);
  Serial.println("Initializing...");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board \n");
    Serial.print("Restarting in 10 seconds \n");
    delay(10000);
    Serial.print("Standby");
    ESP.restart();
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();


  init_wifi();

  query_server(0);
}


void loop(void) {
  Serial.println("Scanning for card...");
  
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      Serial.println("Card Details: Mifare Classic Card (4 byte UID)");
	  
      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	  
	  // Start with block 4 (the first block of sector 1) since sector 0
	  // contains the manufacturer data and it's probably better just
	  // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
	  
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];
		
        // If you want to write something to block 4 to test with, uncomment
		// the following line and this text should be read back in a minute
        // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
        // success = nfc.mifareclassic_WriteDataBlock (4, data);

        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock(4, data);
		
        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Block 4:");
          nfc.PrintHexChar(data, 16);
          Serial.println("");
		  
          // Wait a bit before reading the card again
          delay(1000);
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }
    
    if (uidLength == 7)
    {
      // We probably have a Mifare Ultralight card ...
      Serial.println("Card Details: Mifare Ultralight Tag (7 byte UID)");
	  
      // Try to read the first general-purpose user page (#4)
      Serial.println("Reading page 4");
      uint8_t data[32];
      success = nfc.mifareultralight_ReadPage (4, data);
      if (success)
      {
        // Data seems to have been read ... spit it out
        nfc.PrintHexChar(data, 4);
        Serial.println("");
		
        // Wait a bit before reading the card again
        delay(1000);
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested page!?");
      }
    }
  }
}

