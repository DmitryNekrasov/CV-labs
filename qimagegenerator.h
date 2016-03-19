#pragma once

#include <QImage>

#include "myimage.h"
#include "poi.h"

namespace mycv {

namespace qimg {

using QImagePtrT = std::unique_ptr<QImage>;

QImagePtrT toQImagePtr(const CMyImage& _image);
QImagePtrT drawPoints(const CMyImage& _image, const poi::PointsT& _points);

} // qimg

} // mycv
