

bool BLE_OP_Message(
    std::string message,
    JsonObject data_out,
    JsonObject meta)
{
    data_out["o"] = message;
    meta["__MSG__"] = true;

    return true;
};