#pragma once

#include <array>

#include "descriptors.h"

namespace mycv {

namespace transform {

using TransformationT = std::array<double, 9>;

TransformationT ransac(const desc::BlobsT& _first, const desc::BlobsT& _second, const desc::MatchesT& _matches,
                       size_t _iter_count = 1000, double _threshold = 5);
TransformationT hough(const CMyImage& _first_image, const CMyImage& _second_image,
                      const desc::BlobsT& _first_blobs, const desc::BlobsT& _second_blobs,
                      const desc::AnglesT& _first_angles, const desc::AnglesT& _second_angles,
                      const desc::MatchesT& _matches);

} // transform

} // mycv
