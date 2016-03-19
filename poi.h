#pragma once

#include <list>

#include "matrix.h"
#include "myimage.h"

namespace mycv {

namespace poi {

using PointT    = std::tuple<int, int, double>;
using PointsT   = std::list<PointT>;

CMyImage    applyMoravec(const CMyImage& _image, int _w_size);
CMyImage    applyHarris(const CMyImage& _image, int _w_size, double _k = 0.06);

PointsT     getPoi(const CMyImage& _image, double _threshold, int _p_size);
PointsT     filterPoint(const PointsT& _points, size_t _target_quantity);

} // poi

} // mycv
