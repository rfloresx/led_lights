
#include "palette.h"

#define RGB_RED(hex) ((hex >> 16) & 0xff)
#define RGB_GREEN(hex) ((hex >> 8) & 0xff)
#define RGB_BLUE(hex) ((hex >> 0) & 0xff)

#define RGB_HEX(r, g, b) ((r << 16) | (g << 8) | (b))

#define CLAMP(val, min, max) (val < min ? min : val > max ? max : val)
#define CLAMP_UINT8(val) ((uint8_t)(val < 0 ? 0 : val > 0xff ? 0xff : val))

static void linspace(uint32_t start, uint32_t stop, uint32_t num,
                     ArrayList<uint32_t> &result);
static uint32_t linspace_index(uint32_t data,
                               const ArrayList<uint32_t> &data_list);
static Color color_interp(uint32_t pos, uint32_t start, uint32_t end,
                          const Color &color_start, const Color &color_end);

/*==========================================================================
 * Color Class:
 *==========================================================================*/
const Color Color::BLACK(0x000000);
const Color Color::WHITE(0xFFFFFF);
const Color Color::RED(0xFF0000);
const Color Color::GREEN(0x00FF00);
const Color Color::BLUE(0x0000FF);
const Color Color::ORANGE(0xFF8000);
const Color Color::YELLOW(0xFFFF00);
const Color Color::LIME(0x80FF00);
const Color Color::AQUA(0x00FF80);
const Color Color::CYAN(0x00FFFF);
const Color Color::OCEAN(0x0080FF);
const Color Color::VIOLET(0x8000FF);
const Color Color::MAGENTA(0xFF00FF);
const Color Color::RASPBERRY(0xFF0080);

Color::Color() : Color(0) {}
Color::Color(uint32_t hex) : m_value(hex) {}
Color::Color(const Color &color) : Color(color.m_value) {}
Color::Color(uint8_t r, uint8_t g, uint8_t b) : Color(RGB_HEX(r, g, b)) {}

uint32_t Color::Value() const { return m_value; }
uint8_t Color::R() const { return m_r; }
uint8_t Color::G() const { return m_g; }
uint8_t Color::B() const { return m_b; }
uint8_t Color::W() const { return m_w; }

Color &Color::operator=(const Color &other) {
    m_value = other.m_value;
    return *this;
}

Color Color::operator*(const Color &other) {
    // return
    uint16_t r = (m_r * other.m_r) / 255;
    uint16_t g = (m_g * other.m_g) / 255;
    uint16_t b = (m_b * other.m_b) / 255;
    return Color(CLAMP_UINT8(r), CLAMP_UINT8(g), CLAMP_UINT8(b));
}

Color Color::operator/(const Color &other) {
    uint16_t r = (m_r / other.m_r) * 255;
    uint16_t g = (m_g / other.m_g) * 255;
    uint16_t b = (m_b / other.m_b) * 255;
    return Color(CLAMP_UINT8(r), CLAMP_UINT8(g), CLAMP_UINT8(b));
}
Color Color::operator+(const Color &other) {
    uint16_t r = (m_r + other.m_r);
    uint16_t g = (m_g + other.m_g);
    uint16_t b = (m_b + other.m_b);
    return Color(CLAMP_UINT8(r), CLAMP_UINT8(g), CLAMP_UINT8(b));
}
Color Color::operator-(const Color &other) {
    int16_t r = (m_r - other.m_r);
    int16_t g = (m_g - other.m_g);
    int16_t b = (m_b - other.m_b);
    return Color(CLAMP_UINT8(r), CLAMP_UINT8(g), CLAMP_UINT8(b));
}

/*==========================================================================
 * Palette Class
 *==========================================================================*/
Palette::Palette(const Palette &other)
    : m_colors(other.m_colors), m_colors_map(other.m_colors_map),
      m_color_correction(other.m_color_correction) {}

Palette::Palette(uint16_t resolution, const Color &color,
                 const Color &color_correction)
    : m_colors(2), m_colors_map(), m_color_correction(color_correction) {
    m_colors[1] = color;
    linspace(0, resolution, m_colors.count(), m_colors_map);
}

Palette::Palette(uint16_t resolution, const ArrayList<Color> &colors,
                 const Color &color_correction)
    : m_colors(colors), m_colors_map(), m_color_correction(color_correction) {
    linspace(0, resolution, colors.count(), m_colors_map);
}

void Palette::correct_colors(ArrayList<Color> &colors) {
    for (int i = 0; i < colors.count(); i++) {
        Color &color = colors[i];
        correct_colors(color);
    }
}

void Palette::correct_colors(Color &color) {
    color = color * m_color_correction;
}

/**
 * linear interpolation
 */
void Palette::interp(ArrayList<uint32_t> data, ArrayList<Color> &colors) {
    for (int i = 0; i < data.count(); i++) {
        interp(data[i], colors[i]);
    }
}

void Palette::interp(uint32_t data, Color &color) {
    uint32_t index = linspace_index(data, m_colors_map);
    if (index < 0) {
        color = m_colors[0];
    } else if (index >= m_colors_map.count()) {
        color = m_colors[m_colors_map.count() - 1];
    } else {
        uint32_t start = m_colors_map[index];
        uint32_t end = m_colors_map[index + 1];
        Color color_start = m_colors[index];
        Color color_end = m_colors[index + 1];
        color = color_interp(data, start, end, color_start, color_end);
    }
}

/*==========================================================================
 * Local Static functions
 *==========================================================================*/
static void linspace(uint32_t start, uint32_t stop, uint32_t num,
                     ArrayList<uint32_t> &result) {
    if (num == 0) {
        return;
    }
    if (num == 1) {
        result.add(num);
        return;
    }
    // num must be 2+
    uint32_t distance = stop - start;
    uint32_t step = distance / (num - 1);

    result.resize(num);
    uint32_t n = start;
    while (result.count() < num) {
        result.add(n);
        n += step;
    }
    result[num - 1] = stop;
}

static uint32_t linspace_index(uint32_t data,
                               const ArrayList<uint32_t> &data_list) {
    for (int i = 0; i < data_list.count(); i++) {
        if (data < data_list[i]) {
            return i - 1;
        }
    }
    return data_list.count();
}

static Color color_interp(uint32_t pos, uint32_t start, uint32_t end,
                          const Color &color_start, const Color &color_end) {
    float delta = ((float)pos - start) / ((float)end - start);

    uint8_t r =
        (uint8_t)(delta * (color_end.R() - color_start.R()) + color_start.R());
    uint8_t g =
        (uint8_t)(delta * (color_end.G() - color_start.G()) + color_start.G());
    uint8_t b =
        (uint8_t)(delta * (color_end.B() - color_start.B()) + color_start.B());
    return Color(r, g, b);
}

Palette RainbowPalette(uint32_t resolution) {
    Color colors[] = {Color::RED,  Color::YELLOW,  Color::GREEN, Color::CYAN,
                      Color::BLUE, Color::MAGENTA, Color::RED};
    return Palette(resolution, ArrayList<Color>(colors, COUNT_OF(colors)),
                   Color(255, 120, 120));
}
