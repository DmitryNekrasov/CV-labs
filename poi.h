#pragma once

#include "matrix.h"
#include "myimage.h"

namespace mycv {

namespace poi {

using PointT    = std::pair<int, int>;
using PointsT   = std::vector<PointT>;

CMyImage    applyMoravec(const CMyImage& _image, int _w_size = 3);
CMyImage    applyHarris(const CMyImage& _image, int _w_size = 3, double _k = 0.06);

PointsT     getPoi(const CMyImage& _image, double _threshold, int _p_size = 3);

} // poi

} // mycv
