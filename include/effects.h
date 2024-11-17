#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include "led_controller.h"
#include "palette.h"
#include "utils.h"

class EffectBase {
    // Simulator Base Class
  public:
    EffectBase(ILedStrip *led_strip, const Palette &palette)
        : m_pixels_ptr(led_strip), m_palette(palette),
          m_leds(m_pixels_ptr->getNumPixels()) {}

    // Update simulation
    virtual void update(void);

  private:
    ILedStrip *m_pixels_ptr;

  protected:
    Palette m_palette;
    LedsList m_leds;
};

class EffectsManager : public ITaskManager {
  public:
    EffectsManager(uint32_t num_of_effects, uint32_t refresh_rate = 60,
                   BaseType_t core = 1);
    ~EffectsManager();

    void AddEffect(EffectBase *effect);

    void setup(void);
    void update(void);
    void cleanup(void);

  protected:
    ArrayList<EffectBase *> m_effects;
};

class EffectManager : public EffectsManager {
  public:
    EffectManager(EffectBase *effect, uint32_t refresh_rate = 60,
                  BaseType_t core = 1);
};

class HeatBase : public EffectBase {
  public:
    HeatBase(ILedStrip *pixels, const Palette &palette)
        : EffectBase(pixels, palette), m_heat(m_leds.count()) {}
    // Max heat value
    void setMinHeat(uint32_t val) { m_min_heat = val; }
    // Min heat value
    void setMaxHeat(uint32_t val) { m_max_heat = val; }

    void update(void);

  protected:
    uint32_t m_min_heat = 0;
    uint32_t m_max_heat = 0;
    ArrayList<uint32_t> m_heat;
};

class Sparks : public HeatBase {
  public:
    using HeatBase::HeatBase;
    // How much to cold down per tick
    void setColdDown(float val) { m_cold_down = val; }
    // Number of Sparks to ignite per tick
    void setNumOfSparks(float val) { m_num_of_sparks = val; }
    // Initial spark value
    void setSparkValue(float val) { m_spark_value = val; }

    void update(void);

  protected:
    float m_cold_down = 0;
    float m_num_of_sparks = 0;
    float m_spark_value = 0;

    float m_cold_down_val = 0;
    float m_sparks_val = 0;
};

class Roll : public HeatBase {
  public:
    using HeatBase::HeatBase;

    void update(void);

    // Set how fast change the color
    void setSpeed(float value) { m_heat_speed = value; }

    // Set how fast move shift the color
    void setRollSpeed(float value) { m_roll_speed = value; }

  protected:
    float m_heat_speed = 0;
    float m_roll_speed = 0;

    float m_heat_count = 0;
    float m_roll_count = 0;
};

class Pulses : public HeatBase {
  public:
    using HeatBase::HeatBase;

    void update(void);

    void setSpeed(float value) { m_speed = value; }

  protected:
    float m_speed = 0;
    float m_current = 0;
    float m_direction = 0;
};

#endif