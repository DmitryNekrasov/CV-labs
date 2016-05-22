#include "core.h"

#include <array>
#include <numeric>

#include "simple.h"

namespace mycv {

static const int sobel_kernel_size = 3;

static const double sobel_kernel_x_array[] = {
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1
};

static const double sobel_kernel_y_array[] = {
    -1, -2, -1,
     0,  0,  0,
     1,  2,  1
};

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

CMyImage applySeparableFilter(const CMyImage& _image, const SeparableFilterT& _filter) {

    CMyImage temp_image(_image.getHeight(), _image.getWidth());

    int half = _filter.first.getSize() / 2;

    for (int i = 0, ei = _image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = _image.getWidth(); j < ej; j++) {
            double result_intensity = 0;
            for (int y = 0, ey = _filter.first.getSize(); y < ey; y++) {
                auto intensity = _image.get(i, j + y - half);
                result_intensity += intensity * _filter.first[y];
            }
            temp_image.set(i, j, result_intensity);
        }
    }

    CMyImage result_image(temp_image.getHeight(), temp_image.getWidth());

    for (int i = 0, ei = temp_image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = temp_image.getWidth(); j < ej; j++) {
            double result_intensity = 0;
            for (int x = 0, ex = _filter.second.getSize(); x < ex; x++) {
                auto intensity = temp_image.get(i + x - half, j);
                result_intensity += intensity * _filter.second[x];
            }
            result_image.set(i, j, result_intensity);
        }
    }

    return result_image;
}

double getGradientValue(double _gx, double _gy) {
    return sqrt(smpl::sqr(_gx) + smpl::sqr(_gy));
}

double getGradientDirection(double _gx, double _gy) {
    auto angle = atan2(_gy, _gx);
    return angle >= 0 ? angle : angle + M_PI * 2;
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

CMyImage getGradientValues(const CMyImage& _dx, const CMyImage& _dy) {

    CMyImage result_image(_dx.getHeight(), _dx.getWidth());

    for (int i = 0, ei = result_image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = result_image.getWidth(); j < ej; j++) {
            auto gradient = getGradientValue(_dx.get(i, j), _dy.get(i, j));
            result_image.set(i, j, gradient);
        }
    }

    return result_image;
}

CMyImage getGradientDirections(const CMyImage& _dx, const CMyImage& _dy) {

    CMyImage result_image(_dx.getHeight(), _dx.getWidth());

    for (int i = 0, ei = result_image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = result_image.getWidth(); j < ej; j++) {
            auto direction = getGradientDirection(_dx.get(i, j), _dy.get(i, j));
            result_image.set(i, j, direction);
        }
    }

    return result_image;
}

CMatrix getGaussKernel(double _sigma) {

    int kernel_size = smpl::getGaussKernelSize(_sigma);
    CMatrix kernel(kernel_size, kernel_size);

    int half = kernel_size / 2;

    for (int i = 0, ei = kernel.getHeight(); i < ei; i++) {
        for (int j = 0, ej = kernel.getWidth(); j < ej; j++) {
            kernel.set(i, j, smpl::gauss(i - half, j - half, _sigma));
        }
    }

    return kernel;
}

SeparableFilterT getGaussSeparable(const CMatrix& _gauss_kernel) {

    int kernel_size = _gauss_kernel.getHeight();
    auto separable_filter = std::make_pair<CArray, CArray>(kernel_size, kernel_size);

    for (int i = 0; i < kernel_size; i++) {
        separable_filter.first[i] = separable_filter.second[i] = sqrt(_gauss_kernel.get(i, i));
    }

    return separable_filter;
}

SeparableFilterT getGaussSeparable(double _sigma) {

    int kernel_size = smpl::getGaussKernelSize(_sigma);
    auto separable_filter = std::make_pair<CArray, CArray>(kernel_size, kernel_size);

    int half = kernel_size / 2;

    for (int i = 0; i < kernel_size; i++) {
        separable_filter.first[i] = sqrt(smpl::gauss(i - half, i - half, _sigma));
    }

    auto sum = std::accumulate(separable_filter.first.begin(), separable_filter.first.end(), 0.0, std::plus<double>());
    std::transform(separable_filter.first.begin(), separable_filter.first.end(), separable_filter.first.begin(),
                    [sum](const auto& _intensity) {
                        return _intensity / sum;
                    });

    for (int i = 0; i < kernel_size; i++) {
        separable_filter.second[i] = separable_filter.first[i];
    }

    return separable_filter;
}

CMyImage getDownscale(const CMyImage& _image) {

    CMyImage result_image(_image.getHeight() / 2, _image.getWidth() / 2);

    for (int i = 0, ei = result_image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = result_image.getWidth(); j < ej; j++) {
            auto intensity = _image.get(i * 2, j * 2) +
                    _image.get(i * 2 + 1, j * 2) +
                    _image.get(i * 2, j * 2 + 1) +
                    _image.get(i * 2 + 1, j * 2 + 1);
            result_image.set(i, j, intensity / 4);
        }
    }

    return result_image;
}

CMyImage getUpscale(const CMyImage& _image) {

    CMyImage result_image(_image.getHeight() * 2, _image.getWidth() * 2);

    for (int i = 0, ei = _image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = _image.getWidth(); j < ej; j++) {
            result_image.set(i * 2, j * 2, _image.get(i, j));
            result_image.set(i * 2, j * 2 + 1, _image.get(double(i), j + 0.5));
            result_image.set(i * 2 + 1, j * 2, _image.get(i + 0.5, double(j)));
            result_image.set(i * 2 + 1, j * 2 + 1, _image.get(i + 0.5, j + 0.5));
        }
    }

    return result_image;
}

static double getDerivative(double _v1, double _v2) {
    return (-_v1 + _v2) / 2;
}

double getDx(const CMyImage& _image, int _row, int _col) {
    return getDerivative(_image.get(_row, _col - 1), _image.get(_row, _col + 1));
}

double getDy(const CMyImage& _image, int _row, int _col) {
    return getDerivative(_image.get(_row - 1, _col), _image.get(_row + 1, _col));
}

static const std::array<double, 5> g_SecondDerivative{ {0.232905, 0.002668, -0.471147, 0.002668, 0.232905} };

double getDx2(const CMyImage& _image, int _row, int _col) {
    double sum = 0;
    auto half = int(g_SecondDerivative.size()) / 2;
    for (size_t i = 0, ei = g_SecondDerivative.size(); i < ei; i++) {
        sum += g_SecondDerivative[i] * _image.get(_row, _col - half + int(i));
    }
    return sum;
}

double getDy2(const CMyImage& _image, int _row, int _col) {
    double sum = 0;
    auto half = int(g_SecondDerivative.size()) / 2;
    for (size_t i = 0, ei = g_SecondDerivative.size(); i < ei; i++) {
        sum += g_SecondDerivative[i] * _image.get(_row - half + int(i), _col);
    }
    return sum;
}

double getDxDy(const CMyImage& _image, int _row, int _col) {
    auto dx_top = getDx(_image, _row - 1, _col);
    auto dx_bottom = getDx(_image, _row + 1, _col);
    auto dxdy = getDerivative(dx_top, dx_bottom);
    return dxdy;
}

} // mycv
