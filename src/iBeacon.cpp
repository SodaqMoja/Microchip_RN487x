// iBeacon.cpp
// Author: M16946
// Date: 2017/01/13

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

#include "iBeacon.h"
#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------- Private section ----------------------------------------
iBeacon ibeacon ;

// *********************************************************************************
// Constructor
// *********************************************************************************
// Input : void
// Output: void
// *********************************************************************************
iBeacon::iBeacon()
{

}

// *********************************************************************************
// Destructor
// *********************************************************************************
// Input : void
// Output: void
// *********************************************************************************
iBeacon::~iBeacon()
{

}

// *********************************************************************************
// Start iBeacon
// *********************************************************************************
// Input : const char* uuid [16Bytes) identifies the beacon
//         uint16_t major [2B] identifying a subset of beacons within a large group
//         uint16_t minor [2B] identifying a specific beacon
//         uint8_t txPower [1B] level in 2's complement, indicating the signal
//         strength one meter from the device. This value must be calibrated for
//         each device by the user or manufacturer
// Output: void
// *********************************************************************************
void iBeacon::begin(const char* uuid, uint16_t major, uint16_t minor, uint8_t txPower)
{
  // iBeacon Prefix is constant which contains:
  //  - 0x004C = Apple, see https://www.bluetooth.org/en-us/specification/assigned-numbers/company-identifiers
  //  - 0x02 = iBeacon Type
  //  - 0x15 = iBeacon Length
  const char* prefix = "4C000215" ;
  uint8_t len = strlen(prefix) ;
  char c1[4] ;
  char c2[2] ;

  // Construct the iBeacon frame
  memset(data, 0, strlen(data)) ;
  // Add Prefix
  memcpy(data, prefix, len) ;
  // Add UUID
  memcpy(&data[len], uuid, strlen(uuid)) ;
  len += strlen(uuid) ;
  // Add Major
  sprintf(c1, "%04X", major) ;
  memcpy(&data[len], c1, strlen(c1)) ;
  len += strlen(c1) ;
  // Add Minor
  sprintf(c1, "%04X", minor) ;
  memcpy(&data[len], c1, strlen(c1)) ;
  len += strlen(c1) ;
  // Add Tx Power Measured
  sprintf(c2, "%02X", txPower) ;
  memcpy(&data[len], c2, strlen(c2)) ;

  // Start to transmit the beacon packet
  rn487xBle.startImmediateBeacon(AD_TYPE_MANUFACTURE_SPECIFIC_DATA, data) ;

}


