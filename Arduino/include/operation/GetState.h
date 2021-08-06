
#include "Utilities.h"

bool BLE_OP_GetState(
    JsonVariant data_in,
    JsonVariant data_out,
    JsonObject meta)
{
    data_out["b"] = manager.getBrightness();
    data_out["e"] = getActiveEffect();
    JsonObject ec = data_out.createNestedObject("ec");
    getActiveEffectConfiguration(ec);

    return true;
};