#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void initBluetooth(String name)
{
  SerialBT.begin("ESP32test"); //Name des ESP32
}

void handleBluetooth()
{
  if (Serial.available())
  {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available())
  {
    Serial.write(SerialBT.read());
  }
  delay(25);
}
