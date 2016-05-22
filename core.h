#pragma once

#include <cmath>

#include "array.h"
#include "matrix.h"
#include "myimage.h"

namespace mycv {

using SeparableFilterT = std::pair<CArray, CArray>;

CMyImage applyConvolution(const CMyImage& _image, const CMatrix& _kernel);
CMyImage applySeparableFilter(const CMyImage& _image, const SeparableFilterT& _filter);

double getGradientValue(double _gx, double _gy);
double getGradientDirection(double _gx, double _gy);

CMyImage getSobelDx(const CMyImage& _image);
CMyImage getSobelDy(const CMyImage& _image);
CMyImage getGradientValues(const CMyImage& _dx, const CMyImage& _dy);
CMyImage getGradientDirections(const CMyImage& _dx, const CMyImage& _dy);

CMatrix getGaussKernel(double _sigma);
SeparableFilterT getGaussSeparable(const CMatrix& _gauss_kernel);
SeparableFilterT getGaussSeparable(double _sigma);

CMyImage getDownscale(const CMyImage& _image);
CMyImage getUpscale(const CMyImage& _image);

double getDx(const CMyImage& _image, int _row, int _col);
double getDy(const CMyImage& _image, int _row, int _col);
double getDx2(const CMyImage& _image, int _row, int _col);
double getDy2(const CMyImage& _image, int _row, int _col);
double getDxDy(const CMyImage& _image, int _row, int _col);

} // mycv
