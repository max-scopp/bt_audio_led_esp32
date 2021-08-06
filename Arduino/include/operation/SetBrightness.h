

bool BLE_OP_SetBrightness(
    JsonVariant data_in,
    JsonVariant data_out,
    JsonObject meta)
{
    float brightness = data_in.as<float>();

    manager.setBrightness(brightness);

    return false;
};