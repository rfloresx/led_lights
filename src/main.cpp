#include "effects.h"
#include <Arduino.h>

#include "config.h"

LedStripManager led_strip(STRIP_LED_COUNT, STRIP_PIN, STRIP_TYPE,
                          STRIP_REFRESH_RATE, STRIP_TASK_CORE);
EffectsManager effects(5, EFFECTS_REFRESH_RATE, EFFECTS_TASK_CORE);

void AddSparks(EffectsManager &manager, ILedStrip *segment) {
    Sparks *effect =
        new Sparks(segment, Palette(255, Color::WHITE, Color::WHITE));
    effect->setMinHeat(20);
    effect->setMaxHeat(255);
    effect->setColdDown(-2.5f);
    effect->setNumOfSparks(0.75f);
    effect->setSparkValue(255);
    manager.AddEffect(effect);
}

void AddRoll(EffectsManager &manager, ILedStrip *segment) {
    Roll *effect = new Roll(segment, RainbowPalette(8));
    effect->setMinHeat(0);
    effect->setMaxHeat(8);
    effect->setSpeed(0.1f);
    effect->setRollSpeed(0.1f);
    manager.AddEffect(effect);
}

void AddPulse(EffectsManager &manager, ILedStrip *segment) {
    Pulses *effect = new Pulses(segment, RainbowPalette(255));
    effect->setMinHeat(0);
    effect->setMaxHeat(255);
    effect->setSpeed(1);
    manager.AddEffect(effect);
}

void setup() {
    Serial.begin(9600);

    AddSparks(effects, &led_strip);
    // AddRoll(effects, &led_strip);
    // AddSparks(led_strip, effects. led_strip.);
    // AddPulse(led_strip, effects);

    led_strip.start();
    effects.start();
}

void loop() {}
