#include "Kernel.h"
#include "utils.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

void printKernel(struct Kernel *kernel) {
  printf("[%dx%d Kernel]\n", kernel->width, kernel->height);
  printf("Half X: %d\n", kernel->halfX);
  printf("Half Y: %d\n", kernel->halfY);
  for (int x = 0; x < kernel->width; x++) {
    for (int y = 0; y < kernel->width; y++) {
      printf("%3f ", kernel->array[sizeof(double) * (x + y * kernel->width)]);
    }

    printf("\n");
  }
}

void updateKernelSum(struct Kernel *kernel) {
  kernel->sum = 0;
  for (int x = 0; x < kernel->width; x++) {
    for (int y = 0; y < kernel->height; y++) {
      kernel->sum += kernel->array[sizeof(double) * (x + y * kernel->width)];
    }
  }
}

struct Kernel *newKernel(int width, int height, double initialValue) {
  struct Kernel *kernel = malloc(sizeof(struct Kernel));
  if (!kernel) {
    rexit("Unable to allocate memory for kernel");
  }

  double *kernelArray = malloc(sizeof(double) * width * height);
  if (!kernelArray) {
    rexit("Unable to allocate memory for kernel arrays");
  }

  kernel->width = width;
  kernel->halfX = width / 2;
  kernel->height = height;
  kernel->halfY = height / 2;
  kernel->sum = initialValue * width * height;
  kernel->array = kernelArray;

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      kernelArray[sizeof(double) * (x + y * width)] = initialValue;
    }
  }

  return kernel;
}

void freeKernel(struct Kernel *kernel) {
  if (kernel->array) {
    free(kernel->array);
  }
  if (kernel) {
    free(kernel);
  }
}

struct Kernel *newGaussianKernel(int width, int height,
                                 double standardDeviation) {
  struct Kernel *kernel = newKernel(width, height, 0);

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      int nx = x - width / 2;
      int ny = y - height / 2;
      double coefficient =
          (1 / (2 * 3.1415926535 * standardDeviation * standardDeviation));
      double ex =
          pow(2.718281828, -(nx * nx + ny * ny) /
                               (2 * standardDeviation * standardDeviation));

      double value = coefficient * ex;
      value = value > 1 ? 1 : value;
      value = value < 0 ? 0 : value;

      kernel->array[sizeof(double) * (x + y * width)] = value;
      kernel->sum += value;
    }
  }

  return kernel;
}
