#ifndef CORE_H
#define CORE_H

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
double      getGradient(double _x, double _y);

CMyImage    getSobelDx(const CMyImage& _image);
CMyImage    getSobelDy(const CMyImage& _image);
CMyImage    getSobel(const CMyImage& _dx, const CMyImage& _dy);

} // mycv

#endif // CORE_H
