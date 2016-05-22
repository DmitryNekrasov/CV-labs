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

    QImage qimage_in("/Users/ScanNorOne/Desktop/test_img.png");
    mycv::CMyImage image_in(qimage_in);

    auto blobs = mycv::desc::getBlobs(image_in);

    std::cout << "Blobs count = " << blobs.size() << std::endl;

    mycv::qimg::showBlobs(image_in, blobs)->save("/Users/ScanNorOne/Desktop/blobs.png");

    return 0;
}
