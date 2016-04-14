#pragma once

#include <cmath>

#include "array.h"
#include "matrix.h"
#include "myimage.h"

namespace mycv {

using SeparableFilterT = std::pair<CArray, CArray>;

CMyImage applyConvolution(const CMyImage& _image, const CMatrix& _kernel);
CMyImage applySeparableFilter(const CMyImage& _image, const SeparableFilterT& _filter);

CMyImage getSobelDx(const CMyImage& _image);
CMyImage getSobelDy(const CMyImage& _image);
CMyImage getGradientValues(const CMyImage& _dx, const CMyImage& _dy);
CMyImage getGradientDirections(const CMyImage& _dx, const CMyImage& _dy);

CMatrix getGaussKernel(double _sigma);
SeparableFilterT getGaussSeparable(const CMatrix& _gauss_kernel);
SeparableFilterT getGaussSeparable(double _sigma);

CMyImage getDownscale(const CMyImage& _image);
CMyImage getUpscale(const CMyImage& _image);

} // mycv
