#pragma once

#include <ArduinoJson.h>

#include "Utilities.h"

#include "operation/Unknown.h"
#include "operation/Message.h"

#include "operation/Ping.h"
#include "operation/GetAbout.h"
#include "operation/GetState.h"

#include "operation/ConfigureStrips.h"
#include "operation/ConfigureBluetooth.h"

#include "operation/GetEffects.h"
#include "operation/SetEffect.h"
#include "operation/SetBrightness.h"
#include "operation/ConfigureEffect.h"

using namespace std;

bool BLE_handleOperation(
    short operation,
    JsonVariant data_in,
    JsonVariant data_out,
    JsonObject meta)
{
    switch (operation)
    {
    case 0:
        return BLE_OP_Ping(data_in, data_out, meta);
    case 1:
        return BLE_OP_GetAbout(data_in, data_out, meta);
    case 2:
        return BLE_OP_GetState(data_in, data_out, meta);
    case 3:
        return BLE_OP_ConfigureStrips(data_in, data_out, meta);
    case 4:
        return BLE_OP_ConfigureBluetooth(data_in, data_out, meta);
    case 5:
        return BLE_OP_SetBrightness(data_in, data_out, meta);
    case 6:
        return BLE_OP_GetEffects(data_in, data_out, meta);
    case 7:
        return BLE_OP_SetEffect(data_in, data_out, meta);
    case 8:
        return BLE_OP_ConfigureEffect(data_in, data_out, meta);
    default:
        return BLE_OP_Unknown(data_in, data_out, meta);
    }
};