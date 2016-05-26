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
#include "pyramid.h"
#include "qimagegenerator.h"
#include "simple.h"
#include "transform.h"

int main() {

    QImage first_qimage_in("/Users/ScanNorOne/Desktop/boy1.png");
    mycv::CMyImage first_image_in(first_qimage_in);
    QImage second_qimage_in("/Users/ScanNorOne/Desktop/fig1.png");
    mycv::CMyImage second_image_in(second_qimage_in);

    auto first_tuple = mycv::desc::getDescriptors(first_image_in, 16, 8);
    auto second_tuple = mycv::desc::getDescriptors(second_image_in, 16, 8);

    const auto& first_descriptors = std::get<0>(first_tuple);
    const auto& second_descriptors = std::get<0>(second_tuple);

    const auto& first_angles = std::get<1>(first_tuple);
    const auto& second_angles = std::get<1>(second_tuple);

    const auto& first_blobs = std::get<2>(first_tuple);
    const auto& second_blobs = std::get<2>(second_tuple);

    auto matches = mycv::desc::getMatches(first_descriptors, second_descriptors);

    std::cout << matches.size() << std::endl;

//    auto h = mycv::transform::ransac(first_blobs, second_blobs, matches);
//    for (auto&& d : h) {
//        std::cout << d << std::endl;
//    }

    auto q = mycv::transform::hough(first_image_in, second_image_in, first_blobs, second_blobs,
                                    first_angles, second_angles, matches);

    mycv::qimg::showDescriptors(first_image_in, second_image_in, first_blobs, second_blobs,
        first_angles, second_angles, matches, 255, 128)
            ->save("/Users/ScanNorOne/Desktop/merged.png");

    mycv::qimg::showBlobs(first_image_in, first_blobs, first_angles)
            ->save("/Users/ScanNorOne/Desktop/b1.png");
    mycv::qimg::showBlobs(second_image_in, second_blobs, second_angles)
            ->save("/Users/ScanNorOne/Desktop/b2.png");

//    mycv::qimg::getPanorama(first_image_in, second_image_in, h)->save("/Users/ScanNorOne/Desktop/panorama.png");

    return 0;
}
