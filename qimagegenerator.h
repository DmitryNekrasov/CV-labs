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
QImagePtrT showDescriptors(const CMyImage& _first_image, const CMyImage& _second_image,
                           const poi::PointsT& _first_points, const poi::PointsT& _second_points,
                           const desc::AnglesT& _first_angles, const desc::AnglesT& _second_angles, int _grid_size,
                           const desc::MatchesT& _matches, unsigned char _point_alpha = 255,
                           unsigned char _line_alpha = 255, unsigned char _rect_alpha = 255);
QImagePtrT showBlobs(const CMyImage& _image, const desc::BlobsT& _blobs);

} // qimg

} // mycv
