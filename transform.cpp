#include "transform.h"

#include <cassert>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

namespace mycv {

namespace transform {

TransformationT getTransformation(const desc::BlobsT& _first, const desc::BlobsT& _second) {
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

} // transform

} // mycv
