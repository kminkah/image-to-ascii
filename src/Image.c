#include "Image.h"
#include "Kernel.h"
#include "utils.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
40 = black
41 = red
42 = green
43 = yellow
44 = blue
45 = magenta
46 = cyan
47 = white
*/

/*int totalAnsiColors = 8;*/
int totalAnsiColors = 8;
uint8_t ansiColors[][3] = {
    {0, 0, 0},
    {153, 0, 0},
    {0, 166, 0},
    {153, 153, 0},
    {0, 0, 178},
    {178, 0, 178},
    {0, 166, 178},
    {191, 191, 191},
    //
    {103, 103, 103},
    {203, 0, 0},
    {0, 217, 0},
    {230, 230, 0},
    {0, 0, 255},
    {230, 0, 230},
    {0, 230, 230},
    {230, 230, 230},
};
uint8_t ansiValues[] = {30, 31, 32, 33, 34, 35, 36, 37,
                        90, 91, 92, 93, 94, 95, 96, 97};

void printImageInfo(struct Image *image) {
  printf("[%dx%d Image]\n", image->width, image->height);
  printf("Min Value: %d\n", image->minValue);
  printf("Max Value: %d\n", image->maxValue);
}

void freeImage(struct Image *image) {
  if (image->pixels) {
    free(image->pixels);
  }
  if (image) {
    free(image);
  }
}

struct Image *newInitialImage(int width, int height, int pixelSize,
                              uint8_t initialValue) {
  struct Image *newImage = malloc(sizeof(struct Image));
  if (!newImage) {
    rexit("Unable to allocate memory for image");
  }

  newImage->width = width;
  newImage->height = height;
  newImage->pixelSize = pixelSize;
  newImage->pixels =
      malloc(newImage->pixelSize * newImage->width * newImage->height);

  if (!newImage->pixels) {
    rexit("Error allocating memory for pixel pixels");
  }

  for (int i = 0; i < pixelSize * width * height; i++) {
    newImage->pixels[i] = initialValue;
  }

  return newImage;
}

struct Image *convolveImage(struct Image *image, struct Kernel *kernel) {
  struct Image *newImage = malloc(sizeof(struct Image));
  if (!newImage) {
    rexit("Unable to allocate memory for image");
  }

  newImage->width = image->width;
  newImage->height = image->height;
  newImage->pixelSize = image->pixelSize;
  newImage->pixels =
      malloc(newImage->pixelSize * newImage->width * newImage->height);

  if (!newImage->pixels) {
    rexit("Error allocating memory for pixel pixels");
  }

  printf("convolving\n");

  int x = 0;
  int y = 0;
  uint8_t minValue = 255;
  uint8_t maxValue = 0;
  for (int pixelIndex = 0; pixelIndex < image->width * image->height;
       pixelIndex++) {
    for (int placement = 0; placement < newImage->pixelSize; placement++) {
      double value = 0;
      for (int ox = 0; ox < kernel->width; ox++) {
        for (int oy = 0; oy < kernel->height; oy++) {
          int nx = x + ox - kernel->halfX;
          int ny = y + oy - kernel->halfY;
          if (nx < 0 || nx > newImage->width - 1 || ny < 0 ||
              ny > newImage->height - 1) {
            continue;
          }

          int newPixelIndex = nx + ny * newImage->width;
          uint8_t pixelValue =
              image->pixels[image->pixelSize * newPixelIndex + placement];

          value += pixelValue *
                   kernel->array[sizeof(double) * (ox + oy * kernel->width)];
        }
      }

      value /= kernel->sum;

      value = value < 0 ? 0 : value;
      value = value > 255 ? 255 : value;

      uint8_t intValue = (uint8_t)(value);

      minValue = intValue < minValue ? intValue : minValue;
      maxValue = intValue > maxValue ? intValue : maxValue;

      newImage->pixels[pixelIndex * newImage->pixelSize + placement] = intValue;
    }

    x += 1;
    if (x > image->width - 1) {
      x = 0;
      y += 1;
    }
  }

  newImage->minValue = minValue;
  newImage->maxValue = maxValue;

  return newImage;
}

