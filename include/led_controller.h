#ifndef __LED_CONTROLLER_H__
#define __LED_CONTROLLER_H__

#include <Adafruit_NeoPixel.h>
#include <memory>
#include <task.h>

#include "palette.h"
#include "utils.h"

typedef ArrayList<::Color> LedsList;

class ILedStrip {
  public:
    virtual void updateSegment(const LedsList &leds, size_t start,
                               size_t end) = 0;
    virtual void updatePixels(const LedsList &pixels) = 0;
    virtual void updatePixel(uint16_t index, ::Color color) = 0;
    virtual uint16_t getNumPixels(void) = 0;
    virtual void draw(void) {};
};

class LedStrip : public Adafruit_NeoPixel, public ILedStrip {
  private:
    neoPixelType m_type;
    uint8_t *m_buffer;
    Mutex m_mutex;
    ArrayList<ILedStrip *> m_segments;

  public:
    LedStrip(uint16_t n, int16_t pin, neoPixelType type);
    LedStrip(void) : Adafruit_NeoPixel() {}
    LedStrip(const LedStrip &other)
        : LedStrip(other.numLEDs, other.pin, other.m_type) {}
    ~LedStrip();

    uint8_t getRedOffset(void) { return this->rOffset; }
    uint8_t getGreenOffset(void) { return this->gOffset; }
    uint8_t getBlueOffset(void) { return this->bOffset; }
    uint8_t getWhiteOffset(void) { return this->wOffset; }
    uint16_t getNumPixels(void) { return Adafruit_NeoPixel::numPixels(); }
    bool hasWhite(void) { return this->wOffset != this->rOffset; }

    ILedStrip *GetSegment(uint32_t start, uint32_t stop);
    void ReleaseSegment(ILedStrip *segment);

    void updateSegment(const LedsList &leds, size_t start, size_t end);
    void updatePixels(const LedsList &leds);
    void updatePixel(uint16_t index, ::Color color);
    void draw(void);
};

class LedStripSegment : public ILedStrip {
  public:
    LedStripSegment(LedStrip *led_strip, size_t start, size_t end)
        : m_led_strip_ptr(led_strip), m_start(start), m_end(end),
          m_leds(end - start) {}

    void updateSegment(const LedsList &leds, size_t start, size_t end);
    void updatePixels(const LedsList &leds);
    void updatePixel(uint16_t index, ::Color color);
    uint16_t getNumPixels(void) { return m_end - m_start; }

  protected:
    LedStrip *m_led_strip_ptr;
    size_t m_start;
    size_t m_end;
    LedsList m_leds;
};

/******************************************************************************
 * LedStripManager
 ******************************************************************************/
class LedStripManager : public LedStrip, public ITaskManager {
  public:
    LedStripManager(uint16_t n, int16_t pin, neoPixelType type,
                    uint32_t refresh_rate = 60, BaseType_t core = 0);

    void setup(void);
    void update(void);
    void cleanup(void);
};

/******************************************************************************
 * LedStripsManager
 ******************************************************************************/
class LedStripsManager : public ITaskManager {
  public:
    LedStripsManager(int max_strips, int refresh_rate = 60,
                     BaseType_t core = 0);
    ~LedStripsManager();

    uint16_t addLedStrip(uint16_t number_of_leds, int16_t pind,
                         neoPixelType type);
    LedStrip *getLedStrip(uint16_t id);

    void setup(void);
    void update(void);
    void cleanup(void);

  private:
    ArrayList<LedStrip *> m_led_strips;
};

#endif