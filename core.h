#ifndef CORE_H
#define CORE_H

#include "matrix.h"
#include "myimage.h"

namespace mycv {

CMyImage    applyConvolution(const CMyImage& _image, const CMatrix& _kernel);
double      getGradient(double _x, double _y);
CMyImage    getSobel(const CMyImage& _dx, const CMyImage& _dy);

} // mycv

#endif // CORE_H
