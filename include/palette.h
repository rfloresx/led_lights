#ifndef __PALETTE_H__
#define __PALETTE_H__

#include <malloc.h>
#include <stdint.h>
#include <string.h>

#include "utils.h"

class Color {
  public:
    static const Color BLACK;
    static const Color WHITE;
    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color ORANGE;
    static const Color YELLOW;
    static const Color LIME;
    static const Color AQUA;
    static const Color CYAN;
    static const Color OCEAN;
    static const Color VIOLET;
    static const Color MAGENTA;
    static const Color RASPBERRY;

    Color();
    Color(const Color &color);
    Color(uint32_t rgb);
    Color(uint8_t r, uint8_t g, uint8_t b);

    uint32_t Value() const;
    uint8_t R() const;
    uint8_t G() const;
    uint8_t B() const;
    uint8_t W() const;

    Color &operator=(const Color &other);
    Color operator*(const Color &other);
    Color operator/(const Color &other);
    Color operator+(const Color &other);
    Color operator-(const Color &other);
    operator uint32_t() const { return m_value; }

  private:
    union {
        uint32_t m_value;
        struct {
            uint8_t m_b;
            uint8_t m_g;
            uint8_t m_r;
            uint8_t m_w;
        };
    };
};

class Palette {
  public:
    static const uint8_t GAMMA[];
    Palette(const Palette &other);
    Palette(uint16_t resolution, const Color &colors,
            const Color &color_correction);
    Palette(uint16_t resolution, const ArrayList<Color> &colors,
            const Color &color_correction);

    void correct_colors(ArrayList<Color> &colors);
    void correct_colors(Color &colors);

    /**
     * linear interpolation
     */
    void interp(ArrayList<uint32_t> data, ArrayList<Color> &colors);
    void interp(uint32_t data, Color &color);

  private:
    ArrayList<Color> m_colors;
    ArrayList<uint32_t> m_colors_map;
    Color m_color_correction;
};

Palette RainbowPalette(uint32_t resolution = 255);

#endif
