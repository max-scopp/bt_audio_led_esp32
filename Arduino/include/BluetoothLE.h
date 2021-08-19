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

struct BluetoothLE
{
    static void sendJson(DynamicJsonDocument doc)
    {
        string output;
        serializeJson(doc, output);
        setValue(output);
    }

    template <typename T>
    static void setValue(T value)
    {
        if (deviceConnected)
        {

#if DEBUG_BLE_COMM
            Serial.println("OUT ");
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

    static void sendMessage(string message, bool isProblem = false)
    {
        StaticJsonDocument<255> responseDoc;

        try
        {
            short operation = -1;

            responseDoc[0] = operation;

            JsonObject responsePayload = responseDoc.createNestedObject();
            JsonObject responseMeta = responseDoc.createNestedObject();

            BLE_OP_Message(message, responsePayload, responseMeta);

            if (isProblem)
            {
                responseMeta["__EX__"] = true;
            }

            sendJson(responseDoc);
        }
        catch (const exception &e)
        {
            Serial.printf(e.what());
            Serial.println();
        }
    }

    static void setup()
    {
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
                string value = pCharacteristic->getValue();

#if DEBUG_BLE_COMM
                Serial.print("GOT ");
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
                StaticJsonDocument<255> responseDoc;

                try
                {
                    short operation = cmds[0].as<short>();

                    responseDoc[0] = operation;

                    JsonObject responsePayload = responseDoc.createNestedObject();
                    JsonObject responseMeta = cmds[2].as<JsonObject>();

                    JsonVariant requestPayload = cmds[1];

                    bool responds = BLE_handleOperation(operation, requestPayload, responsePayload, responseMeta);

                    if (responds)
                    {
                        BluetoothLE::sendJson(responseDoc);
                    }
                }
                catch (const exception &e)
                {
                    BluetoothLE::sendMessage(e.what(), true);
                }
            }
        };

        // Create the BLE Device
        BLEDevice::init(DEFAULT_DEVICE_NAME);

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

    static void loop()
    {
        delay(1000);
    }
};
