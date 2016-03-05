#include <iostream>

#include "array.h"
#include "core.h"
#include "defs.h"
#include "matrix.h"
#include "myimage.h"
#include "simple.h"

int main() {

    QImage qimage_in("/Users/ScanNorOne/Desktop/laferrari.jpg");
    mycv::CMyImage image_in(qimage_in);

    auto upscale = mycv::getUpscale(image_in);

    auto filter = mycv::getGaussSeparable(0.866);
    auto gauss_upscale = mycv::applySeparableFilter(upscale, filter);

    upscale.toQImagePtr()->save("/Users/ScanNorOne/Desktop/upscale.png");
    gauss_upscale.toQImagePtr()->save("/Users/ScanNorOne/Desktop/gauss_upscale.png");

    return 0;
}
