#include "effects.h"
#include "utils.h"

/******************************************************************************
 * EffectsManager
 ******************************************************************************/
EffectsManager::EffectsManager(uint32_t num_of_effects, uint32_t refresh_rate,
                               BaseType_t core)
    : ITaskManager(refresh_rate, core), m_effects() {
    m_effects.resize(num_of_effects);
}

EffectsManager::~EffectsManager() {
    for (int i = 0; i < m_effects.count(); i++) {
        EffectBase *ptr = m_effects[i];
        if (ptr != nullptr) {
            delete ptr;
        }
    }
}

void EffectsManager::AddEffect(EffectBase *effect) { m_effects.add(effect); }

void EffectsManager::setup() {}
void EffectsManager::update() {
    for (int i = 0; i < this->m_effects.count(); i++) {
        EffectBase *effect = this->m_effects[i];
        effect->update();
    }
}
void EffectsManager::cleanup() {}

/******************************************************************************
 * Effectmanager
 ******************************************************************************/
EffectManager::EffectManager(uint32_t refresh_rate, BaseType_t core)
    : EffectsManager(1, refresh_rate, core), m_active(0) {}

EffectManager::EffectManager(EffectBase *effect, uint32_t refresh_rate,
                             BaseType_t core)
    : EffectsManager(1, refresh_rate, core), m_active(0) {
    this->AddEffect(effect);
}

void EffectManager::update(void) {
    if (this->m_active < this->m_effects.count()) {
        this->m_effects[this->m_active]->update();
    }
}

void EffectManager::setActive(uint32_t index) { this->m_active = index; }

/******************************************************************************
 * EffectBase
 ******************************************************************************/
void EffectBase::update(void) {
    this->m_pixels_ptr->updatePixels(this->m_leds);
}

/******************************************************************************
 * HeatBase
 ******************************************************************************/
void HeatBase::update(void) {
    for (int i = 0; i < m_heat.count(); i++) {
        uint32_t &value = m_heat[i];
        value = (value < m_min_heat)   ? m_min_heat
                : (value > m_max_heat) ? m_max_heat
                                       : value;
        m_palette.interp(value, m_leds[i]);
        m_palette.correct_colors(m_leds[i]);
    }
    EffectBase::update();
}

/******************************************************************************
 * Sparks
 ******************************************************************************/
void Sparks::update(void) {
    this->m_cold_down_val += this->m_cold_down;
    if (this->m_cold_down_val > 1 || this->m_cold_down_val < -1) {
        int32_t val = (int32_t)(this->m_cold_down_val);
        for (int i = 0; i < this->m_heat.count(); i++) {
            uint32_t &value = this->m_heat[i];
            int64_t new_val = (int64_t)value + (int64_t)val;
            if (new_val < 0) {
                value = 0;
            } else {
                value = (uint32_t)new_val;
            }
        }
        this->m_cold_down_val -= val;
    }
    this->m_sparks_val += this->m_num_of_sparks;
    if (this->m_sparks_val > 1) {
        uint32_t count = (uint32_t)(this->m_sparks_val);
        for (int i = 0; i < count; i++) {
            size_t index = rand() % this->m_heat.count();
            this->m_heat[index] = this->m_spark_value;
        }
        this->m_sparks_val -= count;
    }
    HeatBase::update();
}

/******************************************************************************
 * Roll
 ******************************************************************************/
void shift(ArrayList<uint32_t> &data, int32_t count, uint32_t new_value) {
    if (count > 0) {
        int start = data.count() - 1;
        int end = count;
        for (int i = start; i >= end; i--) {
            data[i] = data[i - count];
        }
        for (int i = 0; i < end; i++) {
            data[i] = new_value;
        }
    }
    if (count < 0) {
        int start = 0;
        int end = data.count() - count;
        for (int i = start; i < end; i++) {
            data[i] = data[i + count];
        }
        for (int i = end; i < data.count(); i++) {
            data[i] = new_value;
        }
    }
}

void Roll::update(void) {
    this->m_heat_count += this->m_heat_speed;
    this->m_roll_count += this->m_roll_speed;
    if (this->m_heat_count > this->m_max_heat) {
        this->m_heat_count = this->m_min_heat;
    } else if (this->m_heat_count < this->m_min_heat) {
        this->m_heat_count = this->m_max_heat;
    }

    if (this->m_roll_count >= 1.0f || this->m_roll_count <= -1.0f) {
        int32_t count = (int32_t)this->m_roll_count;
        shift(this->m_heat, count, (uint32_t)this->m_heat_count);
        this->m_roll_count -= count;
    }
    HeatBase::update();
}

/******************************************************************************
 * Sparks
 ******************************************************************************/

void Pulses::update(void) {
    if (this->m_direction == 0) {
        this->m_current = 0;
        this->m_direction = 1;
    }

    this->m_current += this->m_direction * this->m_speed;
    if (this->m_current > (float)(this->m_max_heat)) {
        this->m_current = this->m_max_heat;
        this->m_direction = -1;
    } else if (this->m_current < (float)(this->m_min_heat)) {
        this->m_current = this->m_min_heat;
        this->m_direction = 1;
    }
    for (int i = 0; i < this->m_heat.count(); i++) {
        this->m_heat[i] = this->m_current;
    }

    HeatBase::update();
}
