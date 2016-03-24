#pragma once

#include <QImage>

#include "descriptors.h"
#include "myimage.h"
#include "poi.h"

namespace mycv {

namespace qimg {

using QImagePtrT = std::unique_ptr<QImage>;

QImagePtrT toQImagePtr(const CMyImage& _image);
QImagePtrT drawPoints(const CMyImage& _image, const poi::PointsT& _points);
QImagePtrT mergeImageAndDrawLines(const CMyImage& _first_image, const poi::PointsT& _first_points,
                                  const CMyImage& _second_image, const poi::PointsT& _second_points,
                                  const desc::ConformityT& _conformity, unsigned char _line_alpha = 255);

} // qimg

} // mycv
