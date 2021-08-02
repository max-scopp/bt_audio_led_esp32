#pragma once

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>

#include "Operations.h"
using namespace std;

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

template <typename T>
void setValue(T value)
{
    if (deviceConnected)
    {

#if DEBUG_BLE_COMM
        Serial.println("> ");
        for (int i = 0; i < value.length(); i++)
            Serial.print(value[i]);
#endif

        pCharacteristic->setValue(value);
        pCharacteristic->notify();
    }
    else
    {
        Serial.println("No device to sent to");
    }
}

void sendJson(StaticJsonDocument<255> doc)
{
    std::string output;
    serializeJson(doc, output);
    setValue(output);
}

class ServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
    {
        Serial.println("New conn!");
        deviceConnected = true;
        BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};

class ClientCallbacks : public BLECharacteristicCallbacks
{
public:
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();

#if DEBUG_BLE_COMM
        Serial.print("< ");
        for (int i = 0; i < value.length(); i++)
            Serial.print(value[i]);

        Serial.println();
#endif

        StaticJsonDocument<255> doc;
        DeserializationError error = deserializeJson(doc, value);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        // doc is ok
        handlePacket(doc);
    }

    void handlePacket(StaticJsonDocument<255> cmds)
    {
        StaticJsonDocument<255> doc;

        try
        {
            short operation = cmds[0].as<short>();

            switch (operation)
            {
            case 0:
            {
                doc = ping(cmds, doc);
                break;
            }
            }

            sendJson(doc);
        }
        catch (const std::exception &e)
        {
            Serial.printf(e.what());
        }
    }
};

class BluetoothService
{
public:
    void sendJson(DynamicJsonDocument doc)
    {
        sendJson(doc);
    }

    template <typename T>
    void setValue(T value)
    {
        setValue(value);
    }

    void setup()
    {
        // Create the BLE Device
        BLEDevice::init("Max's E46 BALE Service");

        // Create the BLE Server
        pServer = BLEDevice::createServer();
        pServer->setCallbacks(new ServerCallbacks());

        // Create the BLE Service
        BLEService *pService = pServer->createService(serviceUUID);

        // Create a BLE Characteristic
        pCharacteristic = pService->createCharacteristic(
            charUUID,
            BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE |
                BLECharacteristic::PROPERTY_NOTIFY |
                BLECharacteristic::PROPERTY_INDICATE);

        // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
        // Create a BLE Descriptor
        pCharacteristic->addDescriptor(new BLE2902());

        pCharacteristic->setCallbacks(new ClientCallbacks());

        // Start the service
        pService->start();

        // Start advertising
        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(serviceUUID);
        pAdvertising->setScanResponse(false);
        pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter
        BLEDevice::startAdvertising();
        pAdvertising->start(); // needed?
        Serial.println("Waiting a client connection to notify...");
    }

    void loop()
    {
        delay(1000);
    }
};

BluetoothService bluetooth;
