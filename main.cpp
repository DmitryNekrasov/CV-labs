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

    auto moravec = mycv::poi::applyMoravec(image_in, 3);
    auto moravec_poi = mycv::poi::getPoi(moravec, 0.15, 5);
    auto filtered_morave_poi = mycv::poi::filterPoint(moravec_poi, 100);
    auto moravec_with_points = mycv::qimg::drawPoints(image_in, moravec_poi);
    auto moravec_with_filtered_points = mycv::qimg::drawPoints(image_in, filtered_morave_poi);

    std::cout << "Number of Moravec poi: " << moravec_poi.size() << std::endl;
    std::cout << "Number of filtered Moravec poi: " << filtered_morave_poi.size() << std::endl;

    auto harris = mycv::poi::applyHarris(image_in, 3);
    auto harris_poi = mycv::poi::getPoi(harris, 1.0, 5);
    auto filtered_harris_poi = mycv::poi::filterPoint(harris_poi, 100);
    auto harris_with_points = mycv::qimg::drawPoints(image_in, harris_poi);
    auto harris_with_filtered_points = mycv::qimg::drawPoints(image_in, filtered_harris_poi);

    std::cout << "Number of Harris poi: " << harris_poi.size() << std::endl;
    std::cout << "Number of filtered Harris poi: " << filtered_harris_poi.size() << std::endl;

    mycv::qimg::toQImagePtr(moravec)->save("/Users/ScanNorOne/Desktop/moravec.png");
    moravec_with_points->save("/Users/ScanNorOne/Desktop/moravec_poi.png");
    moravec_with_filtered_points->save("/Users/ScanNorOne/Desktop/moravec_filtered_poi.png");

    harris.normalize();
    mycv::qimg::toQImagePtr(harris)->save("/Users/ScanNorOne/Desktop/harris.png");
    harris_with_points->save("/Users/ScanNorOne/Desktop/harris_poi.png");
    harris_with_filtered_points->save("/Users/ScanNorOne/Desktop/harris_filtered_poi.png");

    return 0;
}
