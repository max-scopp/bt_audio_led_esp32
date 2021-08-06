
bool BLE_OP_ConfigureEffect(
    JsonVariant data_in,
    JsonVariant data_out,
    JsonObject meta)
{
    manager.g_EffectPointer->updateConfiguration(data_in);
    return false;
};