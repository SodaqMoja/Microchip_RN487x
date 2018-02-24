/*  Sample sketch for illustrating BLE communications with a mobile device app. 
 *  Designed to be used with a chipKIT Fubarino Mini board, but can be used with any Arduino
 *  compatible board. Uses Microchip's RN4870 or RN4871 BLE module.
 *  See https://chipkit.net/wiki/index.php?title=Basic_BLE for a tutorial explaining this sketch.
 *  
 *  Written by Brian Schmalz (brian@schmalzhaus.com) for Microchip, fall 2017.
 *  Based on an example from https://github.com/SodaqMoja/Microchip_RN487x
 *  This sketch is in the public domain
 */

#include <Arduino.h>
#include <RN487x_BLE.h>
#include <SoftPWMServo.h>

/* These are the serial ports to use with a Fubarino Mini. Edit for your hardware setup. */
#define debugSerial Serial      // Serial is Fubarini Mini's USB port
#define bleSerial Serial1       // Serial1 is UART2 on pins 26 (TX) and 25 (RX) to BLE module

/* Analog pin that temp sensor is connected to */
#define TEMP_SENSOR A0
/* Analog pin that pot is connected to */
#define POT_SENSOR A2
/* Maximuim number of milliseconds to wait for USB serial to get ready on boot */
#define SERIAL_TIMEOUT_MS  5000

const char* myDeviceName = "FB Mini";  // Custom Device name
const char* myPrivateServiceUUID = "AD11CF40063F11E5BE3E0002A5D5C51B"; // Custom private service UUID

// Light Sensor
const char* lightCharacteristicUUID = "BF3FBD80063F11E59E690002A5D5C501";  // custom characteristic GATT
const uint8_t lightCharacteristicLen = 2;  // data length (in bytes)
const uint16_t lightHandle = 0x72;
char lightPayload[lightCharacteristicLen*2 + 1];

// Pot
const char* potCharacteristicUUID = "BF3FBD80063F11E59E690002A5D5C502";  // custom characteristic GATT
const uint8_t potCharacteristicLen = 2;  // data length (in bytes)
const uint16_t potHandle = 0x75;
char potPayload[potCharacteristicLen*2 + 1];

// Switch/LED
const char* switchCharacteristicUUID = "BF3FBD80063F11E59E690002A5D5C503";  // custom characteristic GATT
const uint8_t switchCharacteristicLen = 20;  // data length (in bytes)
const uint16_t switchHandle = 0x78;
char switchPayload[switchCharacteristicLen*2 + 1];
const char* ledPayload;

// Temp
const char* temperatureCharacteristicUUID = "BF3FBD80063F11E59E690002A5D5C504";  // custom characteristic GATT
const uint8_t temperatureCharacteristicLen = 2;  // data length (in bytes)
const uint16_t temperatureHandle = 0x7B;
char temperaturePayload[temperatureCharacteristicLen*2 + 1];

// Battery
const char* batteryCharacteristicUUID = "BF3FBD80063F11E59E690002A5D5C505";  // custom characteristic GATT
const uint8_t batteryCharacteristicLen = 2;  // data length (in bytes)
const uint16_t batteryHandle = 0x7E;
char batteryPayload[batteryCharacteristicLen*2 + 1];


/**
 * Read Activity Board temperature sensor in degrees C
 */
float getTemperature()
{
  float accumulator = 0;

  // Make 1000 analog conversions
  for (int i=0; i < 1000; i++)
  {
    float sensorValue = analogRead(TEMP_SENSOR);

    // Convert them to raw volts (raw ADC is 0 to 1023), added fudge factor for accuracy
    float voltage = (sensorValue * (3.3 / 1024.0)) + 0.0025;

    // Sum the raw volts value into the accumulator
    accumulator += voltage;
  }

  // Compute the average voltage over all conversions
  float volt_ave = accumulator/1000.0;

  // Compute Celsius. Sensor reads in 10mV/deg C, with 500mV offset
  float celsius = ((volt_ave * 1000.0) - 500.0) / 10.0;

  return (celsius);
}

/** 
 *  Read pot in milivolts
 */
