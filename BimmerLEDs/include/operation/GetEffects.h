

bool BLE_OP_GetEffects(
    JsonVariant data_in,
    JsonVariant data_out,
    JsonObject meta)
{
    JsonArray e = data_out.createNestedArray("e");

    vector<string> effectsList = getEffects();

    for (auto const &value : effectsList)
    {
        e.add(value);
    };

    return true;
};