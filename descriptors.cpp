#include "descriptors.h"

#include <algorithm>
#include <array>
#include <numeric>

#include "algo.h"
#include "core.h"
#include "pyramid.h"
#include "qimagegenerator.h"
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

static double specifyAngle(double _x0, double _y0, double _x1, double _y1, double _x2, double _y2) {
    auto a = (_y2 - _y0) / ((_x2 - _x0) * (_x2 - _x1)) - (_y1 - _y0) / ((_x1 - _x0) * (_x2 - _x1));
    auto b = (_y1 - _y0) / (_x1 - _x0) - a * (_x1 + _x0);
    return -b / (2 * a);
}

static const int g_MinSize = 16;

std::tuple<DescriptorsT, AnglesT, BlobsT> getDescriptors(const CMyImage& _image, int _descriptor_size, int _histogram_value_number)
{
    size_t s = 8;
    double sigma_a = 0.5;
    double sigma_0 = 1.6;
    auto n = size_t(std::min(std::log2(double(_image.getHeight()) / g_MinSize),
                      std::log2(double(_image.getWidth()) / g_MinSize))) + 1;

    auto gauss_pyramid = getGaussPyramid(_image, n, s, sigma_a, sigma_0);

    auto blobs = getBlobs(gauss_pyramid);

    std::array<double, g_WideHistogramSize> wide_histogram;
    const auto wide_bin_size = M_PI * 2 / g_WideHistogramSize;

    auto get_final_angle = [&](int _max_index) {
        auto angle_center = _max_index * wide_bin_size + wide_bin_size / 2;
        auto angle_left = angle_center - wide_bin_size;
        auto angle_right = angle_center + wide_bin_size;
        auto y_center = wide_histogram[size_t(_max_index)];
        auto y_left = wide_histogram[size_t(smpl::modulo(_max_index - 1, g_WideHistogramSize))];
        auto y_right = wide_histogram[size_t(smpl::modulo(_max_index + 1, g_WideHistogramSize))];
        if (y_center >= y_left && y_center >= y_right) {
            return specifyAngle(angle_center, y_center, angle_left, y_left, angle_right, y_right);
        }
        return angle_center;
    };

    AnglesT angles;
    AnglesT second_angles;
    BlobsT new_blobs;

    for (const auto& blob : blobs) {
        std::fill(wide_histogram.begin(), wide_histogram.end(), 0);

        const auto& image = blob.layer->image;
        auto grid_size = int(_descriptor_size * (blob.layer->current_sigma / sigma_0));
        auto grid_half = grid_size / 2;

        for (int i = 0; i < grid_size; i++) {
            for (int j = 0; j < grid_size; j++) {
                auto u = blob.x / blob.scale - grid_half + i;
                auto v = blob.y / blob.scale - grid_half + j;

                auto dx = getDx(image, u, v);
                auto dy = getDy(image, u, v);

                auto gradient_value = getGradientValue(dx, dy);
                auto gradient_direction = getGradientDirection(dx, dy);

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

        auto first_max_index = int(two_max.first - wide_histogram.begin());
        auto second_max_index = int(two_max.second - wide_histogram.begin());

        auto first_angle = get_final_angle(first_max_index);
        auto second_angle = get_final_angle(second_max_index);

        angles.push_back(first_angle);

        if (*two_max.second >= *two_max.first * 0.8 && fabs(first_angle - second_angle) > 2 * M_PI / g_WideHistogramSize) {
            new_blobs.emplace_back(blob);
            second_angles.push_back(second_angle);
        }
    }

    blobs.insert(blobs.end(), new_blobs.begin(), new_blobs.end());
    angles.insert(angles.end(), second_angles.begin(), second_angles.end());

    int histogram_number = 16;
    int descriptor_value_number = histogram_number * _histogram_value_number;

    DescriptorsT descriptors;

    const auto z = M_PI * 2 / _histogram_value_number;

    size_t angle_index = 0;

    const auto block_count = 4;

    for (const auto& blob : blobs) {
        const auto& image = blob.layer->image;
        auto grid_size = int(_descriptor_size * (blob.layer->current_sigma / sigma_0));
        auto grid_half = grid_size / 2;
        auto block_size = double(grid_size) / block_count;

        descriptors.emplace_back(descriptor_value_number, 0);
        auto& descriptor = descriptors.back();

        const auto angle = angles[angle_index++];
        const auto gap = int(grid_half * (sqrt(2.0) - 1.0));

        for (int i = -gap, ei = grid_size + gap; i < ei; i++) {
            for (int j = -gap, ej = grid_size + gap; j < ej; j++) {
                auto u = blob.x / blob.scale - grid_half + i;
                auto v = blob.y / blob.scale - grid_half + j;

                auto dx = getDx(image, u, v);
                auto dy = getDy(image, u, v);

                auto gradient_value = getGradientValue(dx, dy);
                auto gradient_direction = getGradientDirection(dx, dy) - angle;

                auto new_i = int((i - grid_half) * cos(angle) - (j - grid_half) * sin(angle));
                auto new_j = int((i - grid_half) * sin(angle) + (j - grid_half) * cos(angle));

                new_i += grid_half;
                new_j += grid_half;

                if (new_i >= 0 && new_i < grid_size && new_j >= 0 && new_j < grid_size) {

                    auto histogram_x = int(round(new_i / block_size));
                    auto histogram_y = int(round(new_j / block_size));
                    auto histogram_angle = int(round(gradient_direction / z));

                    for (int x = histogram_x - 1; x <= histogram_x; x++) {
                        if (x >= 0 && x < block_count) {
                            auto dist_to_center_x = fabs(new_i - (block_size * x + block_size / 2));
                            auto percent_x = 1 - (((x == 0 && histogram_x == 0) || (histogram_x == block_count)) ?
                                                      0 : dist_to_center_x / block_size);
                            for (int y = histogram_y - 1; y <= histogram_y; y++) {
                                if (y >= 0 && y < block_count) {
                                    auto dist_to_center_y = fabs(new_j - (block_size * y + block_size / 2));
                                    auto percent_y = 1 - (((y == 0 && histogram_y == 0) || (histogram_y == block_count)) ?
                                                              0 : dist_to_center_y / block_size);
                                    for (int d = histogram_angle - 1; d <= histogram_angle; d++) {
                                        auto bin_index = smpl::modulo(d, _histogram_value_number);
                                        auto dist_to_center_bin = fabs(gradient_direction - (z * d + z / 2));
                                        auto percent_angle = 1 - dist_to_center_bin / z;
                                        auto index = (block_count * x + y) * _histogram_value_number + bin_index;
                                        descriptor[size_t(index)] += gradient_value * percent_x * percent_y * percent_angle;
                                    }
                                }
                            }
                        }
                    }

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

    return std::make_tuple(descriptors, angles, blobs);
}

static double getDistance(const DescriptorT& _first, const DescriptorT& _second) {
    double sum = 0;
    for (size_t i = 0, ei = _first.size(); i < ei; i++) {
        sum += smpl::sqr(_first[i] - _second[i]);
    }
    return sqrt(sum);
}

static bool isExtremum(const Octave& _octave, size_t _k, int _i, int _j) {
    auto center = _octave.layers[_k].image.get(_i, _j);
    if (fabs(center) < 0.035) {
        return false;
    }

    static constexpr size_t extremum_array_size = 26;
    std::array<double, extremum_array_size> intensities;
    size_t index = 0;

    for (size_t k = _k - 1; k <= _k + 1; k++) {
        for (int i = _i - 1; i <= _i + 1; i++) {
            for (int j = _j - 1; j <= _j + 1; j++) {
                if (k != _k || i != _i || j != _j) {
                    intensities[index++] = _octave.layers[k].image.get(i, j);
                }
            }
        }
    }

    auto minmax = std::minmax_element(intensities.begin(), intensities.end());

    return center < *minmax.first || center > *minmax.second;
}

static const double g_R = 10;

BlobsT getBlobs(const PyramidT& _pyramid) {
    auto dog = getDog(_pyramid);

    BlobsT blobs;

    int scale = 1;
    size_t octave_number = 0;
    for (const auto& octave : dog) {
        for (size_t k = 1, ek = octave.layers.size() - 1; k < ek; k++) {
            const auto& layer = octave.layers[k];
            for (int i = 0, ei = layer.image.getHeight(); i < ei; i++) {
                for (int j = 0, ej = layer.image.getWidth(); j < ej; j++) {
                    if (isExtremum(octave, k, i, j)) {
                        auto a = getDx2(layer.image, i, j);
                        auto b = getDxDy(layer.image, i, j);
                        auto c = getDy2(layer.image, i, j);
                        auto det = a * c - smpl::sqr(b);
                        auto trace = a + c;
                        if (smpl::sqr(trace) / det <= smpl::sqr(g_R + 1) / g_R) {
                            const auto& gauss_layer = _pyramid[octave_number].layers[k];
                            blobs.emplace_back(round((i + 0.5) * scale - 0.5), round((j + 0.5) * scale - 0.5),
                                               layer.effective_sigma, &gauss_layer, scale);
                        }
                    }
                }
            }
        }
        scale *= 2;
        octave_number++;
    }

    return blobs;
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
