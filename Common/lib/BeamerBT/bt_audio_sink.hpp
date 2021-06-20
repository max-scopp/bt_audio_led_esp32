#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

void initBluetooth(String name, int PIN_OUT)
{
  SerialBT.begin(name); //Name des ESP32
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
}
