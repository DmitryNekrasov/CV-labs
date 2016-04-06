#include "descriptors.h"

#include <array>
#include <numeric>

#include "algo.h"
#include "core.h"
#include "simple.h"

namespace mycv {

namespace desc {

static constexpr int g_WideHistogramSize = 36;

static double getVectorLength(const DescriptorT& _descriptor) {
    return sqrt(std::accumulate(_descriptor.begin(), _descriptor.end(), 0.0,
        [](const auto& _sum, double _value) {
            return _sum + smpl::sqr(_value);
        }));
}

DescriptorsT getDescriptors(const CMyImage& _image, poi::PointsT& _points,
                            int _descriptor_size, int _block_size, int _histogram_value_number)
{
    auto sobel_dx = getSobelDx(_image);
    auto sobel_dy = getSobelDy(_image);
    auto gradient_values = getGradientValues(sobel_dx, sobel_dy);
    auto gradient_directions = getGradientDirections(sobel_dx, sobel_dy);

    int grid_size = _descriptor_size * _block_size;
    int grid_half = grid_size / 2;

    auto wide_gauss_kernel = getGaussKernel(smpl::getSigmaForKernelSize(grid_half * 2 + 1));
    const auto wide_bin_size = M_PI * 2 / g_WideHistogramSize;

    AnglesT angles;
    AnglesT second_angles;
    poi::PointsT new_poi;

    for (const auto& point : _points) {
        std::array<double, g_WideHistogramSize> wide_histogram;
        std::fill(wide_histogram.begin(), wide_histogram.end(), 0);

        for (int i = 0; i < grid_size; i++) {
            for (int j = 0; j < grid_size; j++) {
                auto u = std::get<toUType(Poi::X)>(point) - grid_half + i;
                auto v = std::get<toUType(Poi::Y)>(point) - grid_half + j;

                auto gradient_value = gradient_values.get(u, v) * wide_gauss_kernel.get(i, j);
                auto gradient_direction = gradient_directions.get(u, v);

                auto first_bin_index = int(gradient_direction / wide_bin_size);
                auto distance_to_bin_center = gradient_direction - (first_bin_index * wide_bin_size + wide_bin_size / 2);
                auto second_bin_index = distance_to_bin_center > 0 ? first_bin_index + 1 : first_bin_index - 1;

                first_bin_index %= g_WideHistogramSize;
                second_bin_index = smpl::modulo(second_bin_index, g_WideHistogramSize);

                auto second_percent = fabs(distance_to_bin_center) / wide_bin_size;
                auto first_percent = 1 - second_percent;

                wide_histogram[size_t(first_bin_index)] += first_percent * gradient_value;
                wide_histogram[size_t(second_bin_index)] += second_percent * gradient_value;
            }
        }

        auto two_max = algo::twoElement(wide_histogram.begin(), wide_histogram.end(),
            [](const auto& _first, const auto& _second) {
                return _first > _second;
            });

        auto first_max_index = size_t(two_max.first - wide_histogram.begin());
        auto second_max_index = size_t(two_max.second - wide_histogram.begin());

        auto first_angle = first_max_index * wide_bin_size + wide_bin_size / 2;
        auto second_angle = second_max_index * wide_bin_size + wide_bin_size / 2;

        angles.push_back(first_angle);

        if (second_angle >= first_angle * 0.8) {
            new_poi.emplace_back(point);
            second_angles.push_back(second_angle);
        }
    }

    _points.insert(_points.end(), new_poi.begin(), new_poi.end());
    angles.insert(angles.end(), second_angles.begin(), second_angles.end());

    int histogram_number = smpl::sqr(_descriptor_size);
    int descriptor_value_number = histogram_number * _histogram_value_number;

    DescriptorsT descriptors;

    const auto z = M_PI * 2 / _histogram_value_number;

    size_t angle_index = 0;
    for (const auto& point : _points) {
        descriptors.emplace_back(descriptor_value_number, 0);
        auto& descriptor = descriptors.back();

        const auto angle = angles[angle_index++];

        auto gap = int(grid_half * (sqrt(2.0) - 1.0));

        for (int i = -gap, ei = grid_size + gap; i < ei; i++) {
            for (int j = -gap, ej = grid_size + gap; j < ej; j++) {
                auto u = std::get<toUType(Poi::X)>(point) - grid_half + i;
                auto v = std::get<toUType(Poi::Y)>(point) - grid_half + j;

                auto gradient_value = gradient_values.get(u, v);
                auto gradient_direction = gradient_directions.get(u, v) - angle;

                auto first_bin_index = int(gradient_direction / z);
                auto distance_to_bin_center = gradient_direction - (first_bin_index * z + z / 2);
                auto second_bin_index = distance_to_bin_center > 0 ? first_bin_index + 1 : first_bin_index - 1;

                first_bin_index %= _histogram_value_number;
                second_bin_index = smpl::modulo(second_bin_index, _histogram_value_number);

                auto new_i = int((i - grid_half) * cos(angle) - (j - grid_half) * sin(angle));
                auto new_j = int((i - grid_half) * sin(angle) + (j - grid_half) * cos(angle));

                new_i += grid_half;
                new_j += grid_half;

                if (new_i >= 0 && new_i < grid_size && new_j >= 0 && new_j < grid_size) {
                    auto histogram_start_index = (new_i / _block_size * _descriptor_size + new_j / _block_size) * _histogram_value_number;

                    auto second_percent = fabs(distance_to_bin_center) / z;
                    auto first_percent = 1 - second_percent;

                    descriptor[size_t(histogram_start_index + first_bin_index)] += first_percent * gradient_value;
                    descriptor[size_t(histogram_start_index + second_bin_index)] += second_percent * gradient_value;
                }
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

    std::vector<double> dst_to_second, dst_to_first;
    dst_to_second.reserve(_second.size());
    dst_to_first.reserve(_first.size());

    for (size_t i = 0, ei = _first.size(); i < ei; i++) {
        dst_to_second.clear();
        for (size_t j = 0, ej = _second.size(); j < ej; j++) {
            dst_to_second.push_back(getDistance(_first[i], _second[j]));
        }
        auto two_nearest = algo::twoElement(dst_to_second.begin(), dst_to_second.end(),
            [](const auto& _first, const auto& _second) {
                return _first < _second;
            });
        auto first_min_index = size_t(two_nearest.first - dst_to_second.begin());

        dst_to_first.clear();
        for (size_t j = 0, ej = _first.size(); j < ej; j++) {
            dst_to_first.push_back(getDistance(_second[first_min_index], _first[j]));
        }
        auto second_min_index = size_t(std::min_element(dst_to_first.begin(), dst_to_first.end()) - dst_to_first.begin());

        if (second_min_index == i && *two_nearest.first / *two_nearest.second < 0.8) {
            matches.emplace_back(i, first_min_index);
        }
    }

    return matches;
}

} // desc

} // mycv
