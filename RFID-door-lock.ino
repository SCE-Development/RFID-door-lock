#define DOOR_PIN 16
#define UNLOCK_DURATION_MS 5000

bool door_active = false;
unsigned long door_unlocked_at = 0;
String line = "";

void HandleCommand(const String &cmd) {
  if (cmd == "PING") {
    Serial.println("PONG");
  } else if (cmd == "UNLOCK") {
    if (door_active) {
      Serial.println("ERR BUSY");
      return;
    }
    digitalWrite(DOOR_PIN, HIGH);
    door_active = true;
    door_unlocked_at = millis();
    Serial.println("OK");
  } else {
    Serial.println("ERR UNKNOWN");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(DOOR_PIN, OUTPUT);
  digitalWrite(DOOR_PIN, LOW);
}

void loop() {
  if (door_active && millis() - door_unlocked_at >= UNLOCK_DURATION_MS) {
    digitalWrite(DOOR_PIN, LOW);
    door_active = false;
  }

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      line.trim();
      if (line.length() > 0) HandleCommand(line);
      line = "";
    } else if (line.length() < 32) {
      line += c;
    }
  }
}
