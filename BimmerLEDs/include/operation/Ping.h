
bool BLE_OP_Ping(
    JsonVariant data_in,
    JsonVariant data_out,
    JsonObject meta)
{
    data_out["got"] = data_in;
    data_out["millis"] = millis();

    return true;
};