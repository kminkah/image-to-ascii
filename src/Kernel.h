
#ifndef KERNEL_H
#define KERNEL_H

#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct Kernel {
  int width;
  int height;
  int halfX;
  int halfY;
  double sum;
  double *array;
};

void printKernel(struct Kernel *kernel);

void freeKernel(struct Kernel *kernel);

void updateKernelSum(struct Kernel *kernel);

struct Kernel *newKernel(int width, int height, double initialValue);

struct Kernel *newGaussianKernel(int width, int height,
                                 double standardDeviation);

#endif