struct Image *operatorImages(struct Image *image0, struct Image *image1,
                             enum ImageOperator imageOperator) {

  if (image0->width != image1->width || image0->height != image1->height) {
    rexit("Images do not have equal sizes");
  }

  struct Image *newImage = malloc(sizeof(struct Image));

  if (!newImage) {
    rexit("Unable to allocate memory for image");
  }

  newImage->width = image0->width;
  newImage->height = image0->height;
  newImage->pixelSize = image0->pixelSize;
  newImage->pixels =
      malloc(newImage->pixelSize * newImage->width * newImage->height);

  if (!newImage->pixels) {
    rexit("Error allocating memory for pixel pixels");
  }

  printf("operating\n");

  uint8_t minValue = 255;
  uint8_t maxValue = 0;
  for (int pixelIndex = 0; pixelIndex < newImage->width * newImage->height;
       pixelIndex++) {
    for (int placement = 0; placement < newImage->pixelSize; placement++) {
      uint8_t value0 =
          image0->pixels[image0->pixelSize * pixelIndex + placement];
      uint8_t value1 =
          image1->pixels[image1->pixelSize * pixelIndex + placement];

      double dvalue0 = (double)value0;
      double dvalue1 = (double)value1;

      double dvalue;

      switch (imageOperator) {
      case IMAGE_SUB:
        dvalue = dvalue0 - dvalue1;
        break;
      case IMAGE_MUL:
        dvalue = dvalue0 * dvalue1;
        break;
      case IMAGE_DIV:
        dvalue = dvalue0 / dvalue1;
        break;
      case IMAGE_MAG:
        dvalue = sqrt(dvalue0 * dvalue0 + dvalue1 * dvalue1);
        break;
      default:
        dvalue = dvalue0 + dvalue1;
      }

      dvalue = dvalue < 0 ? 0 : dvalue;
      dvalue = dvalue > 255 ? 255 : dvalue;

      uint8_t value = (uint8_t)dvalue;

      minValue = value < minValue ? value : minValue;
      maxValue = value > maxValue ? value : maxValue;

      newImage->pixels[pixelIndex * newImage->pixelSize + placement] = value;
    }
  }

  newImage->minValue = minValue;
  newImage->maxValue = maxValue;

  return newImage;
}

struct Image *ansiMapRGBxImage(struct Image *image) {
  if (image->pixelSize < 3) {
    rexit("Image has a pixel size smaller than 3");
  }

  struct Image *newImage = malloc(sizeof(struct Image));
  if (!newImage) {
    rexit("Unable to allocate memory for image");
  }

  newImage->width = image->width;
  newImage->height = image->height;
  newImage->pixelSize = 1;
  newImage->pixels =
      malloc(newImage->pixelSize * newImage->width * newImage->height);

  if (!newImage->pixels) {
    rexit("Error allocating memory for pixel pixels");
  }

  uint8_t minValue = 255;
  uint8_t maxValue = 0;

  for (int pixelIndex = 0; pixelIndex < image->width * image->height;
       pixelIndex++) {
    uint8_t r, g, b;
    r = image->pixels[pixelIndex * image->pixelSize + 0];
    g = image->pixels[pixelIndex * image->pixelSize + 1];
    b = image->pixels[pixelIndex * image->pixelSize + 2];

    double dr = (double)r;
    double dg = (double)g;
    double db = (double)b;

    /*printf("ok %d %d %d\n", r, g, b);*/

    double magnitude = sqrt(dr * dr + dg * dg + db * db);
    uint8_t value = ansiValues[0];

    for (int i = 1; i < totalAnsiColors; i++) {
      uint8_t *ansiColor = ansiColors[i];
      double newMagnitude =
          sqrt(pow(ansiColor[0] - dr, 2) + pow(ansiColor[1] - dg, 2) +
               pow(ansiColor[2] - db, 2));
      if (newMagnitude < magnitude) {
        value = ansiValues[i];
        magnitude = newMagnitude;
      }
    }

    /*if (value == ansiValues[0]) {*/
    /*  value = ansiValues[7];*/
    /*}*/

    minValue = value < minValue ? value : minValue;
    maxValue = value > maxValue ? value : maxValue;

    newImage->pixels[pixelIndex * newImage->pixelSize] = value;
  }

  newImage->minValue = image->minValue;
  newImage->maxValue = image->maxValue;

  return newImage;
}

