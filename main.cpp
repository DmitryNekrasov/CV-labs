#include <iostream>

#include "core.h"
#include "defs.h"
#include "matrix.h"
#include "myimage.h"

int main() {

    QImage qimage_in("/Users/ScanNorOne/Desktop/laferrari.jpg");
    CMyImage image_in(qimage_in, BorderEffect::Mirror);

    auto sobel_dx = mycv::getSobelDx(image_in);
    auto sobel_dy = mycv::getSobelDy(image_in);
    auto sobel_avg = mycv::getSobel(sobel_dx, sobel_dy);

    sobel_dx.normalize();
    sobel_dy.normalize();
    sobel_avg.normalize();

    sobel_dx.toQImagePtr()->save("/Users/ScanNorOne/Desktop/sobel_dx.png");
    sobel_dy.toQImagePtr()->save("/Users/ScanNorOne/Desktop/sobel_dy.png");
    sobel_avg.toQImagePtr()->save("/Users/ScanNorOne/Desktop/sobel_avg.png");

    return 0;
}
