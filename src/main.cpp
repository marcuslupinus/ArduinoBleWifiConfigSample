/* Sample Arduino Sketch to illustrate how to use the BLE Wifi Configuration Management service.

To test, make sure the debug defines are present in AppBLEManager.h and BLEWifiConfigService.h.

This sketch was developped for an Arduino Uno Wifi R4 using the PlatformIO and Visual Studio Code development
environment on Windows.  It should be fairly easy to copy the code and make it work using the Arduino IDE as well.

It uses the ArduinoBLE library.
*/

#include <Arduino.h>
#include <EEPROM.h>

#include <AppBLEManager.h>
#include <BLEWifiConfigService.h>

const char bleLocaDevicelName[] = "BLE Wifi Cfg Sample";
const char bleWifiConfigPINCode[] = "1234";

// the prototype of the callback defined further down in this file.
static void _WifiConfigUpdateHandler(bool ssid_updated, String ssid, bool key_updated, String key);

// create the instance for the BLE manager
AppBLEManager myBLEManager;
// create the bluetooth service for wifi configuration
BLEWifiConfigService _BLEWifiConfigService(bleWifiConfigPINCode, _WifiConfigUpdateHandler);


// For demonstration purpose, here is where we store the Wifi SSID and Wifi Password configured.
// The string sizes are defined in BLEWifiConfigService.h (and should not be changed)
char wifiSSID[BLE_WIFISSID_MAXLEN + 1];
char wifiPassword[BLE_WIFIKEY_MAXLEN + 1];

// prototypes for demo functions that will save and restore the Wifi config to/from non volatile memory.
static void WifiConfigRestore(char *ssidBuffer, int ssidBufferCapacity, char *passwordBuffer, int passwordBufferCapacity);
static void WifiConfigSave(char *ssid, int ssidMaxSize, char *password, int passwordMaxSize);

static void nonVolatileMemoryReadBytes( char *destination, int count, uint16_t startOffset );
static void nonVolatileMemoryWriteBytes( char *source, int bufferSize, uint16_t startOffset );
static void copy_string_limited(String source, char *destination, int maxchars);

void setup() {
    Serial.begin(9600);
    // AppBLEServices must be added to the AppBLEManager BEFORE calling setup().
    myBLEManager.addService((AppBLEService *)&_BLEWifiConfigService);
    myBLEManager.setup(bleLocaDevicelName);

    WifiConfigRestore(wifiSSID, BLE_WIFISSID_MAXLEN+1, wifiPassword, BLE_WIFIKEY_MAXLEN+1);

    Serial.print("Wifi SSID is: ");
    Serial.println(wifiSSID);
    Serial.print("Wifi Password is: ");
    Serial.println(wifiPassword);
}

void loop() {
    myBLEManager.update();

    delay(100);
}


// Sample version of the WifiConfigUpdateCallback.
// This will be called by the BLEWifiConfigService class when the bluetooth client
// disconnects from the Arduino AND if a valid pin was provided.
// Arguments are:
// ssid_updated is true if the BLE client sent us a new value for the Wifi SSID.
// key_updated is true if the BLE client sent us a new value for the Wifi Password.
// ssid = the new wifi ssid string (if ssid_updated is true).
// key = the new wifi password string (if key_updated is true).

static void _WifiConfigUpdateHandler(bool ssid_updated, String ssid, bool key_updated, String key)
{
    Serial.print("_WifiConfigUpdateHandler: ");
    if (ssid_updated)
    {
        Serial.print("ssid : '");
        Serial.print(ssid);
        Serial.print("' ");
        // copy the new value in our buffers defined earlier
        copy_string_limited(ssid, wifiSSID, BLE_WIFISSID_MAXLEN);
    }
    if (key_updated)
    {
        Serial.print("key : '");
        Serial.print(key);
        Serial.print("' ");
        // copy the new value in our buffers defined earlier
        copy_string_limited(key, wifiPassword, BLE_WIFIKEY_MAXLEN);
    }
    Serial.println("\n");
    if (ssid_updated || key_updated) {
        // save the new configuration in non volatile memory.
        WifiConfigSave(wifiSSID, BLE_WIFIKEY_MAXLEN+1, wifiPassword, BLE_WIFIKEY_MAXLEN+1);
    }
}


// NOTE: these functions are very basic - for demonstration only.  In real life, you would need to save some kind of
// known 'signature' into EEPROM the first time you save and when reading the EEPROM, you would check for the presence
// of the signature to make sure that the wifi config information that you are attempting to read has been written
// correctly at least once.  Here we don't go into all that hassle for simplicity purpose.  We just save and restore
// making sure that we dont go beyond the buffers capacity.

static void WifiConfigRestore(char *ssidBuffer, int ssidBufferCapacity, char *passwordBuffer, int passwordBufferCapacity)
{
    uint16_t eepromStartOffset = 0;

    // read the SSID
    nonVolatileMemoryReadBytes(ssidBuffer, ssidBufferCapacity, eepromStartOffset);
    ssidBuffer[ssidBufferCapacity-1] = '\0'; // safety

    // read the password
    nonVolatileMemoryReadBytes(passwordBuffer, passwordBufferCapacity, eepromStartOffset + ssidBufferCapacity);
    passwordBuffer[passwordBufferCapacity-1] = '\0';
}

static void WifiConfigSave(char *ssidBuffer, int ssidMaxSize, char *passwordBuffer, int passwordMaxSize)
{
    uint16_t eepromStartOffset = 0;

    // write the SSID
    nonVolatileMemoryWriteBytes(ssidBuffer, ssidMaxSize, eepromStartOffset);

    // write the password
    nonVolatileMemoryWriteBytes(passwordBuffer, passwordMaxSize, eepromStartOffset + ssidMaxSize);
}


static void nonVolatileMemoryReadBytes( char *destination, int count, uint16_t startOffset )
{
    for (int i=0; i<count; i++) {
        destination[i] = EEPROM.read( i+startOffset );
    }
}

static void nonVolatileMemoryWriteBytes( char *source, int bufferSize, uint16_t startOffset )
{
    for (int i=0; i<bufferSize; i++) {
        EEPROM.write(i+startOffset, source[i]);
    }
}

// Copy a string into a buffer taking care of not writing more than maxchars characters.
// NOTE: the destination buffer must have room for maxchars + 1 for the trailing nul.
static void copy_string_limited(String source, char *destination, int maxchars)
{
  int max = maxchars;
  if (source.length() < max)
    max = source.length();

  for (int i=0; i < max; i++) {
    destination[i] = source[i];
  }
  if (max > 0)
    destination[max] = '\0';
}

// *end*
