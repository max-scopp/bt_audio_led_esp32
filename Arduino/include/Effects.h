
#include "effects/debug.h"
#include "effects/sound_debug.h"
#include "effects/sound_vu_debug.h"
#include "effects/rainbow.h"

// #include "effects/bounce.h"
// #include "effects/comet.h"
// #include "effects/fire.h"
#include "effects/marquee.h"
// #include "effects/twinkle.h"

using namespace std;

std::map<string, StripEffect *> effects = {
    {"debug", new DebugEffect()},
    {"debug:sound", new SoundDebugEffect()},
    {"debug:sound-vu", new SoundVUDebugEffect()},
    {"rainbow", new RainbowEffect()},
    {"marquee", new MarqueeEffect()},
    // {"bouncing-ball", new BouncingBallEffect()},
    // {"comet", new CometEffect()},
    // {"fire", new FireEffect()},
    // {"twinkle", new TwinkleEffect()},
};

string getActiveEffect()
{
    for (auto const &x : effects)
    {
        if (x.second == manager.g_EffectPointer)
        {
            return x.first;
        }
    }

    return jsonNull();
}

void getActiveEffectConfiguration(JsonObject writeTo)
{
    if (manager.g_EffectPointer)
    {
        manager.g_EffectPointer->writeConfiguration(writeTo);
    }
}

vector<string> getEffects()
{
    vector<string> l;

    for (auto const &x : effects)
    {
        l.push_back(x.first);
    }

    return l;
}

bool setEffect(string effectName)
{
    StripEffect *effectInstance = nullptr;

    for (auto const &x : effects)
    {
        Serial.printf("%s === %s", x.first.c_str(), effectName.c_str());
        if (x.first == effectName)
        {
            effectInstance = x.second;
        }
    }

    Serial.printf("Found effect located at %p \n", effectInstance);

    if (effectInstance)
    {
        manager.g_EffectPointer = effectInstance;
        return true;
    }

    return false;
}