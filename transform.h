#pragma once

#include <array>

#include "descriptors.h"

namespace mycv {

namespace transform {

using TransformationT = std::array<double, 9>;

TransformationT ransac(const desc::BlobsT& _first, const desc::BlobsT& _second, const desc::MatchesT& _matches,
                       size_t _iter_count = 1000, double _threshold = 5);

} // transform

} // mycv
