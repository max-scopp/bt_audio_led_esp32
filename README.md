# [Check out the app!](https://github.com/max-scopp/bimmer-led)

# LEDs

This _should_ mainly do the led displaying stuff.

Basically, you setup where and how your strips are done, then, use a somewhat simple
class to define your effect and with a few knobs, the effect magically works
on all areas of your car with the right orientation, length and shit.

## TODOs

- [ ] Make a basic PWA for remote control

1. [x] Define a strip with different sections
2. [ ] Somehow manage all strips dynamically
3. [-] Able to iterate over all strips
   - depends on 2.
4. [ ] Save/Load the strip configuration on flash
5. [ ] Allow dynamically changing the configuration (for app/initial setup)
6. [-] Extend base StripEffect class for additional knobs
   - [-] `bool uniqueSections = false`: Does every section require it's own calculation (e.g. fire effect)
   - [x] `bool fixAlignment = true`: If true, treat every effect as an "from left to right" and if the location prefers "right to left", the effect shall automatically be flipped

### TODOs Bluetooth (will likely be moved)

- [x] Basic BLE connectivity
- [-] Create an protocol for the app (WIP)
  - [ ] jsonb or stick with short json's?

_(probably not be done, but really cool if I finally understand shit)_

- [ ] Allow your phone to connect as Source and let the ESP32 act as Sink
  - Hopefully using BLE Audio instead of oldskool shit
- [ ] Internal communication of multiple ESP32s to let another ESP32 act as source again
  - Without large quality loss

### TODOs Audio (will likely be moved)

- [ ] Just get it fucking working
- [ ] Make dave's analyzer flexible for my use case
- [ ] Implement base led effect for the results of the analyzer (instead of using dave's matrix)
- [ ] Try to get I2S working instead of IQS for increased performance