int getPot()
{
  return ((analogRead(POT_SENSOR) * 3.3) / 1.023);
}

void setup()
{
  
  debugSerial.begin(115200);

  // Wait for PC to connect, give up after SERIAL_TIMEOUT_MS
  while ((!debugSerial) && (millis() < SERIAL_TIMEOUT_MS));

  // Set the optional debug stream
  rn487xBle.setDiag(debugSerial);
  // Initialize the BLE hardware
  rn487xBle.hwInit();
  // Open the communication pipe with the BLE module
  bleSerial.begin(rn487xBle.getDefaultBaudRate());
  // Assign the BLE serial port to the BLE library
  rn487xBle.initBleStream(&bleSerial);
  // Finalize the init. process
  if (rn487xBle.swInit())
  {
    debugSerial.println("Init. procedure done!");
  }
  else
  {
    debugSerial.println("Init. procedure failed!");
    while(1);
  }
  
  // Fist, enter into command mode
  rn487xBle.enterCommandMode();
  // Stop advertising before starting the demo
  rn487xBle.stopAdvertising();
  rn487xBle.clearPermanentAdvertising();
  rn487xBle.clearPermanentBeacon();
  rn487xBle.clearImmediateAdvertising();
  rn487xBle.clearImmediateBeacon();
  rn487xBle.clearAllServices();
  // Set the serialized device name
  rn487xBle.setSerializedName(myDeviceName);
  rn487xBle.setSupportedFeatures(0x4000); // Set to no prompt (no "CMD>")
  rn487xBle.setDefaultServices(DEVICE_INFO_SERVICE);
  // Set the advertising output power (range: min = 5, max = 0)
  rn487xBle.setAdvPower(3);
  rn487xBle.reboot();
  rn487xBle.enterCommandMode();  
  rn487xBle.clearAllServices();
  // Set a private service ...
  rn487xBle.setServiceUUID(myPrivateServiceUUID);
  // which contains ...
  // ...a light sensor (unused) characteristic; readable and can perform notification, 2-octets size
  rn487xBle.setCharactUUID(lightCharacteristicUUID, NOTIFY_PROPERTY, lightCharacteristicLen);
  // ...a pot characteristic; readable and can perform notification, 2-octets size
  rn487xBle.setCharactUUID(potCharacteristicUUID, NOTIFY_PROPERTY, potCharacteristicLen);
  // ...a LED/Switch characteristic; readable and can perform notification, 20-octets size
  rn487xBle.setCharactUUID(switchCharacteristicUUID, WRITE_PROPERTY | NOTIFY_PROPERTY, switchCharacteristicLen);
  // ...a temperature characteristic; readable and can perform notification, 2-octets size
  rn487xBle.setCharactUUID(temperatureCharacteristicUUID, NOTIFY_PROPERTY, temperatureCharacteristicLen);
  // ...a battery (unused) characteristic; readable and can perform notification, 2-octets size
  rn487xBle.setCharactUUID(batteryCharacteristicUUID, NOTIFY_PROPERTY, batteryCharacteristicLen);

  rn487xBle.startPermanentAdvertising(AD_TYPE_FLAGS, "06");
  rn487xBle.startPermanentAdvertising(AD_TYPE_MANUFACTURE_SPECIFIC_DATA, "CD00FE14AD11CF40063F11E5BE3E0002A5D5C51B");
  
  // take into account the settings by issuing a reboot
  rn487xBle.reboot();
  rn487xBle.enterCommandMode();

  rn487xBle.startCustomAdvertising(210);

  debugSerial.println("Fubarino Mini Activity Board as a Peripheral with private service");
  debugSerial.println("================================================");
  debugSerial.println("You can now establish a connection from the Microchip SmartDiscovery App");
  debugSerial.print("with the board: ") ; debugSerial.println(rn487xBle.getDeviceName());
}

