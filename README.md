<p align="center" style="font-size:48px;"> SCE  - RFID Door Lock </p>

<div align=center > 

[![Status](https://img.shields.io/badge/Status-Active-green)]()
[![Espressif](https://img.shields.io/badge/ESP32-grey?logo=espressif)](https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/)
[![Arduino](https://img.shields.io/badge/Arduino-grey?logo=arduino)](https://www.arduino.cc/)
[![NXP](https://img.shields.io/badge/NXP-grey?logo=nxp)](https://www.nxp.com/docs/en/nxp/data-sheets/PN532_C1.pdf)

</div>


## Table of Contents
- General Description
- Project Setup
- Component Selection
    - ESP32C3
    - PN532
    - OLED Display
- Firmware


### General Description
- Using RFID Enabled cards or phone emulators, we can manage access to the SCE office using the reader installed in the window. Based on [Bradley's Prototype](https://github.com/bradokajima/RFID-Door), this project takes that concept and makes it a flexible wifi enabled tool for club use. 

---
### Project Setup
- Install [PlatformIO](https://platformio.org/platformio-ide) into your VSCode
- You will also need to install the drivers for the SiLabs CP210x Drivers to communicate with the ESP32 over UART 
    - [CP210X Drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads)
- Libraries: 
    - [PN532](https://github.com/elechouse/PN532)
    - [HTTP Client](https://github.com/amcewen/HttpClient)


---
### Component Selection

ESP32
- SoC with wifi capability for HTTP requests

PN532
- RFID/NFC Module for phone and card reading

OLED
- Genaric 0x3C 128x32 OLED display for user feedback

---
### Firmware 
#### 2 Primary functions
<details>
    <summary> Reading NFC Card</summary>

    When an NFC Card is present, take the UID of the card that has been read and attach it to an HTTP Post request to the API. Handle the response accordingly in the access feedback section. 

</details>

<details>
    <summary> Access Feedback</summary>

    Based on the response from the HTTP request when a card is present, have the OLED display read one of two values. Either "Granted" or "Denied"

    - In the access granted case, send power to the relevant pin to trigger the MOSFET that will open the door, and display "Granted" onto the OLED display

    - In the access denied, display the "Denied" onto the OLED Display. 

</details>

---
### Running Evan's Server
- [ ] make sure you have python on your computer
- [ ] create a virtual environment with
```
cd server
mkdir venv
python -m venv ./venv
source ./venv/bin/activate
```
- [ ] install fastapi in the above env
```
pip install -r requirements.txt
```
- [ ] create a `database.json` file with the contents
```json
{
    "1234": true
}
```
- [ ] run the server with
```sh
python server.py --host 0.0.0.0 --port 9000 --database-path database.json
```
- [ ] change `reader.ino` to use a real wifi network and password
- [ ] change `reader.ino` to point to the ip address of your server, you can
get your server's network ip address with `ifconfig` on Unix or `ipconfig /all`
on Windows cmd
- [ ] run `reader.ino` on an ESP32, watch server logs to ensure the device
reaches the server correctly
