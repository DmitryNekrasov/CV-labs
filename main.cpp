#include <iostream>

#include <QPainter>

#include "array.h"
#include "core.h"
#include "defs.h"
#include "descriptors.h"
#include "matrix.h"
#include "myimage.h"
#include "poi.h"
#include "qimagegenerator.h"
#include "simple.h"

int main() {

    QImage qimage_in("/Users/ScanNorOne/Desktop/lena_std.tif");
    mycv::CMyImage image_in(qimage_in);

    auto harris = mycv::poi::applyHarris(image_in, 3);
    auto poi = mycv::poi::getPoi(harris, 1.0, 5);
    auto filtered_poi = mycv::poi::filterPoint(poi, 100);

    std::cout << "Number of Harris poi: " << poi.size() << std::endl;
    std::cout << "Number of filtered Harris poi: " << filtered_poi.size() << std::endl;

    auto descriptors = mycv::desc::getDescriptors(image_in, filtered_poi, 4, 4, 8);

    harris.normalize();

    mycv::qimg::toQImagePtr(harris)->save("/Users/ScanNorOne/Desktop/harris.png");
    mycv::qimg::drawPoints(image_in, poi)->save("/Users/ScanNorOne/Desktop/poi.png");
    mycv::qimg::drawPoints(image_in, filtered_poi)->save("/Users/ScanNorOne/Desktop/filtered_poi.png");

    return 0;
}
