#pragma once

#include <vector>

#include "myimage.h"
#include "poi.h"

namespace mycv {

namespace desc {

using DescriptorT = std::vector<double>;
using DescriptorsT = std::vector<DescriptorT>;
using MatchesT = std::vector<std::pair<size_t, size_t>>;

DescriptorsT getDescriptors(const CMyImage& _image, const poi::PointsT& _points,
                            int _descriptor_size, int _block_size, int _histogram_value_number);
MatchesT getMatches(const DescriptorsT& _first, const DescriptorsT& _second);

} // desc

} // mycv
