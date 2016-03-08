#include <iostream>

#include "array.h"
#include "core.h"
#include "defs.h"
#include "matrix.h"
#include "myimage.h"
#include "simple.h"

static bool g_BuildMinusOneOctave = false;

static const int min_size = 64;

int main() {

    QImage qimage_in("/Users/ScanNorOne/Desktop/laferrari.jpg");
    mycv::CMyImage image_in(qimage_in);

    int s = 3;
    double sigma_a = 0.5;
    double sigma_0 = 1.6;
    auto n = int(std::min(std::log2(double(image_in.getHeight()) / min_size),
                      std::log2(double(image_in.getWidth()) / min_size))) + 1;

    mycv::GaussPyramidT gauss_pyramid;

    if (!g_BuildMinusOneOctave) {
        gauss_pyramid = mycv::getGaussPyramid(image_in, n, s, sigma_a, sigma_0);
    } else {
        auto upscale = mycv::getUpscale(image_in);
        gauss_pyramid = mycv::getGaussPyramid(upscale, n + 1, s, sigma_a, sigma_0);
        for (auto& layer : gauss_pyramid) {
            std::get<toUType(mycv::PyramidLayer::EffectiveSigma)>(layer) /= 2;
        }
    }

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

    int y = 320;
    int x = 305;
    auto intensity1 = mycv::getL(image_in, gauss_pyramid, y, x, 12.0);
    auto intensity2 = mycv::getL(image_in, gauss_pyramid, y, x, 1.0);

    std::cout << intensity1 << " " << intensity2 << std::endl;

    image_in.toQImagePtr()->save("/Users/ScanNorOne/Desktop/grayscale.png");

    return 0;
}
