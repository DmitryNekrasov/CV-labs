#pragma once

#include <vector>

#include "myimage.h"
#include "poi.h"
#include "pyramid.h"

namespace mycv {

namespace desc {

using DescriptorT = std::vector<double>;
using DescriptorsT = std::vector<DescriptorT>;
using MatchesT = std::vector<std::pair<size_t, size_t>>;
using AnglesT = std::vector<double>;

struct Blob {
    Blob(int _x, int _y, double _sigma, const PyramidLayer* _layer, int _scale)
        : x(_x)
        , y(_y)
        , sigma(_sigma)
        , layer(_layer)
        , scale(_scale)
    {}

    Blob()
        : Blob(0, 0, 0, nullptr, 0)
    {}

    int x;
    int y;
    double sigma;
    const PyramidLayer* layer;
    int scale;
};

using BlobsT = std::vector<Blob>;

std::tuple<DescriptorsT, AnglesT> getDescriptors(const CMyImage& _image, poi::PointsT& _points,
                                                 int _descriptor_size, int _block_size, int _histogram_value_number);

std::tuple<DescriptorsT, AnglesT, BlobsT> getDescriptors(const CMyImage& _image, int _descriptor_size, int _histogram_value_number);

BlobsT getBlobs(const PyramidT& _pyramid);

MatchesT getMatches(const DescriptorsT& _first, const DescriptorsT& _second);

} // desc

} // mycv
