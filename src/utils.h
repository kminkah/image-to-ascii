#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

enum GrayscaleMethod {
  Average = 0,
  Weighted1 = 1,
  Weighted2 = 2,
  Saturation = 10,
  Min = 20,
  Max = 21,
};

void rexit(char *message);

uint8_t brightnessFromRGB(uint8_t r, uint8_t g, uint8_t b,
                          enum GrayscaleMethod method);

#endif
