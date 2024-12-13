/* BLEWifiConfigService.cpp */

#include <BLEWifiConfigService.h>

// Dont' change the following UUIDs.

// The BLE device exposes 2 services: a) an authentication service and b) a wifin configuration service.

// The authentication service's purpose is to make sure the client has the rights to update the wifi config.
// It exposes a PIN code field (6 characters) that the client must provide and must match what is defined here
// in the code.
// If the PIN received is ok, the PINCodeOk characteristic will show a value of 1 otherwise 0.

BLEService myAuthenticationService("c38f5290-0e9c-4677-adab-9dcac2c882e8");

// the value (pin code) that has to be supplied by the client.
BLEStringCharacteristic pinCodeCharacteristic("6de4bf05-75d5-4a6e-815b-52c6c47655bb", BLEWrite | BLERead, BLE_PINCODE_MAXLENGTH + 1);
// a boolean indicating that the pin code supplied by the client is good or bad
BLEBoolCharacteristic pinCodeOkCharacteristic("cb411e39-1ffe-4520-bead-88d13f77af0e", BLERead | BLENotify);

// The Wifi config service exposes two string characteristics to allow the client to set the SSID and key for the wifi network access.

BLEService myWifiConfigService("897f234d-0704-4dc1-9d46-b42b1d59c57a");

BLEStringCharacteristic wifiSSIDCharacteristic("61b68b3e-9c8c-48ac-9897-95ce7530e707", BLEWrite | BLERead | BLENotify, BLE_WIFISSID_MAXLEN + 1);
BLEStringCharacteristic wifiKeyCharacteristic("ec75a9b3-fe6e-49b2-9665-979efd5a3e4a", BLEWrite | BLERead | BLENotify, BLE_WIFIKEY_MAXLEN + 1);


void BLEWifiConfigService::setup()
{
    pinCodeCharacteristic.writeValue("\0");
    pinCodeOkCharacteristic.writeValue(0);
    myAuthenticationService.addCharacteristic(pinCodeCharacteristic);
    myAuthenticationService.addCharacteristic(pinCodeOkCharacteristic);
    BLE.addService(myAuthenticationService);

    wifiSSIDCharacteristic.writeValue("");
    myWifiConfigService.addCharacteristic(wifiSSIDCharacteristic);
    wifiKeyCharacteristic.writeValue("");
    myWifiConfigService.addCharacteristic(wifiKeyCharacteristic);
    BLE.addService(myWifiConfigService);
}

void BLEWifiConfigService::update()
{
    if (connected)
    {
        if (pinCodeCharacteristic.written())
        {
            String v = pinCodeCharacteristic.value();
            pin_valid = false;
            if (v.length() == strlen(pin_code)) {
                pin_valid = v.equals(pin_code);
            }
            pinCodeOkCharacteristic.writeValue( pin_valid ? 1 : 0);
#ifdef BLEWIFICONFIG_DEBUG
            Serial.print(F("BLEWifiConfigService: Pin code received: '"));
            Serial.print(v);
            Serial.print("' valid=");
            Serial.println(pin_valid);
#endif
        }
        if (!ssid_updated) {
            if (wifiSSIDCharacteristic.written())
                ssid_updated = true;
        }
        if (!key_updated) {
            if (wifiKeyCharacteristic.written())
                key_updated = true;
        }
    }

}

void BLEWifiConfigService::onConnected(BLEDevice central)
{
#ifdef BLEWIFICONFIG_DEBUG
    Serial.println("BLEWifiConfigService: connected");
#endif
    resetState();
    connected = true;
}

void BLEWifiConfigService::onDisconnected(BLEDevice central)
{
    #ifdef BLEWIFICONFIG_DEBUG
    Serial.println("BLEWifiConfigService: disconnected");
#endif
    connected = false;
    if (wifiUpdateCallback != nullptr)
    {
        if (pin_valid & (ssid_updated || key_updated))
        {
            wifiUpdateCallback(
                ssid_updated, wifiSSIDCharacteristic.value(),
                key_updated, wifiKeyCharacteristic.value()
            );
        }
    }
}


void BLEWifiConfigService::resetState()
{
    connected = false;
    pin_valid = false;
    ssid_updated = false;
    key_updated = false;

}

