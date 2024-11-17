#include "effects.h"
#include <Arduino.h>

#include "config.h"

#define DEBOUNCE_TIME 60

LedStripManager led_strip(STRIP_LED_COUNT, STRIP_PIN, STRIP_TYPE,
                          STRIP_REFRESH_RATE, STRIP_TASK_CORE);
EffectManager effect_manager(EFFECTS_REFRESH_RATE, EFFECTS_TASK_CORE);

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

class DigitalInput {
  public:
    DigitalInput(uint8_t pin) : m_pin(pin), m_last_debounce_time(0), m_last_flickerable_state(0) {}
    void init(void);
    int read(void);
  private:
    uint8_t m_pin;
    uint64_t m_last_debounce_time;
    int m_last_flickerable_state;
    int m_last_stable_state;
};

void DigitalInput::init(void) {
    pinMode(m_pin, INPUT_PULLUP);
}

int DigitalInput::read(void) {
    int current_state = digitalRead(m_pin);

    if (current_state != m_last_flickerable_state) {
        m_last_debounce_time = millis();
        m_last_flickerable_state = current_state;
    }
    if ((millis() - m_last_debounce_time) > DEBOUNCE_TIME) {
        m_last_stable_state = current_state;
    }    
    return m_last_stable_state;
}

DigitalInput button(9);
void setup() {
    Serial.begin(9600);
    button.init();

    AddSparks(effect_manager, &led_strip);
    AddRoll(effect_manager, &led_strip);
    AddPulse(effect_manager, &led_strip);

    led_strip.start();
    effect_manager.start();
}


int current_index = 0;
uint64_t last_update_ms = 0;
int lastState = LOW;

void loop() {
    int currentState = button.read();
    if ((millis() - last_update_ms) > DEBOUNCE_TIME) {
        last_update_ms = millis();
        if(lastState == HIGH && currentState == LOW) {
            current_index = (current_index+1)%effect_manager.count();
            effect_manager.setActive(current_index);
        }
        lastState = currentState;
    }
}
