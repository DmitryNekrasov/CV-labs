#include <ctime>
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

    QImage first_qimage_in("/Users/ScanNorOne/Desktop/msu1.png");
    mycv::CMyImage first_image_in(first_qimage_in);
    QImage second_qimage_in("/Users/ScanNorOne/Desktop/msu2.png");
    mycv::CMyImage second_image_in(second_qimage_in);

    int w_size = 3;
    double threshold = 5.0;
    int p_size = 5;
    size_t target_quantity = 500;

    auto first_harris = mycv::poi::applyHarris(first_image_in, w_size);
    auto first_poi = mycv::poi::getPoi(first_harris, threshold, p_size);
    auto first_filtered_poi = mycv::poi::filterPoint(first_poi, target_quantity);

    auto second_harris = mycv::poi::applyHarris(second_image_in, w_size);
    auto second_poi = mycv::poi::getPoi(second_harris, threshold, p_size);
    auto second_filtered_poi = mycv::poi::filterPoint(second_poi, target_quantity);

    std::cout << "First filtered point count: " << first_filtered_poi.size() << std::endl;
    std::cout << "Second filtered point count: " << second_filtered_poi.size() << std::endl;

    int descriptor_size = 4;
    int block_size = 4;
    int histogram_value_number = 8;

    auto first_descriptors = mycv::desc::getDescriptors(first_image_in, first_filtered_poi,
                                                        descriptor_size, block_size, histogram_value_number);
    auto second_descriptors = mycv::desc::getDescriptors(second_image_in, second_filtered_poi,
                                                        descriptor_size, block_size, histogram_value_number);

    auto matches = mycv::desc::getMatches(first_descriptors, second_descriptors);

    std::cout << matches.size() << std::endl;

    first_harris.normalize();
    second_harris.normalize();

    mycv::qimg::toQImagePtr(first_harris)->save("/Users/ScanNorOne/Desktop/first_harris.png");
    mycv::qimg::drawPoints(first_image_in, first_poi)->save("/Users/ScanNorOne/Desktop/first_poi.png");
    mycv::qimg::drawPoints(first_image_in, first_filtered_poi)->save("/Users/ScanNorOne/Desktop/first_filtered_poi.png");

    mycv::qimg::toQImagePtr(second_harris)->save("/Users/ScanNorOne/Desktop/second_harris.png");
    mycv::qimg::drawPoints(second_image_in, second_poi)->save("/Users/ScanNorOne/Desktop/second_poi.png");
    mycv::qimg::drawPoints(second_image_in, second_filtered_poi)->save("/Users/ScanNorOne/Desktop/second_filtered_poi.png");

    mycv::qimg::mergeImageAndDrawLines(first_image_in, first_filtered_poi, second_image_in, second_filtered_poi,
                                       matches, 128)->save("/Users/ScanNorOne/Desktop/merged.png");

    return 0;
}
