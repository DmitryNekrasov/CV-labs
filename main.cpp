#include <iostream>

#include <QPainter>

#include "array.h"
#include "core.h"
#include "defs.h"
#include "matrix.h"
#include "myimage.h"
#include "poi.h"
#include "qimagegenerator.h"
#include "simple.h"

int main() {

    QImage qimage_in("/Users/ScanNorOne/Desktop/laferrari.jpg");
    mycv::CMyImage image_in(qimage_in);

    auto sobel_dx = mycv::getSobelDx(image_in);
    auto sobel_dy = mycv::getSobelDy(image_in);
    auto gradient_values = mycv::getSobelValue(sobel_dx, sobel_dy);
    auto gradient_directions = mycv::getSobelDirection(sobel_dx, sobel_dy);

    gradient_values.normalize();
    gradient_directions.normalize();

    mycv::qimg::toQImagePtr(gradient_values)->save("/Users/ScanNorOne/Desktop/values.png");
    mycv::qimg::toQImagePtr(gradient_directions)->save("/Users/ScanNorOne/Desktop/directions.png");

    return 0;
}
