#include "transform.h"

#include <cassert>
#include <cmath>

#include <random>
#include <unordered_map>
#include <vector>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

#include "simple.h"
#include "space.h"

namespace mycv {

namespace transform {

static TransformationT getTransformation(const desc::BlobsT& _first, const desc::BlobsT& _second) {
    assert(_first.size() == _second.size());

    TransformationT transformation;

    auto n = _first.size();

    auto* a = gsl_matrix_alloc(2 * n, transformation.size());

    for (size_t i = 0; i < n; i++) {
        const auto& p1 = _first[i];
        const auto& p2 = _second[i];
        TransformationT even{ { double(p1.x), double(p1.y), 1, 0, 0, 0, double(-p2.x) * p1.x, double(-p2.x) * p1.y, double(-p2.x) } };
        TransformationT odd{ { 0, 0, 0, double(p1.x), double(p1.y), 1, double(-p2.y) * p1.x, double(-p2.y) * p1.y, double(-p2.y) } };
        for (size_t j = 0, ej = transformation.size(); j < ej; j++) {
            gsl_matrix_set(a, i * 2, j, even[j]);
            gsl_matrix_set(a, i * 2 + 1, j, odd[j]);
        }
    }

    using MatrixMemoryT = std::array<double, transformation.size() * transformation.size()>;

    MatrixMemoryT at_a_memory;
    auto at_a = gsl_matrix_view_array(at_a_memory.begin(), transformation.size(), transformation.size()).matrix;

    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1, a, a, 0, &at_a);

    MatrixMemoryT v_memory;
    auto v = gsl_matrix_view_array(v_memory.begin(), transformation.size(), transformation.size()).matrix;

    TransformationT s_memory, work_memory;
    auto s = gsl_vector_view_array(s_memory.begin(), s_memory.size()).vector;
    auto work = gsl_vector_view_array(work_memory.begin(), work_memory.size()).vector;

    gsl_linalg_SV_decomp(&at_a, &v, &s, &work);

    for (size_t i = 0, ei = transformation.size(); i < ei; i++) {
        transformation[i] = gsl_matrix_get(&v, i, transformation.size() - 1);
    }

    gsl_matrix_free(a);

    return transformation;
}

TransformationT ransac(const desc::BlobsT& _first, const desc::BlobsT& _second, const desc::MatchesT& _matches,
                       size_t _iter_count, double _threshold)
{
    auto n = _matches.size();

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<size_t> random_int(0, n - 1);

    const size_t points_count = 4;

    desc::BlobsT left(points_count);
    desc::BlobsT right(points_count);

    TransformationT result;
    size_t max_inliers_count = 0;

    size_t iter;
    for (iter = 0; iter < _iter_count && max_inliers_count < n / 2; iter++) {
        for (size_t i = 0; i < points_count; i++) {
            auto rand = random_int(mt);
            left[i] = _first[_matches[rand].first];
            right[i] = _second[_matches[rand].second];
        }

        auto h = getTransformation(left, right);
        size_t inliers_count = 0;
        for (const auto& match : _matches) {
            const auto& p = _first[match.first];
            auto den = h[6] * p.x + h[7] * p.y + h[8];
            auto x_hatch = (h[0] * p.x + h[1] * p.y + h[2]) / den;
            auto y_hatch = (h[3] * p.x + h[4] * p.y + h[5]) / den;
            const auto& second_point = _second[match.second];
            auto dx = second_point.x - x_hatch;
            auto dy = second_point.y - y_hatch;
            if (smpl::sqr(dx) + smpl::sqr(dy) < _threshold) {
                inliers_count++;
            }
        }

        if (inliers_count > max_inliers_count) {
            max_inliers_count = inliers_count;
            result = h;
        }
    }

    std::cout << "RANSAC iter count: " << iter << std::endl;

    return result;
}

TransformationT hough(const CMyImage& _first_image, const CMyImage& _second_image,
                      const desc::BlobsT& _first_blobs, const desc::BlobsT& _second_blobs,
                      const desc::AnglesT& _first_angles, const desc::AnglesT& _second_angles,
                      const desc::MatchesT& _matches)
{
    size_t x_size = 100;
    size_t y_size = 100;
    size_t scale_size = 30;
    size_t angle_size = 16;

    int x_min = -3000;
    int x_max = _second_image.getHeight() + 3000;
    int y_min = -3000;
    int y_max = _second_image.getWidth() + 3000;
    double min_scale = pow(2, -8);
    double max_scale = pow(2, 8);

    CSpace space(x_size, y_size, scale_size, angle_size);

    std::unordered_map<size_t, std::vector<size_t>> map;

    int center_x = _first_image.getHeight() / 2;
    int center_y = _first_image.getWidth() / 2;

    for (size_t match_index = 0; match_index < _matches.size(); match_index++) {
        const auto& match = _matches[match_index];
        const auto& first_point = _first_blobs[match.first];
        const auto& first_angle = _first_angles[match.first];
        const auto& second_point = _second_blobs[match.second];
        const auto& second_angle = _second_angles[match.second];

        auto dx = center_x - first_point.x;
        auto dy = center_y - first_point.y;

        auto x1 = (dx * cos(-first_angle) - dy * sin(-first_angle)) / first_point.sigma;
        auto y1 = (dx * sin(-first_angle) + dy * cos(-first_angle)) / first_point.sigma;

        auto x2 = (x1 * cos(second_angle) - y1 * sin(second_angle)) * second_point.sigma;
        auto y2 = (x1 * sin(second_angle) + y1 * cos(second_angle)) * second_point.sigma;

        auto target_x = x2 + second_point.x;
        auto target_y = y2 + second_point.y;
        auto target_scale = second_point.sigma / first_point.sigma;
        auto target_angle = fmod(fmod(second_angle - first_angle, M_PI * 2) + M_PI * 2, M_PI * 2);

        auto quantum_x = size_t((target_x - x_min) * x_size / (x_max - x_min));
        auto quantum_y = size_t((target_y - y_min) * y_size / (y_max - y_min));
        auto quantum_scale = size_t(log2(target_scale / min_scale) * scale_size / log2(max_scale / min_scale));
        auto quantum_angle = std::min(size_t((target_angle * angle_size) / (2 * M_PI)), angle_size - 1);

        for (size_t x = quantum_x; x <= quantum_x + 1; x++) {
            for (size_t y = quantum_y; y <= quantum_y + 1; y++) {
                for (size_t scale = quantum_scale; scale <= quantum_scale + 1; scale++) {
                    for (size_t angle = quantum_angle; angle <= quantum_angle + 1; angle++) {
                        auto index = space.getIndex(x, y, scale, angle % angle_size);
                        space.increase(index);
                        map[index].push_back(match_index);
                    }
                }
            }
        }
    }

    auto max_it = std::max_element(space.begin(), space.end());
    auto max_index = size_t(max_it - space.begin());
    std::cout << "Hough max: " << *max_it << ". Index: " << max_index << std::endl;

    desc::BlobsT first_blobs, second_blobs;

    for (auto match_index : map[max_index]) {
        const auto& match = _matches[match_index];
        first_blobs.push_back(_first_blobs[match.first]);
        second_blobs.push_back(_second_blobs[match.second]);
    }

    return getTransformation(first_blobs, second_blobs);
}

} // transform

} // mycv