struct Image *grayscaleRGBxImage(struct Image *image,
                                 enum GrayscaleMethod method) {
  if (image->pixelSize < 3) {
    rexit("Image has a pixel size smaller than 3");
  }

  struct Image *newImage = malloc(sizeof(struct Image));
  if (!newImage) {
    rexit("Unable to allocate memory for image");
  }

  newImage->width = image->width;
  newImage->height = image->height;
  newImage->pixelSize = 1;
  newImage->pixels =
      malloc(newImage->pixelSize * newImage->width * newImage->height);

  if (!newImage->pixels) {
    rexit("Error allocating memory for pixel pixels");
  }

  uint8_t minValue = 255;
  uint8_t maxValue = 0;

  for (int pixelIndex = 0; pixelIndex < image->width * image->height;
       pixelIndex++) {
    char r, g, b;
    r = image->pixels[pixelIndex * image->pixelSize + 0];
    g = image->pixels[pixelIndex * image->pixelSize + 1];
    b = image->pixels[pixelIndex * image->pixelSize + 2];

    uint8_t value = brightnessFromRGB(r, g, b, method);

    minValue = value < minValue ? value : minValue;
    maxValue = value > maxValue ? value : maxValue;

    newImage->pixels[pixelIndex * newImage->pixelSize] = value;
  }

  newImage->minValue = image->minValue;
  newImage->maxValue = image->maxValue;

  return newImage;
}

struct Image *boxBlurImage(struct Image *image, int size) {
  struct Kernel *kernel = newKernel(size, size, 1.0);

  printKernel(kernel);

  struct Image *newImage = convolveImage(image, kernel);

  freeKernel(kernel);

  return newImage;
}

struct Image *gaussianBlurImage(struct Image *image, int size,
                                double standardDeviation) {
  struct Kernel *kernel = newGaussianKernel(size, size, standardDeviation);

  printKernel(kernel);

  struct Image *newImage = convolveImage(image, kernel);

  freeKernel(kernel);

  return newImage;
}

struct Image *sobelImage(struct Image *image, uint8_t min, uint8_t max) {
  struct Image *sobels = malloc(2 * sizeof(struct Image));
  if (!sobels) {
    rexit("Unable to allocate memory for image");
  }

  sobels[0].width = image->width;
  sobels[0].height = image->height;
  sobels[0].pixelSize = image->pixelSize;
  sobels[0].pixels =
      malloc(sobels[0].pixelSize * sobels[0].width * sobels[0].height);

  sobels[1].width = image->width;
  sobels[1].height = image->height;
  sobels[1].pixelSize = image->pixelSize;
  sobels[1].pixels =
      malloc(sobels[1].pixelSize * sobels[1].width * sobels[1].height);

  if (!sobels[0].pixels || !sobels[1].pixels) {
    rexit("Unable to allocate memory for image pixels");
  }

