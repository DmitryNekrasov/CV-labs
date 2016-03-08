#pragma once

namespace mycv {

namespace smpl {

double  convertToDouble(int _intensity);
int     convertToInt(double _intensity);

double  gauss(int _x, int _y, double _sigma);
int     getGaussKernelSize(double _sigma);
double  getSigmaB(double _sigma_c, double _sigma_a);

} // smpl

} // mycv