void loop()
{
  // Check the connection status
  if (rn487xBle.getConnectionStatus())
  {
    // We are connected to a peer
    debugSerial.print("Connected to a peer central ");
    debugSerial.println(rn487xBle.getLastResponse());

    // Light Sensor
    // 1000d = 21 lux
    // 2000d = 83 lux
    // Since we don't have a light sensor, just hard-code this to a known value
    sprintf(lightPayload, "%04X", 2000);
    rn487xBle.writeLocalCharacteristic(lightHandle, lightPayload);

    // Temperature
    /* Convert to units that the Phone app wants
     * 1000d = -8.47 F (Apple) --- (Android)
     * 1200d = 18.29 F (Apple) 39.76 F (Android)
     * 1500d = 58.84 F (Apple) 85.24 F (Android)
     * 1600d = 71.8 F (Apple)
     * So to convert from Celcius to counts that the app needs, we mulitply by 13.32 and add 1302 (for Apple)
     * Note that the two phone apps interpret this value differently, so we can't create one formula that will
     * work properly for both. This math is set up for the iPhone app.
     */      
    sprintf(temperaturePayload, "%04X", (uint16_t)((getTemperature() * 13.32) + 1302.0));
    rn487xBle.writeLocalCharacteristic(temperatureHandle, temperaturePayload);

    // Pot
    sprintf(potPayload, "%04X", getPot());
    rn487xBle.writeLocalCharacteristic(potHandle, potPayload);

    // Battery
    // 2000 = 7%
    // 3000 = 78%
    // Since we don't have a battery sensor, just hard code this to be a known value
    sprintf(batteryPayload, "%04X", 3000);
    rn487xBle.writeLocalCharacteristic(batteryHandle, batteryPayload);

    // LED
    // This one is the complicated one. Every time the user moves the LED slider on the phone 
    // app, the readLocalCharactistic will get a value like XXXXXXXX2CYYYYYYYY where XXXXXXXX 
    // and YYYYYYYY are four byte hex numbers encoded as ASCII values. (So 0x89AB would be 
    // 38394142) I'm not exactly sure what each value is for, but we just use the first
    // one to dim the LED on the Fubarino.
    if (rn487xBle.readLocalCharacteristic(switchHandle))
    {
      ledPayload = rn487xBle.getLastResponse();
      if ((ledPayload != NULL) && (ledPayload[0] != 'N') && (strlen(ledPayload) >= 9)) // != "N/A" response
      {
        // The difficult thing here is that beacuse the value is encoded as ASCII values, we have
        // to convert each of the 4 ASCII values into the 4 hex bytes, then interpret the hex
        // value as a number. Also, for some reason "%2hhx" doesn't seem to work in this version
        // of sscanf, so we can't directly convert the ASCII values into bytes, we have to have
        // sscanf convert each 2-byte ASCII value into an int, then cast that into to a byte.
        // Then the resulting four byte string can be converted to a 16-bit unsigned number.
        // Seems like way too much work to go through just to get a 16 bit value from the phone.
        uint32_t led = 0;
        uint32_t hexstring_int[4];
        uint8_t hexstring[5];
        uint8_t i;

        for (i=0; i < 4; i++)
        {
          sscanf(&ledPayload[i*2], "%2x", (unsigned int *)&hexstring_int[i]);
          hexstring[i] = hexstring_int[i];
        }
        hexstring[4] = 0;
        
        // led = 500 when slider is all the way right (android) or 1750 (Apple)
        // led = 0 when slider is all the way left (android and Apple)
        // led = 26670 when slider is just a bit in from the left side, decreasing down to 500 on the right side (28221 Apple)
        sscanf((char*)hexstring, "%x", (unsigned int *)&led);
        SoftPWMServoPWMWrite(PIN_LED1, map(led, 28000, 500, 0, 255));
      }
    }

    // Switch
    // Note that because we use the same server characteristic for sending switch data to the phone as well
    // as getting LED information back from the phone, we have to do things in this order (LED read first,
    // then switch write) otherwise the LED read will always return "0001" or "0000" from the switch write.
    sprintf(switchPayload, "000%1d", digitalRead(PIN_BTN1));      
    rn487xBle.writeLocalCharacteristic(switchHandle, switchPayload);
    
    // Delay inter connection polling - when connected, update every 1/4 second
    delay(250);
  }
  else
  {
    // Not connected to a peer device
    debugSerial.println("Not connected to a peer device");

    // Delay inter connection polling - when not connected, check for new connections ever 1 second
    delay(1000);
  }
}