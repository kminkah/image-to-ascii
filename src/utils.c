#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t brightnessFromRGB(uint8_t r, uint8_t g, uint8_t b,
                          enum GrayscaleMethod method) {
  float result;
  uint8_t maxValue;
  uint8_t minValue;

  float fr = (float)r;
  float fg = (float)g;
  float fb = (float)b;

  if (fr > fg && fr > fb) {
    maxValue = fr;
  } else if (fg > fr && fg > fb) {
    maxValue = fg;
  } else {
    maxValue = fb;
  }
  if (fr < fg && fr < fb) {
    minValue = fr;
  } else if (fg < fr && fg < fb) {
    minValue = fg;
  } else {
    minValue = fb;
  }

  switch (method) {
  case Weighted1:
    result = (fr * 0.3 + fg * 0.59 + fb * 0.11); // weighted, "perceived"
    break;
  case Weighted2:
    result = (fr * 0.299 + fg * 0.587 + fb * 0.114);
    break;
  case Saturation:
    result = (minValue + maxValue) / 2.0; // saturation
    break;
  case Min:
    result = minValue;
    break;
  case Max:
    result = maxValue;
    break;
  default:
    result = (fr + fg + fb) / 3.0; // average
  }

  return (uint8_t)result;
}

void rexit(char *message) {
  printf("%s\n", message);
  exit(EXIT_FAILURE);
}
