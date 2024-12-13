/* BLEWifiConfigService.h

Definition for the Wifi Config Service.

*/

#ifndef _BLE_WIFICONFIGSERVICE_H_
#define _BLE_WIFICONFIGSERVICE_H_

#include <AppBLEService.h>

// uncomment this to print debug info on Serial.
#define BLEWIFICONFIG_DEBUG


// typedef for callback function to update application wifi config.
// this will be called when the connection closes IF a valid pin code was provided AND the SSID and/or the Key were changed.
// the bool argument indicate if the corresponding string has been updated or not.
typedef void (*WifiConfigUpdateCallback)(bool ssid_updated, String ssid, bool key_updated, String key);

// the length of the pin code excluding the trailing null.
#define BLE_PINCODE_MAXLENGTH 20

// The supported SSID and Key string max length.
#define BLE_WIFISSID_MAXLEN 48
#define BLE_WIFIKEY_MAXLEN 48


class BLEWifiConfigService : AppBLEService {
public:
    BLEWifiConfigService(const char *pinCode, WifiConfigUpdateCallback callback) {
        pin_code = pinCode;
        wifiUpdateCallback = callback;
        resetState();
    }
    virtual void setup();
    virtual void update();
    virtual void onConnected(BLEDevice central);
    virtual void onDisconnected(BLEDevice central);

    // TRUE if a client is currently connected.
    bool connected;
    // true if a valid PIN was received
    bool pin_valid;
    // indicates that the corresponding field has been updated during the last connection.
    bool ssid_updated;
    // indicates that the corresponding field has been updated during the last connection.
    bool key_updated;
    // this is the expected (correct) PIN code value we are waiting for...
    const char *pin_code;
    // the app should set this to call function when wifi info updates...
    WifiConfigUpdateCallback wifiUpdateCallback;

private:
    void resetState();

};


#endif