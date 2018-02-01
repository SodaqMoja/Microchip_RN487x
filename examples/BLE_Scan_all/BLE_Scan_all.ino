/* BLE_Scan_all
 * Based on work by M16946 (2017/01/13) for Microchip. 
 * Modified by Brian Schmalz of Schmalz Haus LLC (brian@schmalzhaus.com) also for Microchip, Dec 2017
 */

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include <Arduino.h>
#include <RN487x_BLE.h>

#define debugSerial Serial
#define bleSerial Serial1

#define SERIAL_TIMEOUT  10000

void setup()
{
  while ((!debugSerial) && (millis() < SERIAL_TIMEOUT)) ;
  
  debugSerial.begin(115200) ;

  // Set the optional debug stream
  rn487xBle.setDiag(debugSerial) ;
  // Initialize the BLE hardware
  rn487xBle.hwInit() ;
  // Open the communication pipe with the BLE module
  bleSerial.begin(rn487xBle.getDefaultBaudRate()) ;
  // Assign the BLE serial port to the BLE library
  rn487xBle.initBleStream(&bleSerial) ;
  // Finalize the init. process
  if (rn487xBle.swInit())
  {
    debugSerial.println("Init. procedure done!") ;
  }
  else
  {
    debugSerial.println("Init. procedure failed!") ;
    while(1) ;
  }
  
  // >> Configuring the BLE
  // First enter in command/configuration mode
  rn487xBle.enterCommandMode() ;
  // Remove GATT services
  rn487xBle.setDefaultServices(NO_SERVICE) ;
  // Set passive scan and does not filter out duplicate scan results
  rn487xBle.setSupportedFeatures(PASSIVE_SCAN_BMP | NO_DUPLICATE_SCAN_BMP) ;
  // Take into account the settings by issuing a reboot
  rn487xBle.reboot() ;
  rn487xBle.enterCommandMode() ;
  // Halt advertisement
  rn487xBle.stopAdvertising() ;

  // Start scanning
  rn487xBle.startScanning() ;

  debugSerial.println("Starter Kit as an Observer without filtering the duplicate scan results") ;
  debugSerial.println("=======================================================================") ;
}

void loop()
{
  // Display the result of the scanning
  if (bleSerial.available())
  {
    debugSerial.print((char)bleSerial.read()) ;
  }
}
