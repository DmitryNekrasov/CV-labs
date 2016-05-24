#include "transform.h"

#include <cassert>

#include <random>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

#include "simple.h"

namespace mycv {

namespace transform {

static TransformationT getTransformation(const desc::BlobsT& _first, const desc::BlobsT& _second) {
    assert(_first.size() == _second.size());

    TransformationT transformation;

    auto n = _first.size();

    gsl_matrix* a = gsl_matrix_alloc(2 * n, transformation.size());

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

    gsl_matrix* at_a = gsl_matrix_alloc(transformation.size(), transformation.size());
    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1, a, a, 0, at_a);

    gsl_matrix* v = gsl_matrix_alloc(transformation.size(), transformation.size());
    gsl_vector* s = gsl_vector_alloc(transformation.size());
    gsl_vector* work = gsl_vector_alloc(transformation.size());
    gsl_linalg_SV_decomp(at_a, v, s, work);

    for (size_t i = 0, ei = transformation.size(); i < ei; i++) {
        transformation[i] = gsl_matrix_get(v, i, transformation.size() - 1);
    }

    gsl_vector_free(work);
    gsl_vector_free(s);
    gsl_matrix_free(v);
    gsl_matrix_free(at_a);
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

} // transform

} // mycv
