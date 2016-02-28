#include <iostream>

#include "core.h"
#include "defs.h"
#include "matrix.h"
#include "myimage.h"
#include "simple.h"

int main() {

    QImage qimage_in("/Users/ScanNorOne/Desktop/laferrari.jpg");
    mycv::CMyImage image_in(qimage_in, mycv::BorderEffect::Clamp);

    double sigma = 1;
    auto gauss_kernel = mycv::getGaussKernel(sigma);
    auto gauss = mycv::applyConvolution(image_in, gauss_kernel);

    std::cout << gauss_kernel.getHeight() << " " << gauss_kernel.getWidth() << std::endl;

    double sum = 0;
    for (int i = 0, ei = gauss_kernel.getHeight(); i < ei; i++) {
        for (int j = 0, ej = gauss_kernel.getWidth(); j < ej; j++) {
            double v = gauss_kernel.get(i, j);
            sum += v;
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "sum = " << sum << std::endl;

    image_in.toQImagePtr()->save("/Users/ScanNorOne/Desktop/grayscale.png");
    gauss.toQImagePtr()->save("/Users/ScanNorOne/Desktop/gauss.png");

    return 0;
}
