#define BLE_MODE_PIN  6
#define BLE_RESET_PIN 7

void setup() {
  pinMode(BLE_MODE_PIN, OUTPUT);
  digitalWrite(BLE_MODE_PIN, HIGH);
  pinMode(BLE_RESET_PIN, OUTPUT);
  digitalWrite(BLE_RESET_PIN, HIGH);

  Serial.begin(115200);
  Serial1.begin(115200);
  
  while (!Serial) {
    ; // wait for serial port to connect.
  }
}

void loop() // run over and over
{
  if (Serial.available())
    Serial1.write(Serial.read());
  if (Serial1.available())
    Serial.write(Serial1.read());
}

