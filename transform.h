#pragma once

#include <array>

#include "descriptors.h"

namespace mycv {

namespace transform {

using TransformationT = std::array<double, 9>;

TransformationT getTransformation(const desc::BlobsT& _first, const desc::BlobsT& _second);

} // transform

} // mycv
