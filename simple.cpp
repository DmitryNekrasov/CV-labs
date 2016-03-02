#include <cmath>

#include "simple.h"

namespace mycv {

namespace smpl {

double convertToDouble(int _intensity) {
    return double(_intensity) / 255;
}

int convertToInt(double _intensity) {
    return int(_intensity * 255);
}

double gauss(int _x, int _y, double _sigma) {
    double two_sigma_sqr = 2 * _sigma * _sigma;
    return 1.0 / (M_PI * two_sigma_sqr) * exp(-(_x * _x + _y * _y) / two_sigma_sqr);
}

int getGaussKernelSize(double _sigma) {
    return int(std::ceil(3 * _sigma)) * 2 + 1;
}

} // smpl

} // mycv
