/* AppBLEManager.cpp */

#include <AppBLEManager.h>

// The UUID for our main service.  This can be used by BLE clients to filter out
// the available device list to only those that are exposing this BLEWifiConfigMgr behaviour.
// NOTE: The android app doesn't care about the main service uuid, you can make it anythin you want.
//       On the other hand, the services and characteristics defined further down this file are important
//       to allow the Android app to recognize the Arduino and communicate with it.

static const char bleWifiConfigServiceUUID[] = "19B10000-E8F2-537E-4F6C-D104768A1214";

BLEService _myMainService(bleWifiConfigServiceUUID);

static AppBLEManager *_managerInstance = nullptr;

static void _bleConnectedHandler(BLEDevice central)
{
    _managerInstance->onConnected(central);
}
static void _bleDisconnectedHandler(BLEDevice central)
{
    _managerInstance->onDisconnected(central);
}

void AppBLEManager::setup(const char *localDeviceName)
{
    _managerInstance = this;
    if (!BLE.begin())
    {
#ifdef BLEMANAGER_DEBUG
        Serial.println("starting Bluetooth Low Energy module failed!");
#endif
        while (1) ;
    }

    BLE.setLocalName(localDeviceName);
    BLE.setAdvertisedService(_myMainService);

    AppBLEService * ps = pFirstService;
    while (ps != nullptr) {
        ps->setup();
        ps = ps->pNextService;
    }

    // register handles for connect/disconnect events
    BLE.setEventHandler(BLEConnected, _bleConnectedHandler);
    BLE.setEventHandler(BLEDisconnected, _bleDisconnectedHandler);

    // 1 unit = 0.625ms
    float adunits = BLE_ADVERTISE_INTERVAL / 0.625;
    BLE.setAdvertisingInterval((uint16_t)adunits);
    BLE.advertise(); // start advertising
}


// this must be called periodically from the main loop.
void AppBLEManager::update()
{
    BLE.poll();
    AppBLEService * ps = pFirstService;
    while (ps != nullptr) {
        ps->update();
        ps = ps->pNextService;
    }
}

void AppBLEManager::onConnected(BLEDevice central)
{
#ifdef BLEMANAGER_DEBUG
    Serial.print("BLEManager: Connected");
    Serial.println(central.address());
#endif
    AppBLEService * ps = pFirstService;
    while (ps != nullptr) {
        ps->onConnected(central);
        ps = ps->pNextService;
    }
}

void AppBLEManager::onDisconnected(BLEDevice central)
{
#ifdef BLEMANAGER_DEBUG
    Serial.print("Disconnected event, central: ");
    Serial.println(central.address());
#endif
    AppBLEService * ps = pFirstService;
    while (ps != nullptr) {
        ps->onDisconnected(central);
        ps = ps->pNextService;
    }
}
