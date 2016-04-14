#pragma once

#include "myimage.h"

namespace mycv {

struct PyramidLayer {
    PyramidLayer(double _current_sigma, double _effective_sigma, CMyImage&& _image)
        : current_sigma(_current_sigma)
        , effective_sigma(_effective_sigma)
        , image(std::move(_image))
    {}

    double current_sigma;
    double effective_sigma;
    CMyImage image;
};

struct Octave {
    std::vector<PyramidLayer> layers;
    int level;
};

using PyramidT = std::vector<Octave>;

PyramidT getGaussPyramid(const CMyImage& _image, size_t _n, const size_t _s, const double _sigma_a,
                         const double _sigma_0, bool _from_octave_minus_one = false);
double getL(const PyramidT& _pyramid, int _y, int _x, double _sigma);
void savePyramid(const PyramidT& _pyramid, const std::string& _path);

} // mycv
