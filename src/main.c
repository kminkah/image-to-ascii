#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Image.h"
#include "Kernel.h"
#include "utils.h"

const int maxAsciiCharacters = 22;

const char asciiSpectrum[maxAsciiCharacters + 1] = "$@%&#0XxoO+*^=~-:\"'.` ";

void printUsage(char *s) {
  printf("Usage: %s <filename.ppm>(P6 PPM) charSet<1...%d> method1 method2 "
         "method3 ...\n",
         s, maxAsciiCharacters);
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printUsage(argv[0]);
    rexit("Invalid number of arguments");
  }

  int totalAsciiCharacters = 0;

  if (sscanf(argv[2], "%d", &totalAsciiCharacters) != 1) {
    printUsage(argv[0]);
    rexit("Provided ascii character length is not valid (must be an integer)");
  }

  if (totalAsciiCharacters < 0 || totalAsciiCharacters > maxAsciiCharacters) {
    printUsage(argv[0]);
    rexit("Provided ascii character length is not valid (must be between 0 < x "
          "< maxAsciiCharacters)");
  }

  char asciiChars[totalAsciiCharacters + 1];
  for (int i = 0; i < totalAsciiCharacters; i++) {
    double alpha = i / (totalAsciiCharacters - 1.0);
    int index = (int)(alpha * (maxAsciiCharacters - 1));
    printf("%d %f %d %c\n", i, alpha, index, asciiSpectrum[index]);
    asciiChars[i] = asciiSpectrum[index];
  }

  printf("totalAsciiCharacters: %d\n", totalAsciiCharacters);
  printf("ascii characters: %s\n", asciiChars);

  struct Image *image = parsePPMFile(argv[1]);

  printImageInfo(image);

  struct Image *grayscale = grayscaleRGBxImage(image, Weighted2);
  /*struct Image *grayscale = grayscaleRGBxImage(image, Saturation);*/
  printf("grayscaled\n");

  /*struct Image *blurredGrayscale = grayscale;*/
  /*struct Image *blurredGrayscale = boxBlurImage(grayscale, 7);*/
  /*struct Image *blurredGrayscale = gaussianBlurImage(grayscale, 7,
   * 0.84089642);*/
  /*struct Image *blurredGrayscale = gaussianBlurImage(grayscale, 3, 1);*/
  printf("blurred\n");

  struct Image *blurA = gaussianBlurImage(grayscale, 9, 0.84089642);
  struct Image *blurB = gaussianBlurImage(grayscale, 9, 1.84089642);
  struct Image *diff = operatorImages(blurB, blurA, IMAGE_SUB);
  /*struct Image *sum = operatorImages(grayscale, blurA, IMAGE_DIV);*/

  struct Image *imageBlurred = gaussianBlurImage(image, 9, 0.84089642);
  struct Image *ansiMap = ansiMapRGBxImage(imageBlurred);

  /*struct Image *sobels = sobelImage(blurredGrayscale, 170, 255);*/
  struct Image *sobels = sobelImage(diff, 127, 255);
  /*struct Image *sobels = sobelImage(blurredGrayscale, 80, 255);*/

  /*struct Kernel *goofyKernel = newKernel(5, 5, 0);*/
  /**/
  /*for (int i = 0; i < 9; i++) {*/
  /*  goofyKernel->array[sizeof(double) * i] = (double)(i * i);*/
  /*}*/
  /*struct Image *goofy = convolveImage(diff, goofyKernel);*/

  /*struct Image *finalImage = boxBlurImage(grayscale, 27);*/
  /*struct Image *finalImage = gaussianBlurImage(grayscale, 27, 0.84089642);*/
  /*struct Image *finalImage = diff;*/
  /*struct Image *finalImage = grayscale;*/
  /*struct Image *finalImage = goofy;*/
  /*struct Image *finalImage = operatorImages(grayscale, goofy, IMAGE_SUB);*/
  struct Image *finalImage = grayscale;
  /*struct Image *finalImage = sum;*/
  /*struct Image *finalImage = operatorImages(grayscale, diff, IMAGE_MAG);*/
  /*struct Image *finalImage = blurredGrayscale;*/
  /*struct Image *finalImage = &sobels[0];*/

  /*struct Image *finalColors = gaussianBlurImage(image, 24, 0.84089642);*/
  struct Image *finalColors = image;

  int column = 0;
  /*int row = 0;*/

  uint8_t ansiColorValue = 0;
  printf("\033[%dm", ansiColorValue);

  FILE *write = fopen("output.txt", "w");
  if (!write) {
    rexit("Unable to open an exit file");
  }
  for (int i = 0; i < finalImage->width * finalImage->height; i++) {
    uint8_t r = finalColors->pixels[i * finalColors->pixelSize + 0];
    uint8_t g = finalColors->pixels[i * finalColors->pixelSize + 1];
    uint8_t b = finalColors->pixels[i * finalColors->pixelSize + 2];
    uint8_t value = finalImage->pixels[i];
    uint8_t edgeValue = (int)sobels[0].pixels[i];
    uint8_t angle = sobels[1].pixels[i];
    uint8_t ansi = ansiMap->pixels[i];
    char c = ' ';

    if (ansi != ansiColorValue) {
      ansiColorValue = ansi;
      printf("\033[%dm", ansiColorValue);
    }

    angle += 255 / 16;

    if (edgeValue < 255) {
      /*if (edgeValue < 0) {*/
      /*uint8_t charIndex =*/
      /*    (uint8_t)((1 - (edgeValue / 255.0)) * (totalAsciiCharacters - 1));*/
      /*c = asciiChars[charIndex];*/
      if (angle < 1 * 255 / 8) {
        c = '_';
      } else if (angle < 2 * 255 / 8) {
        c = '/';
      } else if (angle < 3 * 255 / 8) {
        c = '|';
      } else if (angle < 4 * 255 / 8) {
        c = '\\';
      } else if (angle < 5 * 255 / 8) {
        c = '-';
      } else if (angle < 6 * 255 / 8) {
        c = '/';
      } else if (angle < 7 * 255 / 8) {
        c = '|';
      } else {
        c = '\\';
      }
    } else {
      uint8_t charIndex =
          (uint8_t)((1 - (value / 255.0)) * (totalAsciiCharacters - 1));
      /*if (i % 4 == 0) {*/
      /*  charIndex = rand() % totalAsciiCharacters;*/
      /*}*/
      c = asciiChars[charIndex];
    }

    // printf("%d", charIndex);
    printf("%c", c);
    /*printf("\033[38;2;%d;%d;%dm%c\033[0m", r, g, b, c);*/

    fprintf(write, "%c", c);

    column += 1;

    if (column > finalImage->width - 1) {
      column = 0;
      /*row += 1;*/

      printf("\n");
      fprintf(write, "\n");
    }
  }

  fclose(write);

  ansiColorValue = 0;
  printf("\033[%dm", ansiColorValue);

  freeImage(image);

  return 0;
}
