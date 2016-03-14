#pragma once

#include "matrix.h"
#include "myimage.h"

namespace mycv {

namespace poi {

using PointT    = std::pair<int, int>;
using PointsT   = std::vector<PointT>;

double      getC(const CMyImage& _image, int _w_size, int _x, int _y, int _dx, int _dy);
CMyImage    applyMoravec(const CMyImage& _image, int _w_size);
PointsT     getMoravecPoi(const CMyImage& _image, int _w_size, int _p_size, double _threshold);

} // poi

} // mycv
