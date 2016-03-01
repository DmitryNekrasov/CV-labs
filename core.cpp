#include "core.h"
#include "simple.h"

namespace mycv {

CMyImage applyConvolution(const CMyImage& _image, const CMatrix& _kernel) {

    CMyImage result_image(_image.getHeight(), _image.getWidth());

    int half_height = _kernel.getHeight() / 2;
    int half_width = _kernel.getWidth() / 2;

    for (int i = 0, ei = _image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = _image.getWidth(); j < ej; j++) {
            double result_intensity = 0;
            for (int x = 0, ex = _kernel.getHeight(); x < ex; x++) {
                for (int y = 0, ey = _kernel.getWidth(); y < ey; y++) {
                    auto intensity = _image.get(i + x - half_height, j + y - half_width);
                    result_intensity += _kernel.get(x, y) * intensity;
                }
            }
            result_image.set(i, j, result_intensity);
        }
    }

    return result_image;
}

CMyImage applySeparableFilter(const CMyImage& _image, const double _filter[], int _size) {

    CMyImage temp_image(_image.getHeight(), _image.getWidth());

    int half = _size / 2;

    for (int i = 0, ei = _image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = _image.getWidth(); j < ej; j++) {
            double result_intensity = 0;
            for (int y = 0; y < _size; y++) {
                auto intensity = _image.get(i, j + y - half);
                result_intensity += _filter[y] * intensity;
            }
            temp_image.set(i, j, result_intensity);
        }
    }

    CMyImage result_image(temp_image.getHeight(), temp_image.getWidth());

    for (int i = 0, ei = temp_image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = temp_image.getWidth(); j < ej; j++) {
            double result_intensity = 0;
            for (int x = 0; x < _size; x++) {
                auto intensity = temp_image.get(i + x - half, j);
                result_intensity += _filter[x] * intensity;
            }
            result_image.set(i, j, result_intensity);
        }
    }

    return result_image;
}

CMyImage getSobelDx(const CMyImage& _image) {
    CMatrix kernel_x(sobel_kernel_size, sobel_kernel_size, sobel_kernel_x_array);
    auto result_image = applyConvolution(_image, kernel_x);
    return result_image;
}


CMyImage getSobelDy(const CMyImage& _image) {
    CMatrix kernel_y(sobel_kernel_size, sobel_kernel_size, sobel_kernel_y_array);
    auto result_image = applyConvolution(_image, kernel_y);
    return result_image;
}

CMyImage getSobel(const CMyImage& _dx, const CMyImage& _dy) {

    CMyImage result_image(_dx.getHeight(), _dx.getWidth());

    for (int i = 0, ei = result_image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = result_image.getWidth(); j < ej; j++) {
            auto gradient = getGradient(_dx.get(i, j), _dy.get(i, j));
            result_image.set(i, j, gradient);
        }
    }

    return result_image;
}

CMatrix getGaussKernel(double _sigma) {

    int kernel_size = int(std::ceil(3 * _sigma)) * 2 + 1;
    CMatrix kernel(kernel_size, kernel_size);

    int half = kernel_size / 2;

    for (int i = 0, ei = kernel.getHeight(); i < ei; i++) {
        for (int j = 0, ej = kernel.getWidth(); j < ej; j++) {
            kernel.set(i, j, smpl::gauss(i - half, j - half, _sigma));
        }
    }

    return kernel;
}

SeparableFilterT getSeparableGauss(const CMatrix& _gauss_kernel) {

    auto kernel_size = _gauss_kernel.getHeight();
    auto _separable_filter = std::make_pair<CArray, CArray>(kernel_size, kernel_size);

    for (int i = 0; i < kernel_size; i++) {
        _separable_filter.first[i] = _separable_filter.second[i] = sqrt(_gauss_kernel.get(i, i));
    }

    return _separable_filter;
}

CMyImage getDownscale(const CMyImage& _image) {

    CMyImage result_image(_image.getHeight() / 2, _image.getWidth() / 2);

    for (int i = 0, ei = result_image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = result_image.getWidth(); j < ej; j++) {
            auto intensity = _image.get(i * 2, j * 2);
            result_image.set(i, j, intensity);
        }
    }

    return result_image;
}

} // mycv
