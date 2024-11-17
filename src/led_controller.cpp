#include "led_controller.h"
#include <stream.h>

/******************************************************************************
 * LedStrip
 ******************************************************************************/
LedStrip::LedStrip(uint16_t n, int16_t pin, neoPixelType type)
    : Adafruit_NeoPixel(n, pin, type), m_type(type), m_buffer(nullptr),
      m_mutex(), m_segments(0) {
    m_buffer = (uint8_t *)calloc(sizeof(uint8_t), this->numBytes);
}

LedStrip::~LedStrip() {
    if (m_buffer) {
        free(m_buffer);
    }
    for (uint32_t i = 0; i < m_segments.count(); i++) {
        ILedStrip *ptr = m_segments[i];
        if (ptr != nullptr) {
            delete ptr;
        }
    }
}

ILedStrip *LedStrip::GetSegment(uint32_t start, uint32_t end) {
    ILedStrip *ptr = new LedStripSegment(this, start, end);
    m_segments.add(ptr);
    return ptr;
}

void LedStrip::ReleaseSegment(ILedStrip *segment) {
    m_segments.remove(segment);
    delete segment;
}

void LedStrip::updateSegment(const LedsList &leds, size_t start, size_t end) {
    LockGuard lock(this->m_mutex);

    if (this->numLEDs < start) {
        return;
    }
    if (this->numLEDs < end) {
        end = this->numLEDs;
    }
    uint8_t bytes = hasWhite() ? 4 : 3;
    for (uint16_t i = start; i < end; i++) {
        const ::Color &color = leds[i - start];
        uint8_t *p = &(this->m_buffer[i * bytes]);
        p[this->rOffset] = color.R();
        p[this->gOffset] = color.G();
        p[this->bOffset] = color.B();
    }
}

void LedStrip::updatePixels(const LedsList &leds) {
    updateSegment(leds, 0, leds.count());
}

void LedStrip::updatePixel(uint16_t index, ::Color color) {
    LockGuard lock(this->m_mutex);
    uint8_t bytes = this->hasWhite() ? 4 : 3;
    uint8_t *p = &(this->m_buffer[index * bytes]);
    p[this->rOffset] = color.R();
    p[this->gOffset] = color.G();
    p[this->bOffset] = color.B();
}

void LedStrip::draw(void) {
    {
        LockGuard lock(this->m_mutex);
        memcpy(this->pixels, this->m_buffer, this->numBytes);
    }
    this->show();
}

/******************************************************************************
 * LedStripSegment
 ******************************************************************************/
void LedStripSegment::updateSegment(const LedsList &leds, size_t start,
                                    size_t end) {
    if (this->m_leds.count() < start) {
        return;
    }
    if (this->m_leds.count() < end) {
        end = this->m_leds.count();
    }
    for (uint16_t i = start; i < end; i++) {
        this->m_leds[i] = leds[i];
    }
    if (this->m_led_strip_ptr != nullptr) {
        this->m_led_strip_ptr->updateSegment(this->m_leds, this->m_start,
                                             this->m_end);
    }
}

void LedStripSegment::updatePixels(const LedsList &leds) {
    updateSegment(leds, 0, leds.count());
}
void LedStripSegment::updatePixel(uint16_t index, ::Color color) {
    this->m_led_strip_ptr->updatePixel(this->m_start + index, color);
}

/******************************************************************************
 * LedStripManager
 ******************************************************************************/
LedStripManager::LedStripManager(uint16_t n, int16_t pin, neoPixelType type,
                                 uint32_t refresh_rate, BaseType_t core)
    : LedStrip(n, pin, type), ITaskManager(refresh_rate, core) {}

void LedStripManager::setup(void) {
    this->begin();
    this->clear();
}

void LedStripManager::update(void) { this->draw(); }

void LedStripManager::cleanup(void) { this->clear(); }

/******************************************************************************
 * LedStripsmanager
 ******************************************************************************/
LedStripsManager::LedStripsManager(int max_strips, int refresh_rate,
                                   BaseType_t core)
    : m_led_strips(), ITaskManager(refresh_rate, core) {
    m_led_strips.resize(max_strips);
}

LedStripsManager::~LedStripsManager() {
    for (int i = 0; i < this->m_led_strips.count(); i++) {
        LedStrip *ptr = this->m_led_strips[i];
        if (ptr != nullptr) {
            delete ptr;
        }
    }
}

uint16_t LedStripsManager::addLedStrip(uint16_t number_of_leds, int16_t pin,
                                       neoPixelType type) {
    LedStrip *strip_ptr = new LedStrip(number_of_leds, pin, type);
    if (this->m_led_strips.add(strip_ptr) == false) {
        delete strip_ptr;
    } else {
        return this->m_led_strips.count() - 1;
    }
    return 0xffff;
}

LedStrip *LedStripsManager::getLedStrip(uint16_t id) {
    if (id < this->m_led_strips.count()) {
        return this->m_led_strips[id];
    }
    return nullptr;
}

void LedStripsManager::setup(void) {
    for (int i = 0; i < this->m_led_strips.count(); i++) {
        LedStrip *ptr = this->m_led_strips[i];
        if (ptr != nullptr) {
            ptr->begin();
            ptr->clear();
        }
    }
}

void LedStripsManager::update(void) {
    for (int i = 0; i < this->m_led_strips.count(); i++) {
        LedStrip *ptr = this->m_led_strips[i];
        if (ptr != nullptr) {
            ptr->draw();
        }
    }
}

void LedStripsManager::cleanup(void) {
    for (int i = 0; i < this->m_led_strips.count(); i++) {
        LedStrip *ptr = this->m_led_strips[i];
        if (ptr != nullptr) {
            ptr->clear();
        }
    }
}
