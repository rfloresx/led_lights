#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <Adafruit_NeoPixel.h>

#define STRIP_LED_COUNT 250
#define STRIP_PIN 5
#define STRIP_TYPE (NEO_RBG + NEO_KHZ800)
#define STRIP_REFRESH_RATE 60
#define STRIP_TASK_CORE 0

#define EFFECTS_REFRESH_RATE 60
#define EFFECTS_TASK_CORE 1

#endif
