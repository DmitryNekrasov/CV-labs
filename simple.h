#pragma once

namespace mycv {

namespace smpl {

double  convertToDouble(int _intensity);
int     convertToInt(double _intensity);

double  gauss(int _x, int _y, double _sigma);

} // smpl

} // mycv
