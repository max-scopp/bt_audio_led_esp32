#pragma once

#include <ArduinoJson.h>

#include "Utilities.h"

using namespace std;

StaticJsonDocument<255> ping(StaticJsonDocument<255> p, StaticJsonDocument<255> doc)
{
    doc["pong"] = millis();
    return doc;
};