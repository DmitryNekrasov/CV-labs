#pragma once

#include <vector>

#include "myimage.h"
#include "poi.h"

namespace mycv {

namespace desc {

using DescriptorT = std::vector<double>;
using DescriptorsT = std::vector<DescriptorT>;

DescriptorsT getDescriptors(const CMyImage& _image, const poi::PointsT& _points,
                            int _descriptor_size, int _block_size, int _histogram_value_number);

} // desc

} // mycv
