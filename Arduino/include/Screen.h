struct Screen
{
    static void show()
    {
        EVERY_N_MILLISECONDS(400)
        {
            Serial.printf("\rLEDs at %d fps; Audio Sampling at %d fps", sFPS, gFPS);
        }
    }
};
