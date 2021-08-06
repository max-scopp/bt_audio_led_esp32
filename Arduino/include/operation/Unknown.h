
bool BLE_OP_Unknown(
    JsonVariant data_in,
    JsonVariant data_out,
    JsonObject meta)
{
    meta["__UNKNOWN__"] = true;

    return true;
};