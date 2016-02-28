#pragma once

#include <cmath>

#include "matrix.h"
#include "myimage.h"

namespace mycv {

const int sobel_kernel_size = 3;

const double sobel_kernel_x_array[] = {
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1
};

const double sobel_kernel_y_array[] = {
    -1, -2, -1,
    0, 0, 0,
    1, 2, 1
};

CMyImage    applyConvolution(const CMyImage& _image, const CMatrix& _kernel);
CMyImage    applySeparableFilter(const CMyImage& _image, const double _filter[], int _size);

CMyImage    getSobelDx(const CMyImage& _image);
CMyImage    getSobelDy(const CMyImage& _image);
CMyImage    getSobel(const CMyImage& _dx, const CMyImage& _dy);

static inline double getGradient(double _x, double _y) {
    return sqrt(_x * _x + _y * _y);
}

CMatrix     getGaussKernel(double _sigma);
CMyImage    getDownscale(const CMyImage& _image);

} // mycv
