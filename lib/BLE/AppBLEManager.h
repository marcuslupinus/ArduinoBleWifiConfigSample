/* AppBLEManager.h

The AppBLEManager class is a management class for one or more BLE services supported
by the Arduino.

The BLE services are created as separate classes that inherit from AppBLEService class.
Each BLE service class for the application is added to the manager class who takes
care of managing them.

Only one insctance of this manager class is created and used throughout the application.
*/

#ifndef _APPBLEMANAGER_H_
#define _APPBLEMANAGER_H_

#include <AppBLEService.h>

// comment this out to remove any debug trace.
#define BLEMANAGER_DEBUG

// the BLE advertisement interval in milliseconds.
#define BLE_ADVERTISE_INTERVAL 1000


class AppBLEManager {
public:
    AppBLEManager() {
        pFirstService = nullptr;
    }
    // add the service to the list of managed services.  This must be called before 'setup()'.
    void addService(AppBLEService *service)
    {
        service->pNextService = pFirstService;
        pFirstService = service;
    }

    // localDeviceName will be the broadcasted Bluetooth device name.
    void setup(const char *localDeviceName);
    // call this from the main loop().
    void update();

    AppBLEService *pFirstService;
public:
    void onConnected(BLEDevice central);
    void onDisconnected(BLEDevice central);
};

#endif