  struct Kernel *kernelX = newKernel(3, 3, 0);
  struct Kernel *kernelY = newKernel(3, 3, 0);
  int sobelXArray[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
  int sobelYArray[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};

  for (int i = 0; i < 9; i++) {
    kernelX->array[sizeof(double) * i] = (double)sobelXArray[i];
    kernelY->array[sizeof(double) * i] = (double)sobelYArray[i];
  }

  printf("sobeling\n");

  int x = 0;
  int y = 0;
  uint8_t minValue = 255, minAngle = 255;
  uint8_t maxValue = 0, maxAngle = 0;
  double maxdvalue = -10000;
  double mindvalue = 10000;
  double *dvalues =
      malloc(sizeof(double) * image->pixelSize * image->width * image->height);
  if (!dvalues) {
    rexit("Not enough memory for dvalues");
  }

  for (int pixelIndex = 0; pixelIndex < image->width * image->height;
       pixelIndex++) {
    for (int placement = 0; placement < image->pixelSize; placement++) {
      double dvalueX = 0;
      double dvalueY = 0;
      for (int ox = 0; ox < 3; ox++) {
        for (int oy = 0; oy < 3; oy++) {
          int nx = x + ox - 1;
          int ny = y + oy - 1;
          if (nx < 0 || nx > image->width - 1 || ny < 0 ||
              ny > image->height - 1) {
            continue;
          }

          int newPixelIndex = nx + ny * image->width;
          uint8_t pixelValue =
              image->pixels[image->pixelSize * newPixelIndex + placement];

          dvalueX +=
              pixelValue * kernelX->array[sizeof(double) * (ox + oy * 3)];
          dvalueY +=
              pixelValue * kernelY->array[sizeof(double) * (ox + oy * 3)];
        }
      }

      double dvalue = sqrt(dvalueX * dvalueX + dvalueY * dvalueY);
      double dangle = atan2(dvalueY, dvalueX);

      mindvalue = dvalue < mindvalue ? dvalue : mindvalue;
      maxdvalue = dvalue > maxdvalue ? dvalue : maxdvalue;

      dvalues[pixelIndex * image->pixelSize + placement] = dvalue;

      /*printf("%f ", dangle);*/
      uint8_t angle = (uint8_t)(255 * (dangle / (2 * 3.14159265358979) + 0.5));
      /*uint8_t angle =*/
      /*    (uint8_t)(255 * (dangle % 3.14159265358979) / 3.14159265358979);*/
      /*printf("%d\n", angle);*/

      angle = angle < 0 ? 0 : angle;
      angle = angle > 255 ? 255 : angle;

      minAngle = angle < minAngle ? angle : minAngle;
      maxAngle = angle > maxAngle ? angle : maxAngle;

      sobels[1].pixels[pixelIndex * sobels[1].pixelSize + placement] = angle;
    }

    x += 1;
    if (x > image->width - 1) {
      x = 0;
      y += 1;
    }
  }

  for (int pixelIndex = 0; pixelIndex < image->width * image->height;
       pixelIndex++) {
    for (int placement = 0; placement < image->pixelSize; placement++) {
      double dvalue = dvalues[pixelIndex * image->pixelSize + placement];
      dvalue = 255 * dvalue / (sqrt(2 * 255 * 255));
      /*dvalue = 255 * (dvalue - mindvalue) / (maxdvalue - mindvalue);*/

      dvalue = dvalue < 0 ? 0 : dvalue;
      dvalue = dvalue > 255 ? 255 : dvalue;

      uint8_t value = (uint8_t)dvalue;
      minValue = value < minValue ? value : minValue;
      maxValue = value > maxValue ? value : maxValue;

      if (value < min || value > max) {
        value = 255;
      } else {
        value = 0;
      }

      sobels[0].pixels[pixelIndex * sobels[0].pixelSize + placement] = value;
    }
  }

  sobels[0].minValue = minValue;
  sobels[0].maxValue = maxValue;

  sobels[1].minValue = minAngle;
  sobels[1].maxValue = maxAngle;

  return sobels;
}

struct Image *parsePPMFile(const char *filePath) {
  FILE *ppmFile = fopen(filePath, "r");

  if (!ppmFile) {
    rexit("File does not exist");
  }

  char magicNumber[4];
  if (!fgets(magicNumber, sizeof(magicNumber), ppmFile)) {
    rexit("Error reading magic number");
  }

  if (strcmp(magicNumber, "P6\n") != 0) {
    printf("%s\n", magicNumber);
    rexit("Unsupported format. Only P6 is supported.\n");
  }

  int width, height;
  uint8_t minValue = 255;
  int maxValue = 0;
  char ch;
  struct Image *image = malloc(sizeof(struct Image));
  if (!image) {
    rexit("Unable to allocate memory for image");
  }

  // Skip comments
  do {
    ch = fgetc(ppmFile);
    if (ch == '#') {
      while (fgetc(ppmFile) != '\n')
        ;
    } else {
      ungetc(ch, ppmFile);
      break;
    }
  } while (1);

  if (fscanf(ppmFile, "%d %d %d", &width, &height, &maxValue) != 3) {
    rexit("Unable to parse header");
  }

  if (maxValue != 255) {
    rexit("Unsupported max color value. Only 255 is supported.\n");
  }

  // Skip a single whitespace character after the header
  fgetc(ppmFile);

  image->width = width;
  image->height = height;
  image->maxValue = (uint8_t)maxValue;
  image->pixelSize = 3;
  image->pixels = malloc(image->pixelSize * width * height);

  if (!image->pixels) {
    rexit("Error allocating memory for pixel pixels");
  }

  if (2 == 2) {
    int i = 0;
    int value = 0;
    while (value != EOF) {
      value = fgetc(ppmFile);
      // printf("%c\n", value);
      if (value == EOF) {
        // printf("terminated; reached EOF %d\n", i);
        break;
      } else {
        if (value < minValue) {
          minValue = value;
        }

        image->pixels[i] = value;
        i += 1;
      }
    }

    image->minValue = minValue;
  } else {
    if (fread(image->pixels, image->pixelSize, width * height, ppmFile) !=
        (size_t)(width * height)) {
      rexit("Error reading pixel pixels\n");
    }
  }

  fclose(ppmFile);

  return image;
}
