#include "simple.h"

#include <cmath>

namespace mycv {

namespace smpl {

double gauss(int _x, int _y, double _sigma) {
    double two_sigma_sqr = 2 * _sigma * _sigma;
    return 1.0 / (M_PI * two_sigma_sqr) * exp(-(_x * _x + _y * _y) / two_sigma_sqr);
}

int getGaussKernelSize(double _sigma) {
    return int(std::ceil(3 * _sigma)) * 2 + 1;
}

double getSigmaForKernelSize(int _size) {
    return double(_size - 1) / 6;
}

double getSigmaB(double _sigma_c, double _sigma_a) {
    return sqrt(_sigma_c * _sigma_c - _sigma_a * _sigma_a);
}

double sqr(double _x) {
    return _x * _x;
}

int sqr(int _x) {
    return _x * _x;
}

double getDistance(int _x1, int _y1, int _x2, int _y2) {
    return sqrt(sqr(_x1 - _x2) + sqr(_y1 - _y2));
}

int modulo(int _x, int _mod) {
    return (_x % _mod + _mod) % _mod;
}

} // smpl

} // mycv
