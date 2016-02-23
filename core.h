#ifndef CORE_H
#define CORE_H

#include "matrix.h"
#include "myimage.h"

namespace mycv {

CMyImage applyConvolution(const CMyImage& _image, const CMatrix& _kernel);

} // mycv

#endif // CORE_H
