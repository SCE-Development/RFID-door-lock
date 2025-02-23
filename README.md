# RFID Door Lock
## Parts list
### Reading the card
- Mifare RC522 RF IC Card Sensor Module [amazon link](https://www.amazon.com/HiLetgo-3pcs-RFID-Kit-Raspberry/dp/B07VLDSYRW)
- ESP32 Development Board [amazon link](https://www.amazon.com/Development-AYWHP-ESP-WROOM-32-Bluetooth-Compatible/dp/B0DG8LRV2F)
- RFID capable card (the above RFID Sensor amazon link includes with purchase)

### Powering the door lock
- IRLZ44N Power Mosfet [amazon link](https://www.amazon.com/ALLECIN-IRLZ44N-Transistors-IRLZ44NPBF-Mosfets/dp/B0CBKH4XGL)
- 10k Ohm 1/2 W through hole Resistor

### The button for adding a card
- 5mm LED Light Diodes
- 220 Ohm 1/2 W through hole Resistor
- Breadboard button

## Running the Server
### Arduino IDE - Board
- Go to `tools > board manager` and search for `esp32`
- Install `esp32` by `Espressif Systems`, specifically version 3.1.0
- Set the board in `tools` as `ESP32-WROOM-DA Module`

### Arduino IDE - Library
- Go to `tools > library`, install `MFRC522` by `Github Community`, version 1.4.11

### Code
- [ ] change the API KEY from `NOTHING_REALLY` to the key used by
 the card verification server
- [ ] change the `ssid` and `password` variables to the appropriate
 values of the network that we need to connect to
- [ ] (optional) if the code does not compile, consider following
 [miguelbalboa/rfid#371](https://github.com/miguelbalboa/rfid/issues/371#issuecomment-1740021871)
