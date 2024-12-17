#ifndef IMAGE_H
#define IMAGE_H

#include "Kernel.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct Image {
  int width;
  int height;
  uint8_t minValue;
  uint8_t maxValue;
  int pixelSize;
  char *pixels;
};

enum ImageOperator {
  IMAGE_ADD = 0,
  IMAGE_SUB = 1,
  IMAGE_MUL = 2,
  IMAGE_DIV = 3,
  IMAGE_MAG = 4,
};

void printImageInfo(struct Image *image);

void freeImage(struct Image *image);

struct Image *newInitialImage(int width, int height, int pixelSize,
                              uint8_t initialValue);

struct Image *convolveImage(struct Image *image, struct Kernel *kernel);

struct Image *operatorImages(struct Image *image0, struct Image *image1,
                             enum ImageOperator imageOperator);

struct Image *ansiMapRGBxImage(struct Image *image);

struct Image *grayscaleRGBxImage(struct Image *image,
                                 enum GrayscaleMethod method);

struct Image *boxBlurImage(struct Image *image, int size);

struct Image *gaussianBlurImage(struct Image *image, int size,
                                double standardDeviation);

struct Image *sobelImage(struct Image *image, uint8_t min, uint8_t max);

struct Image *parsePPMFile(const char *fileName);

#endif
