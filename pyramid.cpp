#include "pyramid.h"

#include "core.h"
#include "qimagegenerator.h"
#include "simple.h"

namespace mycv {

PyramidT getGaussPyramid(const CMyImage& _image, size_t _n, const size_t _s, const double _sigma_a,
                         const double _sigma_0, bool _from_octave_minus_one /* = false */)
{
    auto k = pow(2.0, 1.0 / _s);
    std::vector<SeparableFilterT> filters;

    auto old_sigma = _sigma_0;
    for (size_t i = 0; i < _s; i++) {
        auto new_sigma = old_sigma * k;
        auto sigma = smpl::getSigmaB(new_sigma, old_sigma);
        filters.push_back(getGaussSeparable(sigma));
        old_sigma = new_sigma;
    }

    if (_from_octave_minus_one) {
        _n++;
    }

    PyramidT gauss_pyramid(_n);

    auto sigma_b = smpl::getSigmaB(_sigma_0, _sigma_a);
    auto gauss = getGaussSeparable(sigma_b);

    if (_from_octave_minus_one) {
        gauss_pyramid.front().layers.emplace_back(_sigma_0, _sigma_0, applySeparableFilter(getUpscale(_image), gauss));
    } else {
        gauss_pyramid.front().layers.emplace_back(_sigma_0, _sigma_0, applySeparableFilter(_image, gauss));
    }

    for (auto octave_it = gauss_pyramid.begin(); octave_it != gauss_pyramid.end(); ++octave_it) {
        octave_it->level = int(octave_it - gauss_pyramid.begin());

        for (size_t i = 0; i < _s; i++) {
            const auto& layer = octave_it->layers.back();
            octave_it->layers.emplace_back(layer.current_sigma * k, layer.effective_sigma * k,
                                           applySeparableFilter(layer.image, filters[i]));
        }

        if (octave_it->level != int(_n - 1)) {
            const auto& layer = octave_it->layers.back();
            (octave_it + 1)->layers.emplace_back(_sigma_0, layer.effective_sigma, getDownscale(layer.image));
        }
    }

    if (_from_octave_minus_one) {
        for (auto& octave : gauss_pyramid) {
            octave.level--;
            for (auto& layer : octave.layers) {
                layer.effective_sigma /= 2;
            }
        }
    }

    return gauss_pyramid;
}

double getL(const PyramidT& _pyramid, int _y, int _x, double _sigma) {
    auto comp = [_sigma](const auto& _first_layer, const auto& _second_layer) {
        return fabs(_first_layer.effective_sigma - _sigma) < fabs(_second_layer.effective_sigma - _sigma);
    };

    auto target_layer = _pyramid.front().layers.begin();
    int octave_level = _pyramid.front().level;
    for (const auto& octave : _pyramid) {
        auto layer = std::min_element(octave.layers.begin(), octave.layers.end(), comp);
        if (comp(*layer, *target_layer)) {
            target_layer = layer;
            octave_level = octave.level;
        }
    }

    auto row = _y / pow(2.0, octave_level);
    auto col = _x / pow(2.0, octave_level);

    return target_layer->image.get(row, col);
}

void savePyramid(const PyramidT& _pyramid, const std::string& _path) {
    int num = 0;
    for (const auto& octave : _pyramid) {
        for (const auto& layer : octave.layers) {
            auto name = _path +
                    std::to_string(++num) + "_img_" +
                    std::to_string(octave.level) + "_" +
                    std::to_string(layer.current_sigma) + "_" +
                    std::to_string(layer.effective_sigma) + ".png";
            qimg::toQImagePtr(layer.image)->save(name.c_str());
        }
    }
}

} // mycv
