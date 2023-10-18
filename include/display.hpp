#pragma once

#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display;


void display_init_status(const char * status) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print("RFID Lock");
  display.setTextSize(1);
  display.setCursor(0, 17);
  display.print(status);
  display.display();

}

void display_ready() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print("Ready!");
  display.setTextSize(1);
  display.setCursor(0, 17);
  display.print("Tap your card on the");
  display.setCursor(0, 24);
  display.print("reader.");
  display.display();
}

void display_reading() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print("Reading!");
  display.setTextSize(1);
  display.setCursor(0, 17);
  display.print("Keep it over the");
  display.setCursor(0, 24);
  display.print("reader.");
  display.display();
}


char hex(uint8_t a) {
    if(a >= 10) return a - 10 + 'A';
    return a + '0';
}

void display_hex(uint8_t n) {
  display.print(hex(n >> 4));
  display.print(hex(n & 0x0F));
}

void display_uid(uint8_t * uid, uint8_t length) {

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print("Reading!");
  display.setTextSize(1);
  display.setCursor(0, 17);
  display.print("UID: 0x");
  for(int i = 0 ; i < length; i ++) {
    display_hex(uid[i]);
  }

  display.display();
}

void display_authenticating() {

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("Authenticating ...");
  display.setTextSize(1);
  display.setCursor(0, 9);
  display.print("Wait a moment.");
  display.display();

}

void display_auth_success() {

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("Authenticated!");
  display.setTextSize(1);
  display.setCursor(0, 9);
  display.print("Opening door ...");
  display.display();

}

void display_auth_fail() {

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("Access denied!");
  // display.setTextSize(1);
  // display.setCursor(0, 17);
  // display.print("Opening door ...");
  display.display();
}

void display_error() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print("Couldn't read");
  display.setTextSize(1);
  display.setCursor(0, 17);
  display.print("Try again in three seconds");
  display.display();
}

void init_display() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}