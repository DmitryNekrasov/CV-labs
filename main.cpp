#include <iostream>

#include "array.h"
#include "core.h"
#include "defs.h"
#include "matrix.h"
#include "myimage.h"
#include "simple.h"

int main() {

    QImage qimage_in("/Users/ScanNorOne/Desktop/laferrari.jpg");
    mycv::CMyImage image_in(qimage_in);

    int n = 3;
    int s = 3;
    double sigma_a = 0.5;
    double sigma_0 = 1;

    auto gauss_pyramid = mycv::getGaussPyramid(image_in, n, s, sigma_a, sigma_0);

    int num = 0;
    for (const auto& layer : gauss_pyramid) {

        auto current_sigma = std::get<toUType(mycv::PyramidLayer::CurrentSigma)>(layer);
        auto effective_sigma = std::get<toUType(mycv::PyramidLayer::EffectiveSigma)>(layer);
        auto& img = std::get<toUType(mycv::PyramidLayer::Image)>(layer);

        std::string name = "/Users/ScanNorOne/Desktop/" +
                std::to_string(++num) + "img_" +
                std::to_string(current_sigma) + "_" +
                std::to_string(effective_sigma) + ".png";

        img.toQImagePtr()->save(name.c_str());
    }

    image_in.toQImagePtr()->save("/Users/ScanNorOne/Desktop/grayscale.png");

    return 0;
}
