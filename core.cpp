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
        separable_filter.first[i] = separable_filter.second[i] = sqrt(smpl::gauss(i - half, i - half, _sigma));
    }

    return separable_filter;
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

GaussPyramidT getGaussPyramid(const CMyImage& _image, const int _n, const int _s,
                              const double _sigma_a, const double _sigma_0)
{
    auto sigma_b = smpl::getSigmaB(_sigma_0, _sigma_a);
    auto gauss = getGaussSeparable(sigma_b);

    GaussPyramidT gauss_pyramid;
    gauss_pyramid.push_back(std::make_tuple(_sigma_0, _sigma_0, applySeparableFilter(_image, gauss)));

    auto k = pow(2.0, 1.0 / _s);
    std::vector<SeparableFilterT> filters;

    auto old_sigma = _sigma_0;
    for (int i = 0; i < _s; i++) {
        auto new_sigma = old_sigma * k;
        auto sigma = smpl::getSigmaB(new_sigma, old_sigma);
        filters.push_back(getGaussSeparable(sigma));
        old_sigma = new_sigma;
    }

    for (int octave = 0; octave < _n; octave++) {
        for (int i = 0; i < _s; i++) {

            const auto& layer = gauss_pyramid.back();

            auto last_current_sigma = std::get<toUType(PyramidLayer::CurrentSigma)>(layer);
            auto last_effective_sigma = std::get<toUType(PyramidLayer::EffectiveSigma)>(layer);
            auto& last_img = std::get<toUType(PyramidLayer::Image)>(layer);

            gauss_pyramid.push_back(std::make_tuple(last_current_sigma * k, last_effective_sigma * k,
                                                    applySeparableFilter(last_img, filters[i])));
        }

        if (octave != _n - 1) {

            const auto& layer = gauss_pyramid.back();

            auto effective_sigma = std::get<toUType(PyramidLayer::EffectiveSigma)>(layer);
            auto& image = std::get<toUType(PyramidLayer::Image)>(layer);

            gauss_pyramid.push_back(std::make_tuple(_sigma_0, effective_sigma,
                                                    getDownscale(image)));
        }
    }

    return gauss_pyramid;
}

} // mycv
