#include "descriptors.h"

#include "numeric"

#include "core.h"
#include "simple.h"

namespace mycv {

namespace desc {

static double getVectorLength(const DescriptorT& _descriptor) {
    return sqrt(std::accumulate(_descriptor.begin(), _descriptor.end(), 0.0,
        [](const auto& _sum, double _value) {
            return _sum + smpl::sqr(_value);
        }));
}

DescriptorsT getDescriptors(const CMyImage& _image, const poi::PointsT& _points,
                            int _descriptor_size, int _block_size, int _histogram_value_number)
{
    auto sobel_dx = getSobelDx(_image);
    auto sobel_dy = getSobelDy(_image);
    auto gradient_values = getGradientValues(sobel_dx, sobel_dy);
    auto gradient_directions = getGradientDirections(sobel_dx, sobel_dy);

    int histogram_number = smpl::sqr(_descriptor_size);
    int descriptor_value_number = histogram_number * _histogram_value_number;
    int grid_size = _descriptor_size * _block_size;
    int grid_half = grid_size / 2;

    auto gauss_kernel = getGaussKernel(smpl::getSigmaForKernelSize(grid_half * 2 + 1));

    DescriptorsT descriptors;

    const auto z = M_PI * 2 / _histogram_value_number;

    for (const auto& point : _points) {

        descriptors.emplace_back(descriptor_value_number, 0);
        auto& descriptor = descriptors.back();

        for (int i = 0; i < grid_size; i++) {
            for (int j = 0; j < grid_size; j++) {
                auto u = std::get<toUType(Poi::X)>(point) - grid_half + i;
                auto v = std::get<toUType(Poi::Y)>(point) - grid_half + j;

                auto gradient_value = gradient_values.get(u, v) * gauss_kernel.get(i, j);
                auto gradient_direction = gradient_directions.get(u, v);

                auto first_bin_index = int(gradient_direction / z);
                auto distance_to_bin_center = gradient_direction - (first_bin_index * z + z / 2);
                auto second_bin_index = distance_to_bin_center > 0 ? first_bin_index + 1 : first_bin_index - 1;

                first_bin_index %= _histogram_value_number;
                second_bin_index = smpl::modulo(second_bin_index, _histogram_value_number);

                auto histogram_start_index = (i / _block_size * _descriptor_size + j / _block_size) * _histogram_value_number;

                auto second_percent = fabs(distance_to_bin_center) / z;
                auto first_percent = 1 - second_percent;

                descriptor[size_t(histogram_start_index + first_bin_index)] += first_percent * gradient_value;
                descriptor[size_t(histogram_start_index + second_bin_index)] += second_percent * gradient_value;
            }
        }
    }

    for (auto& descriptor : descriptors) {
        auto vector_length = getVectorLength(descriptor);
        std::transform(descriptor.begin(), descriptor.end(), descriptor.begin(),
            [vector_length](const auto& _value) {
                return _value / vector_length;
            });
    }

    return descriptors;
}

static double getDistance(const DescriptorT& _first, const DescriptorT& _second) {
    double sum = 0;
    for (size_t i = 0, ei = _first.size(); i < ei; i++) {
        sum += smpl::sqr(_first[i] - _second[i]);
    }
    return sqrt(sum);
}

MatchesT getMatches(const DescriptorsT& _first, const DescriptorsT& _second) {

    MatchesT matches;

    for (size_t i = 0, ei = _first.size(); i < ei; i++) {
        size_t min_index = 0;
        double min_distance = getDistance(_first[i], _second[min_index]);
        for (size_t j = 1, ej = _second.size(); j < ej; j++) {
            auto distance = getDistance(_first[i], _second[j]);
            if (distance < min_distance) {
                min_distance = distance;
                min_index = j;
            }
        }
        if (min_distance < 0.5) {
            matches.emplace_back(i, min_index);
        }
    }

    return matches;
}

} // desc

} // mycv
