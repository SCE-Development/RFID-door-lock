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

## Using the Python Developer Tool
### Setup
- **Create a Python Virtual Environment**: `python3 -m venv .venv`
- **Activate the Virtual Environment**:  
On Mac/Linux: `source .venv/bin/activate`  
On Windows: `.venv\Scripts\activate`  
- Install dependencies: `cd backend && pip install -r requirements.txt`  
### Code
- **To check if a card exists**: 
```
python mock_card_reader.py \ 
    --base-url [base_url] \
    --port [port] \
    --card-bytes [card_bytes] \
    --api-key [api_key]
```
- **Example**: 
```
python mock_card_reader.py \ 
    --base-url localhost \
    --port 8080 \
    --card-bytes 12345 \
    --api-key TESTAPIKEY123
```
- **To add a card**: same command as to check if a card exists, but add the `--add` flag to the end:  
```
python mock_card_reader.py \ 
    --base-url localhost \
    --port 8080 \
    --card-bytes 12345 \
    --api-key TESTAPIKEY123 \
    --add
```
- Note that you may need to use `python3` instead of `python` depending on your system specifications.
- The required fields are `--card-bytes` and `--api-key`. `--base-url` defaults to localhost, and `--port` defaults to 8080 if not provided.
- Once the arguments are parsed, the tool will send an HTTP request to the server and return with the response you desire.
