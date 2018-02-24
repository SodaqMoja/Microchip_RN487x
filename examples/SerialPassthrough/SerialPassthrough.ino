/* SerialPassthrough
 *  Written by Brian Schmalz of Schmalz Haus LLC (brian@schmalzhaus.com) for Microchip, Dec 2017
 *  
 *  This sketch simply provides a way to directly talk to an RN4870 or RN4871 module. It assumes that 
 *  the USB serial connection is Serial, and the serial connection to the BLE module is on Serial1.
 *  
 *  If you open up a terminal emulator like TeraTerm and connect to your board's serial port,  you can
 *  talk through the microcontroller to the BLE module. This is very useful for figuring out what commands
 *  the BLE module understand, and debugging connection problems.
 */
 
/* These pins are correct for RGB Station boards. Adjust to fit your hardware. */
#define BLE_MODE_PIN  21
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

