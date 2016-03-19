#include "qimagegenerator.h"

#include <QPainter>

#include "simple.h"

namespace mycv {

namespace qimg {

static int convertToInt(double _intensity) {
    return int(_intensity * 255);
}

QImagePtrT toQImagePtr(const CMyImage& _image) {
    auto qimage_ptr = std::make_unique<QImage>(_image.getWidth(), _image.getHeight(), QImage::Format_RGB32);
    for (int i = 0, ei = _image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = _image.getWidth(); j < ej; j++) {
            auto gray = convertToInt(_image.get(i, j));
            assert(gray >= 0 && gray < 256 && "NOT NORMALIZED VALUE");
            qimage_ptr->setPixel(j, i, qRgb(gray, gray, gray));
        }
    }
    return qimage_ptr;
}

QImagePtrT drawPoints(const CMyImage& _image, const poi::PointsT& _points) {

    auto qimage = std::make_unique<QImage>(*toQImagePtr(_image));

    QPainter painter(qimage.get());
    painter.setPen(Qt::red);

    for (const auto& point : _points) {
        auto y = std::get<toUType(Poi::X)>(point);
        auto x = std::get<toUType(Poi::Y)>(point);
        painter.drawPoint(x - 1, y);
        painter.drawPoint(x, y - 1);
        painter.drawPoint(x, y + 1);
        painter.drawPoint(x + 1, y);
    }

    return qimage;
}

} // qimg

} // mycv
