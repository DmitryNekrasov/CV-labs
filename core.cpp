#include <cmath>

#include "core.h"

namespace mycv {

CMyImage applyConvolution(const CMyImage& _image, const CMatrix& _kernel) {

    CMyImage result_image(_image.getHeight(), _image.getWidth());

    int half_height = _kernel.getHeight() / 2;
    int half_width = _kernel.getWidth() / 2;

    for (int i = 0; i < _image.getHeight(); i++) {
        for (int j = 0; j < _image.getWidth(); j++) {
            double result_intensity = 0;
            for (int x = 0; x < _kernel.getHeight(); x++) {
                for (int y = 0; y < _kernel.getWidth(); y++) {
                    auto intensity = _image.get(i + x - half_height, j + y - half_width);
                    result_intensity += _kernel.get(x, y) * intensity;
                }
            }
            result_image.set(i, j, result_intensity);
        }
    }

    return result_image;
}

double getGradient(double _x, double _y) {
    return sqrt(_x * _x + _y * _y);
}

CMyImage getSobel(const CMyImage& _dx, const CMyImage& _dy) {

    CMyImage result_image(_dx.getHeight(), _dx.getWidth());

    for (int i = 0; i < result_image.getHeight(); i++) {
        for (int j = 0; j < result_image.getWidth(); j++) {
            auto gradient = getGradient(_dx.get(i, j), _dy.get(i, j));
            result_image.set(i, j, gradient);
        }
    }

    return result_image;
}

}
