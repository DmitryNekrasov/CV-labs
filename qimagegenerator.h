#pragma once

#include <QImage>

#include "descriptors.h"
#include "myimage.h"
#include "poi.h"
#include "transform.h"

namespace mycv {

namespace qimg {

using QImagePtrT = std::unique_ptr<QImage>;

QImagePtrT toQImagePtr(const CMyImage& _image);
QImagePtrT drawPoints(const CMyImage& _image, const poi::PointsT& _points);
QImagePtrT showDescriptors(const CMyImage& _first_image, const CMyImage& _second_image,
                           const desc::BlobsT& _first_blobs, const desc::BlobsT& _second_blobs,
                           const desc::AnglesT& _first_angles, const desc::AnglesT& _second_angles,
                           const desc::MatchesT& _matches, unsigned char _point_alpha = 255,
                           unsigned char _line_alpha = 255, unsigned char _rect_alpha = 255);
QImagePtrT showBlobs(const CMyImage& _image, const desc::BlobsT& _blobs, const desc::AnglesT& _angles);
QImagePtrT getPanorama(const CMyImage& _first_image, const CMyImage& _second_image,
                       const transform::TransformationT& _h);

} // qimg

} // mycv
