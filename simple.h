#pragma once

namespace mycv {

namespace smpl {

double gauss(int _x, int _y, double _sigma);
int getGaussKernelSize(double _sigma);
double getSigmaForKernelSize(int _size);
double getSigmaB(double _sigma_c, double _sigma_a);

double sqr(double _x);
int sqr(int _x);

double getDistance(int _x1, int _y1, int _x2, int _y2);

int modulo(int _x, int _mod);

} // smpl

} // mycv
