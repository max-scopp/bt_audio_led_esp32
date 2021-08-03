

bool BLE_OP_SetEffect(
    JsonVariant data_in,
    JsonVariant data_out,
    JsonObject meta)
{
    string effectName = data_in.as<string>();
    setEffect(effectName);

    return true;
};