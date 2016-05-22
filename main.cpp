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

int main() {

    QImage first_qimage_in("/Users/ScanNorOne/Desktop/q.png");
    mycv::CMyImage first_image_in(first_qimage_in);
    QImage second_qimage_in("/Users/ScanNorOne/Desktop/q2.png");
    mycv::CMyImage second_image_in(second_qimage_in);

    auto first_descriptors = mycv::desc::getDescriptors(first_image_in, 16, 8);
    auto second_descriptors = mycv::desc::getDescriptors(second_image_in, 16, 8);

    auto matches = mycv::desc::getMatches(std::get<0>(first_descriptors), std::get<0>(second_descriptors));

    std::cout << matches.size() << std::endl;

    mycv::qimg::showDescriptors(first_image_in, second_image_in, std::get<2>(first_descriptors), std::get<2>(second_descriptors),
        std::get<1>(first_descriptors), std::get<1>(second_descriptors), 16,
        matches, 255, 128)->save("/Users/ScanNorOne/Desktop/merged.png");

    mycv::qimg::showBlobs(first_image_in, std::get<2>(first_descriptors), std::get<1>(first_descriptors))
            ->save("/Users/ScanNorOne/Desktop/b1.png");
    mycv::qimg::showBlobs(second_image_in, std::get<2>(second_descriptors), std::get<1>(second_descriptors))
            ->save("/Users/ScanNorOne/Desktop/b2.png");

    return 0;
}
