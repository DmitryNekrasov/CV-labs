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

static bool g_BuildMinusOneOctave = false;
static const int g_MinSize = 16;

int main() {

    QImage qimage_in("/Users/ScanNorOne/Desktop/q.png");
    mycv::CMyImage image_in(qimage_in);

    size_t s = 8;
    double sigma_a = 0.5;
    double sigma_0 = 1.6;
    auto n = size_t(std::min(std::log2(double(image_in.getHeight()) / g_MinSize),
                      std::log2(double(image_in.getWidth()) / g_MinSize))) + 1;

    auto gauss_pyramid = mycv::getGaussPyramid(image_in, n, s, sigma_a, sigma_0, g_BuildMinusOneOctave);

    int y = 20;
    int x = 205;
    auto intensity1 = mycv::getL(gauss_pyramid, y, x, 25.0);
    auto intensity2 = mycv::getL(gauss_pyramid, y, x, 0.0);

    std::cout << intensity1 << " " << intensity2 << std::endl;

    auto dog = mycv::getDog(gauss_pyramid);
    mycv::savePyramid(dog, "/Users/ScanNorOne/Desktop/");

    return 0;
}
