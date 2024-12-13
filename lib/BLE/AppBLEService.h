/* Class for a BLE Service. */

#ifndef _APP_BLE_SERVICE_H_
#define _APP_BLE_SERVICE_H_

#include <Arduino.h>
#include <ArduinoBLE.h>

class AppBLEService {
public:
    AppBLEService()
    { 
        pNextService = nullptr;
    }
    // these are pure virtual and must be defined in subclasses
    virtual void setup() = 0;
    virtual void update() = 0;
    virtual void onConnected(BLEDevice central) = 0;
    virtual void onDisconnected(BLEDevice central) = 0;
public:
    AppBLEService *pNextService;
};

#endif