#pragma once

#include <cmath>

#include "array.h"
#include "matrix.h"
#include "myimage.h"

namespace mycv {

using SeparableFilterT = std::pair<CArray, CArray>;
using PyramidLayerT = std::tuple<double, double, CMyImage>;
using GaussPyramidT = std::vector<PyramidLayerT>;

CMyImage applyConvolution(const CMyImage& _image, const CMatrix& _kernel);
CMyImage applySeparableFilter(const CMyImage& _image, const SeparableFilterT& _filter);

CMyImage getSobelDx(const CMyImage& _image);
CMyImage getSobelDy(const CMyImage& _image);
CMyImage getSobel(const CMyImage& _dx, const CMyImage& _dy);

CMatrix getGaussKernel(double _sigma);
SeparableFilterT getGaussSeparable(const CMatrix& _gauss_kernel);
SeparableFilterT getGaussSeparable(double _sigma);

CMyImage getDownscale(const CMyImage& _image);
CMyImage getUpscale(const CMyImage& _image);

GaussPyramidT getGaussPyramid(const CMyImage& _image, const int _n, const int _s,
                                    const double _sigma_a, const double _sigma_0);
double getL(const CMyImage& _image, const GaussPyramidT& _gauss_pyramid, int _y, int _x, double _sigma);

} // mycv
