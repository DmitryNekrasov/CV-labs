#pragma once

#include <array>

#include "descriptors.h"

namespace mycv {

namespace transform {

using TransformationT = std::array<double, 9>;

TransformationT getTransformation(const desc::BlobsT& _first, const desc::BlobsT& _second);
TransformationT ransac(const desc::BlobsT& _first, const desc::BlobsT& _second, const desc::MatchesT& _matches,
                       size_t _iter_count = 100, double _threshold = 1.0);

} // transform

} // mycv
