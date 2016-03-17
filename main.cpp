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

    QImage qimage_in("/Users/ScanNorOne/Desktop/lena_std.tif");
    mycv::CMyImage image_in(qimage_in);

    auto moravec = mycv::poi::applyMoravec(image_in);
    auto moravec_poi = mycv::poi::getPoi(moravec, 0.095);
    auto moravec_with_points = mycv::qimg::drawPoints(image_in, moravec_poi);

    std::cout << "Number of Moravec poi: " << moravec_poi.size() << std::endl;

    auto harris = mycv::poi::applyHarris(image_in);
    auto harris_poi = mycv::poi::getPoi(harris, 1.0);
    auto harris_with_points = mycv::qimg::drawPoints(image_in, harris_poi);

    std::cout << "Number of Harris poi: " << harris_poi.size() << std::endl;

    moravec.normalize();
    moravec.toQImagePtr()->save("/Users/ScanNorOne/Desktop/moravec.png");
    moravec_with_points->save("/Users/ScanNorOne/Desktop/moravec_poi.png");

    harris.normalize();
    harris.toQImagePtr()->save("/Users/ScanNorOne/Desktop/harris.png");
    harris_with_points->save("/Users/ScanNorOne/Desktop/harris_poi.png");

    return 0;
}
